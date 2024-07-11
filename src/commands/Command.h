#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

enum class CommandType
{
    NONE,
    HELP,
    CREATE,
    CD,
    INFO,
    LS,
    FIND
};

struct CommandInfo
{
    std::string name;
    std::string description;
    int numArgs;
    int numFlags;
};

/**
* Used to store data of a file
*/
struct CommonFileInfo
{
    std::string permissions;
    std::string numLinks;
    std::string owner;
    std::string size;
    std::string lastModified;
    std::string name;
};

/**
* Used to add padding to the command output. Holds the max number of characters between the info and its corresponding header
*/
struct CommonFileInfoPadding
{
    int permissionsPadding;
    int numLinksPadding;
    int ownerPadding;
    int sizePadding;
    int lastModifiedPadding;
    int namePadding;
};

class Command
{
public:
    Command(int argc, char** argv);

    std::map<std::string, CommandType> stringToCommandTypeMapping = {
        {"help", CommandType::HELP},
        {"create", CommandType::CREATE},
        {"cd", CommandType::CD},
        {"info", CommandType::INFO},
        {"ls", CommandType::LS},
        {"find", CommandType::FIND}
    };

protected:
    CommandInfo commandInfo;
    std::string command;
    std::vector<std::string> args; 
    std::vector<std::string> flags;
    std::string errorMessage;
    static const int defaultPadding = 2;

    // These need to be stored to pass them to child classes
    int argc;
    char** argv;

public:
    void determineCommand();

protected:
    virtual void execute() {};
    virtual bool hasValidArgsAndFlags() {return false;};
    bool containsFlag(std::string_view flag);
    static void printCommonHeaders(const CommonFileInfoPadding& infoPadding);
    static void printCommonFileInfo(const CommonFileInfo& info, const CommonFileInfoPadding& infoPadding);
    static CommonFileInfoPadding getCommonFileInfoPadding(const CommonFileInfo& info);
    static std::string getLastModified(const struct stat& fileStat);
    static std::string getPermissions(const struct stat& fileStat);

private:
    void setArgsAndFlags();
};
