#include <iostream>

#include "HelpCommand.h"
#include "../../printer/Printer.h"

HelpCommand::HelpCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "help";
    commandInfo.description = "Get info about Ogy and view a summary of the available commands.";
    commandInfo.numArgs = 0;
    commandInfo.numFlags = 0;
}

void HelpCommand::execute()
{
    Printer::print("\n------------------------------", 0, TextColor::WHITE, TextEmphasis::BOLD);
    std::cout << "\n\n";
    Printer::print("Ogy is your very own command line companion!\n\n", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Its purpose is to offer you a range of helpful commands that will make your life easier. Some of these commands are improved versions of existing commands while others offer new functionalities that make working with the command line more convenient.\n", 0, TextColor::WHITE, TextEmphasis::BOLD);
    
    Printer::print("\nAvailable commands:", 0, TextColor::WHITE, TextEmphasis::BOLD_UNDERLINED);
    std::cout << "\n\n";
    Printer::print("`ogy help` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Get info about Ogy and view a summary of the available commands.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("`ogy info {file name} (-rec)` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Show info about the specified file.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("`ogy ls (-all) (-rec) (-mt)` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("List info about items in the current directory. Include the `-all` flag to include hidden items. Include the `-mt` flag to use mutlithreading.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("`ogy find {term} (-rec)` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Find all files in the current directory which include `term` in their file name.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("`ogy cd {alias} {path}` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Change directory. If the alias exists, go to its corresponding path. Otherwise store the path as the alias in the config file and go to the specified path. Alias is optional, so it could also be used as the built-in cd command.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("`ogy create {template name}` - ", 0, TextColor::WHITE, TextEmphasis::BOLD);
    Printer::print("Create a new project directory based on the project structure specified in the config file.", 0, TextColor::WHITE, TextEmphasis::NORMAL);
    std::cout << "\n\n";
    Printer::print("Author: devran", 0, TextColor::GRAY, TextEmphasis::BOLD);
    Printer::print("\n\n------------------------------", 0, TextColor::WHITE, TextEmphasis::BOLD);
    std::cout << "\n\n";
}

bool HelpCommand::hasValidArgsAndFlags()
{
    if (args.size() > commandInfo.numArgs)
    {
        errorMessage = "`help` command doesn't accept any arguments.\n";
        return false;
    }
    if (flags.size() > commandInfo.numFlags)
    {
        errorMessage = "`help` command doesn't accept any flags.\n";
        return false;
    }
    return true;
}
