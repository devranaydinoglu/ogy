#include <iostream>
#include <vector>

#include "Command.h"
#include "./info//InfoCommand.h"
#include "./list/ListCommand.h"

Command::Command(int argc, char** argv)
    : errorMessage("")
{
    this->argc = argc;
    this->argv = argv;

    command = argv[1];
    setArgsAndFlags();

    std::cout << "------------------------------\n";
    std::cout << "Command:\n";
    std::cout << "Number of command arguments passed:" << argc - 1 << "\n";
    std::cout << "command: " << command << "\n";
    for (int i = 0; i < args.size(); i++) std::cout << "args: " << args[i] << "\n";
    for (int i = 0; i < flags.size(); i++) std::cout << "flags: " << flags[i] << "\n";
    std::cout << "------------------------------\n\n";
}

void Command::determineCommand()
{
    const CommandType commandType = stringToCommandTypeMapping[command];

    switch (commandType)
    {
        case CommandType::NONE:
            std::cout << "Command not recognized.\n";
            break;
        case CommandType::HELP:
            std::cout << "Command passed: help\n";
            break;
        case CommandType::CREATE:
            std::cout << "Command passed: create\n";
            break;
        case CommandType::CD:
            std::cout << "Command passed: cd\n";
            break;
        case CommandType::INFO:
        {
            InfoCommand infoCom(argc, argv);

            if (!infoCom.hasValidArgsAndFlags())
            {
                std::cout << infoCom.errorMessage;
                return;
            }
            
            infoCom.execute();
            return;
        }
        case CommandType::LS:
        {
            std::cout << "Command passed: ls\n";

            ListCommand listCom(argc, argv);

            if (!listCom.hasValidArgsAndFlags())
            {
                std::cout << listCom.errorMessage;
                return;
            }

            listCom.execute();
            return;
        }
        case CommandType::FIND:
            std::cout << "Command passed: find\n";
            break;
        default:
            std::cout << "No valid command passed\n";
            break;
    }
}

void Command::setArgsAndFlags()
{
    for (size_t i = 2; i < argc; i++)
    {
        if (argv[i][0] == '-') flags.emplace_back(argv[i]);
        else args.emplace_back(argv[i]);
    }
}

bool Command::containsFlag(const std::string& flag)
{
    for (const auto& f : flags)
    {
        if (f == flag) return true;
    }

    return false;
}
