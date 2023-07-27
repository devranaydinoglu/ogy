#pragma once

#include <string>
#include <map>
#include <string_view>

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
    std::map<int, std::string> args;
};

class Command
{
public:
    // Command() {};
    Command(int argc, char** argv);

    inline static std::map<std::string, CommandType> stringToCommandTypeMapping = {
        {"help", CommandType::HELP},
        {"create", CommandType::CREATE},
        {"cd", CommandType::CD},
        {"info", CommandType::INFO},
        {"ls", CommandType::LS},
        {"find", CommandType::FIND}
    };

protected:
    CommandInfo info;
    std::string command;
    std::vector<std::string> args; 
    std::vector<std::string> flags;
    std::string errorMessage;

    // These need to be stored to pass them to child classes
    int argc;
    char** argv;

public:
    void determineCommand();

protected:
    virtual void execute() {};
    virtual bool hasValidArgsAndFlags() {return false;};
    bool containsFlag(std::string_view flag);

private:
    void setArgsAndFlags();
};
