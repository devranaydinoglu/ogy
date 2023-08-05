#include <iostream>
#include <vector>

#include "Command.h"
#include "./info//InfoCommand.h"
#include "./list/ListCommand.h"
#include "./help/HelpCommand.h"

Command::Command(int argc, char** argv)
    : errorMessage(""), commandInfo()
{
    this->argc = argc;
    this->argv = argv;

    command = argv[1];
    setArgsAndFlags();

    // std::cout << "------------------------------\n";
    // std::cout << "Command:\n";
    // std::cout << "Number of command arguments passed:" << argc - 1 << "\n";
    // std::cout << "command: " << command << "\n";
    // for (int i = 0; i < args.size(); i++) std::cout << "args: " << args[i] << "\n";
    // for (int i = 0; i < flags.size(); i++) std::cout << "flags: " << flags[i] << "\n";
    // std::cout << "------------------------------\n\n";
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
        {
            HelpCommand helpCom(argc, argv);

            if (!helpCom.hasValidArgsAndFlags())
            {
                std::cout << helpCom.errorMessage;
                return;
            }

            helpCom.execute();
            return;
        }
        
        case CommandType::CREATE:
            break;
        case CommandType::CD:
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

bool Command::containsFlag(std::string_view flag)
{
    for (const auto& f : flags)
    {
        if (f == flag) return true;
    }

    return false;
}
