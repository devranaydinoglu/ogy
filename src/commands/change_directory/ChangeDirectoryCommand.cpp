#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>

#include "rapidjson/filereadstream.h"
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
    std::string configFilePath = std::string(user->pw_dir) + "/.local/bin/ogy/config.json";

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
        if (hasAlias)
        {
            if (containsKey(doc.GetObject(), alias.c_str()))
            {
                std::cout << recursiveValueSearch(doc.GetObject(), alias.c_str()) << "\n";
                return;
            }
            std::cout << "Invalid alias or path";
        }

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
                if (isValidPath(newPath))
                {
                    std::cout << "2. Path: change dir and update config\n";
                    return;
                }
                std::cout << "Invalid path\n";
            }
            else
            {
                if (isValidPath(newPath))
                {
                    std::cout << "2. Add alias and path to config\n";
                    return;
                }
                std::cout << "Invalid path\n";
            }
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

bool ChangeDirectoryCommand::containsValue(const rj::Value &val, const char *value)
{
    for (const auto& v : val.GetObject())
    {
        std::string stringVal = "";
        if (v.value.IsString())
        {
            stringVal = v.value.GetString();
        }
        if (v.value.IsBool())
        {
            stringVal = std::to_string(v.value.GetBool());
        }
        if (v.value.IsInt())
        {
            stringVal = std::to_string(v.value.GetInt());
        }
        if (stringVal == value)
        {
            return true;
        }

        if (v.value.IsObject())
        {
            bool found = containsValue(v.value.GetObject(), value);
            if (found) return true;
        }
        if (v.value.IsArray())
        {
            for (const auto& elem : v.value.GetArray())
            {
                bool found = containsValue(elem.GetObject(), value);
                if (found) return true;
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
