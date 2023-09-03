#include <iostream>
#include <vector>
#include <sys/stat.h>

#include "Command.h"
#include "./info//InfoCommand.h"
#include "./list/ListCommand.h"
#include "./help/HelpCommand.h"
#include "./find/FindCommand.h"
#include "../printer/Printer.h"

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
        {
            FindCommand findCom(argc, argv);

            if (!findCom.hasValidArgsAndFlags())
            {
                std::cout << findCom.errorMessage;
                return;
            }

            findCom.execute();
            return;
        }
        default:
            std::cout << "No valid command passed\n";
            return;
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

void Command::printCommonHeaders(const CommonFileInfoPadding& infoPadding)
{
    int padding = 2;

    Printer::print("Permissions", infoPadding.permissionsPadding + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Links", infoPadding.numLinksPadding + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Owner", infoPadding.ownerPadding + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Size",  infoPadding.sizePadding + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Last Modified", infoPadding.lastModifiedPadding + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("File Name", infoPadding.namePadding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    std::cout << std::endl;
}

void Command::printCommonFileInfo(const CommonFileInfo& info, const CommonFileInfoPadding& infoPadding)
{
    int padding = 2;
    std::cout << infoPadding.sizePadding;
    Printer::print(info.permissions, infoPadding.permissionsPadding + padding, TextColor::GREEN, TextEmphasis::BOLD);
    Printer::print(info.numLinks, infoPadding.numLinksPadding + padding, TextColor::MAGENTA, TextEmphasis::BOLD);
    Printer::print(info.owner, infoPadding.ownerPadding + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.size, infoPadding.sizePadding + padding, TextColor::YELLOW, TextEmphasis::BOLD);
    Printer::print(info.lastModified, infoPadding.lastModifiedPadding + padding, TextColor::GREEN, TextEmphasis::BOLD);
    Printer::print(info.name, infoPadding.namePadding + padding, TextColor::MAGENTA, TextEmphasis::BOLD);
    std::cout << "\n";
}

CommonFileInfoPadding Command::getCommonFileInfoPadding(const CommonFileInfo& info)
{
    CommonFileInfoPadding padding;

    padding.permissionsPadding = std::max(std::string("Permissions").length(), info.permissions.length());
    padding.numLinksPadding = std::max(std::string("Links").length(), info.numLinks.length());
    padding.ownerPadding = std::max(std::string("Owner").length(), info.owner.length());
    padding.sizePadding = std::max(std::string("Size").length(), info.size.length());
    padding.lastModifiedPadding = std::max(std::string("Last Modified").length(), info.lastModified.length());
    padding.namePadding = std::max(std::string("File Name").length(), info.name.length());

    return padding;
}

std::string Command::getLastModified(const struct stat& fileStat)
{
    char buffer[30];
    std::strftime(buffer, sizeof buffer, "%a %d %b %Y at %H:%M", std::localtime(&fileStat.st_mtimespec.tv_sec));

    std::string lastModified = buffer;

    return lastModified;
}

std::string Command::getPermissions(const struct stat& fileStat)
{
    std::string permsAsString;
    mode_t perm = fileStat.st_mode;
    
    // Check for directory
    permsAsString += S_ISDIR(perm) ? "d" : "-";
    permsAsString += " ";

    // Check for owner permissions
    permsAsString += (perm & S_IRUSR) ? "r" : "-";
    permsAsString += (perm & S_IWUSR) ? "w" : "-";
    permsAsString += (perm & S_IXUSR) ? "x" : "-";
    permsAsString += " ";

    // Check for group permissions
    permsAsString += (perm & S_IRGRP) ? "r" : "-";
    permsAsString += (perm & S_IWGRP) ? "w" : "-";
    permsAsString += (perm & S_IXGRP) ? "x" : "-";
    permsAsString += " ";

    // Check for other permissions
    permsAsString += (perm & S_IROTH) ? "r" : "-";
    permsAsString += (perm & S_IWOTH) ? "w" : "-";
    permsAsString += (perm & S_IXOTH) ? "x" : "-";

    return permsAsString;
}