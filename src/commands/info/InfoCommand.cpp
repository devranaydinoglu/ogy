#include <algorithm>
#include <iostream>
#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>

#include "InfoCommand.h"
#include "../../utils/Math.h"
#include "../../printer/Printer.h"

using DirIterator = std::filesystem::directory_iterator;
using RecDirIterator = std::filesystem::recursive_directory_iterator;
using Path = std::filesystem::path;

InfoCommand::InfoCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "info";
    commandInfo.description = "Show info about the specified file.";
    commandInfo.numArgs = 1;
    commandInfo.numFlags = 1;
}

void InfoCommand::execute()
{
    struct stat fileStat;
    
    // Check if valid file info has been returned
    if (stat(args[0].c_str(), &fileStat) != 0) 
    {
        std::cout << "File error: " << strerror(errno) << "\n";
        return;
    }

    // Set file info
    CommonFileInfo info = setFileInfo(fileStat);
    
    // Get the length of the longest string to set the width of each column (to line them up)
    CommonFileInfoPadding padding = Command::getCommonFileInfoPadding(info);

    // Print headers
    Command::printCommonHeaders(padding);

    // Print info of each header
    Command::printCommonFileInfo(info, padding);
}

CommonFileInfo InfoCommand::setFileInfo(const struct stat& fileStat)
{
    CommonFileInfo info;
    // Get permissions for the file
    info.permissions = Command::getPermissions(fileStat);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileStat.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileStat.st_uid)->pw_name ? getpwuid(fileStat.st_uid)->pw_name : "-";

    // Get file name from args
    std::string fileName;
    Path filePath;

    for (const auto& entry : DirIterator(std::filesystem::current_path(), std::filesystem::directory_options::skip_permission_denied))
    {
        std::string tempFileName = entry.path().filename();

        // Convert file name to all uppercases and lowercases to support uppercase and lowercase user input
        std::string fileNameUpper = tempFileName;
        std::transform(fileNameUpper.begin(), fileNameUpper.end(), fileNameUpper.begin(), ::toupper);
        std::string fileNameLower = tempFileName;
        std::transform(fileNameLower.begin(), fileNameLower.end(), fileNameLower.begin(), ::tolower);

        std::string argsUpper = args[0];
        std::transform(argsUpper.begin(), argsUpper.end(), argsUpper.begin(), ::toupper);

        if (tempFileName == args[0] || fileNameLower == args[0] || fileNameUpper == argsUpper)
        {
            fileName = tempFileName;
            filePath = entry.path();
        }
    }

    info.name = fileName;

    // Get file size in bytes or number of bytes allocated to directory
    off_t totalSize = 0;
    mode_t perm = fileStat.st_mode;

    if (S_ISDIR(perm) && containsFlag("-rec"))
    {
        for (const auto& entry : RecDirIterator(filePath, std::filesystem::directory_options::skip_permission_denied))
        {
            struct stat fileStat;

            // Check if valid file info has been returned
            if (stat(entry.path().c_str(), &fileStat) != 0)
            {
                break;
            }
            totalSize += fileStat.st_size; 
        }
    }
    else
    {
        totalSize = fileStat.st_size;
    }

    info.size = std::to_string(totalSize);

    // Get time and date of last modification
    info.lastModified = Command::getLastModified(fileStat);

    return info;
}

bool InfoCommand::hasValidArgsAndFlags()
{
    if (args.size() < commandInfo.numArgs)
    {
        errorMessage = "No arguments passed to 'info' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (args.size() > commandInfo.numArgs)
    {
        errorMessage = "Too many arguments passed to 'info' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (flags.size() > commandInfo.numFlags)
    {
        errorMessage = "'info' command doesn't accept any flags. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }

    return true;
}