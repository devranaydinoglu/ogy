#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string>
#include <string_view>

#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "ChangeDirectoryCommand.h"

ChangeDirectoryCommand::ChangeDirectoryCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "cd";
    commandInfo.description = "Change directory based on the configured alias.";
    commandInfo.numArgs = 2;
    commandInfo.numFlags = 0;
}

void ChangeDirectoryCommand::execute()
{
    rj::Document doc;
    struct stat configFileStat;
    
    struct passwd* user = getpwuid(getuid());
    configFilePath = std::string(user->pw_dir) + "/.local/bin/ogy/config.json";

    // Check if the config file exists at the install directory
    if (stat(configFilePath.c_str(), &configFileStat) != 0)
    {
        std::cout << "Config file error: " << strerror(errno) << "\n";
        return;
    }

    readConfigFile(doc, configFilePath.c_str());
    assert(doc.IsObject());

    determineArgs(doc);
    
    bool hasAlias = !alias.empty();
    bool hasPath = !path.empty();

    std::string newPath = getCurrentPath() + "/" + path;

    if (args.size() == 1)
    {
        // Alias was provided, so check if it exists in the config file
        if (hasAlias)
        {
            if (containsKey(doc.GetObject(), alias.c_str()))
            {
                std::cout << recursiveValueSearch(doc.GetObject(), alias.c_str()) << "\n";
                return;
            }
            std::cout << "Invalid alias or path";
        }

        // Path was provided, so change to specified path
        if (hasPath)
        {
            if (isValidPath(newPath))
            {
                std::cout << newPath;
                return;
            }
            std::cout << "Invalid alias or path";
        }
    }
    else if (args.size() == 2)
    {
        if (hasAlias && hasPath)
        {
            if (containsKey(doc.GetObject(), alias.c_str()))
            {
                // Alias exists, so update its path in the config file and change to dir
                if (isValidPath(newPath))
                {
                    if (doc.HasMember("paths") && doc["paths"].IsArray())
                    {
                        updatePathInConfig(doc, newPath);
                    }
                }
                std::cout << "Invalid path\n";
                return;
            }
            else
            {
                // Alias doesn't exist, so add it to the config file with the specified path and change to dir
                if (isValidPath(newPath))
                {
                    addPathToConfig(doc, newPath);
                    return;
                }
                std::cout << "Invalid path\n";
                return;
            }
        }
        else if (!hasPath)
        {
            std::cout << "Invalid path";
        }
        else
        {
            std::cout << "Invalid path";
        }
    }
    else
    {
        std::cout << "Invalid args\n";
    }
}

bool ChangeDirectoryCommand::hasValidArgsAndFlags()
{
    if (args.size() < commandInfo.numArgs - 1)
    {
        errorMessage = "No arguments passed to 'cd' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    else if (args.size() > commandInfo.numArgs)
    {
        errorMessage = "Too many arguments passed to 'cd' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    else if (flags.size() > commandInfo.numFlags)
    {
        errorMessage = "Too many flags passed to 'cd' command. Use 'ogy help' to view the expected flags.\n";
        return false;
    }

    return true;
}

void ChangeDirectoryCommand::determineArgs(const rj::Document& doc)
{
    bool isAlias = false;
    bool isPath = false;

    for (const std::string& arg : args)
    {
        // Check if arg is a path
        if (!isPath)
        {
            bool isValidDir = isValidPath(arg);

            if (isValidDir)
            {
                isPath = true;
                path = arg;
            }
        }

        // Check if arg is an alias
        if (args.size() == 1 && isPath)
        {
            return;
        }

        if (!isAlias)
        {
            if (arg.find("/") == std::string::npos)
            {
                isAlias = true;
                alias = arg;
            }
        }
    }
}

void ChangeDirectoryCommand::readConfigFile(rj::Document& doc, const char* fileName)
{
    FILE* file = fopen(fileName, "r");
    
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[fileSize];
    rj::FileReadStream frs(file, buffer, sizeof(buffer));

    doc.ParseStream(frs);
    fclose(file);
}

std::string ChangeDirectoryCommand::recursiveValueSearch(const rj::Value &val, const char *key)
{
    for (const auto& v : val.GetObject())
    {
        if (v.name == key)
        {
            if (v.value.IsString())
            {
                return v.value.GetString();
            }
            if (v.value.IsBool())
            {
                return std::to_string(v.value.GetBool());
            }
            if (v.value.IsInt())
            {
                return std::to_string(v.value.GetInt());
            }
        }
        else
        {    
            if (v.value.IsObject())
            {
                std::string valueFound = recursiveValueSearch(v.value.GetObject(), key);
                if (valueFound.size() > 0) return valueFound;
            }
            if (v.value.IsArray())
            {
                for (const auto& elem : v.value.GetArray())
                {
                    std::string valueFound = recursiveValueSearch(elem.GetObject(), key);
                    if (valueFound.size() > 0) return valueFound;
                }
            }
        }
    }
    return "";
}

bool ChangeDirectoryCommand::containsKey(const rj::Value &val, const char *key)
{
    for (const auto& v : val.GetObject())
    {
        if (v.name == key)
        {
            return true;
        }
        else
        {    
            if (v.value.IsObject())
            {
                bool found = containsKey(v.value.GetObject(), key);
                if (found) return true; 
            }
            if (v.value.IsArray())
            {
                for (const auto& elem : v.value.GetArray())
                {
                    bool found = containsKey(elem.GetObject(), key);
                    if (found) return true;
                }
            }
        }
    }
    return false;
}

bool ChangeDirectoryCommand::isValidPath(std::string path)
{
    bool isValidDir = false;

    struct stat dirStat;
            
    if (stat(path.c_str(), &dirStat) == 0) 
    {
        if (S_ISDIR(dirStat.st_mode)) isValidDir = true;
        else std::cout << "Not a valid subdirectory in current directory";
    }

    return isValidDir;
}

std::string ChangeDirectoryCommand::getCurrentPath()
{
    char currentDir[PATH_MAX];
    if (getcwd(currentDir, sizeof(currentDir)) == nullptr)
    {
        std::cout << "Error: " << strerror(errno) << "\n";
        return "";
    }

    return std::string(currentDir);
}

void ChangeDirectoryCommand::updatePathInConfig(rj::Document& doc, std::string newPath)
{
    auto pathsArr = doc["paths"].GetArray();

    for (auto& obj : pathsArr)
    {
        if (obj.IsObject())
        {
            if (obj.HasMember(alias.c_str()))
            {
                auto itr = obj.GetObject().FindMember(alias.c_str());
                itr->value.SetString(newPath.c_str(), doc.GetAllocator());
                jsonToFile(doc, configFilePath);

                std::cout << newPath;
                return;
            }
        }
    }
}

void ChangeDirectoryCommand::addPathToConfig(rj::Document &doc, std::string newPath)
{
    rj::Document::AllocatorType& allocator = doc.GetAllocator();

    // Create new object to add to paths array
    rj::Value newObj(rj::kObjectType);
    rj::Value key(alias.c_str(), allocator);
    rj::Value value(newPath.c_str(), allocator);
    newObj.AddMember(key, value, allocator);

    doc["paths"].PushBack(newObj, allocator);
    jsonToFile(doc, configFilePath);

    std::cout << newPath;
    return;
}

void ChangeDirectoryCommand::jsonToFile(rj::Document& doc, std::string fullPath)
{
    std::ofstream outputFile;
    outputFile.open(fullPath);

    if(outputFile.is_open()) {
        std::string jsonObjectData = jsonToString(doc);
        outputFile << jsonObjectData;
    }

    outputFile.close();
}

std::string ChangeDirectoryCommand::jsonToString(rj::Document& doc)
{
	rj::StringBuffer buffer;
	rj::PrettyWriter<rj::StringBuffer> jsonWriter(buffer);
	doc.Accept(jsonWriter);

	return buffer.GetString();
}