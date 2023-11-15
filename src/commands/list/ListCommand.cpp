#include <iostream>
#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <vector>

#include "ListCommand.h"
#include "../info/InfoCommand.h"
#include "../../printer/Printer.h"

using DirIterator = std::filesystem::directory_iterator;
using RecDirIterator = std::filesystem::recursive_directory_iterator;

ListCommand::ListCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "list";
    commandInfo.description = "List info about items in the current directory. Include the `-all` flag to include hidden items.";
    commandInfo.numArgs = 0;
    commandInfo.numFlags = 2;
}

void ListCommand::execute()
{
    Path currentPath = std::filesystem::current_path();
    bool printHeader = true;
    
    std::vector<CommonFileInfo> filesInfo;

    auto it = DirIterator(currentPath, std::filesystem::directory_options::skip_permission_denied);
    try
    {
        for (auto i = std::filesystem::begin(it); i != std::filesystem::end(it); i++)
        {
            auto entry = *i;

            struct stat fileStat;
            
            // Check if valid file info has been returned
            if (stat(entry.path().c_str(), &fileStat) != 0)
            {
                std::cout << "Error: " << strerror(errno) << "\n";
                return;
            }
            
            std::string fileName = entry.path().filename();
            if (fileName[0] == '.' && !containsFlag("-all")) continue;

            filesInfo.emplace_back(setFileInfo(fileStat, entry.path()));
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    if (filesInfo.size() < 1) return;

    Printer::print("Current Path: ", 0, TextColor::GRAY, TextEmphasis::BOLD);
    Printer::print(currentPath.string() + "\n", 0, TextColor::WHITE, TextEmphasis::BOLD);

    CommonFileInfoPadding tempPadding;
    CommonFileInfoPadding padding = {};

    // Get the length of the longest string to set the width of each column (to line them up)
    for (const auto& info : filesInfo)
    {
        tempPadding = Command::getCommonFileInfoPadding(info);

        if (tempPadding.lastModifiedPadding > padding.lastModifiedPadding) padding.lastModifiedPadding = tempPadding.lastModifiedPadding;
        if (tempPadding.namePadding > padding.namePadding) padding.namePadding = tempPadding.namePadding;
        if (tempPadding.numLinksPadding > padding.numLinksPadding) padding.numLinksPadding = tempPadding.numLinksPadding;
        if (tempPadding.ownerPadding > padding.ownerPadding) padding.ownerPadding = tempPadding.ownerPadding;
        if (tempPadding.permissionsPadding > padding.permissionsPadding) padding.permissionsPadding = tempPadding.permissionsPadding;
        if (tempPadding.sizePadding > padding.sizePadding) padding.sizePadding = tempPadding.sizePadding;
    }

    // Print headers
    Printer::print(" ", defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Command::printCommonHeaders(padding);

    for (size_t i = 0; i < filesInfo.size(); i++)
    {
        Printer::print(std::to_string(i + 1), defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD);
        
        // Print info of each header
        Command::printCommonFileInfo(filesInfo[i], padding);
    }
}

CommonFileInfo ListCommand::setFileInfo(const struct stat& fileStat, const Path& entryPath)
{
    CommonFileInfo info;
    // Get permissions for the file
    info.permissions = Command::getPermissions(fileStat);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileStat.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileStat.st_uid)->pw_name ? getpwuid(fileStat.st_uid)->pw_name : "-";

    // Get file or directory size in bytes
    off_t totalSize = 0;
    mode_t perm = fileStat.st_mode;

    if (S_ISDIR(perm) && containsFlag("-rec"))
    {
        auto it = RecDirIterator(entryPath, std::filesystem::directory_options::skip_permission_denied);
        try
        {
            for (auto i = std::filesystem::begin(it); i != std::filesystem::end(it); i++)
            {
                auto entry = *i;

                struct stat fileStat;

                // Check if valid file info has been returned
                if (stat(entry.path().c_str(), &fileStat) != 0)
                {
                    break;
                }
                totalSize += fileStat.st_size;
            }
        }
        catch(std::filesystem::filesystem_error& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else
    {
        totalSize = fileStat.st_size;
    }

    info.size = std::to_string(totalSize);

    // Get time and date of last modification
    info.lastModified = Command::getLastModified(fileStat);
    // Get file name from path
    info.name = entryPath.filename();

    return info;
}

bool ListCommand::hasValidArgsAndFlags()
{
    if (args.size() > commandInfo.numArgs)
    {
        errorMessage = "Too many arguments passed to 'ls' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (flags.size() > commandInfo.numFlags)
    {
        errorMessage = "Too many flags passed to 'ls' command. Use 'ogy help' to view the expected flags.\n";
        return false;
    }

    return true;
}
