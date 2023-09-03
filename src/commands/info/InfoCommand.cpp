#include <algorithm>
#include <iostream>
#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>

#include "InfoCommand.h"
#include "../../utils/Math.h"
#include "../../printer/Printer.h"

InfoCommand::InfoCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "info";
    commandInfo.description = "Show info about the specified file.";
    commandInfo.numArgs = 1;
    commandInfo.numFlags = 0;
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
    // Get file size in bytes or number of bytes allocated to directory
    info.size = std::to_string(static_cast<int>(fileStat.st_size));
    // Get time and date of last modification
    info.lastModified = Command::getLastModified(fileStat);
    // Get file name from args
    info.name = args[0];

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