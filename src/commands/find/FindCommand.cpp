#include <algorithm>
#include <cctype>
#include <iostream>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>


#include "FindCommand.h"

FindCommand::FindCommand(int argc, char** argv)
    : Command(argc, argv)
{
    commandInfo.name = "find";
    commandInfo.description = "Find all files in the current directory which include `term` in their file name.";
    commandInfo.numArgs = 1;
    commandInfo.numFlags = 1;
}

void FindCommand::execute()
{
    if (containsFlag("-rec")) findFiles<RecDirIterator>();
    else findFiles<DirIterator>();
}

CommonFileInfo FindCommand::setFileInfo(const struct stat& fileStat, const Path& entryPath)
{
    CommonFileInfo info;
    // Get permissions for the file
    info.permissions = Command::getPermissions(fileStat);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileStat.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileStat.st_uid)->pw_name ? getpwuid(fileStat.st_uid)->pw_name : "-";
    // Get file size in bytes or number of bytes allocated to directory
    info.size = std::to_string(fileStat.st_size);
    // Get time and date of last modification
    info.lastModified = Command::getLastModified(fileStat);
    // Get file name from args
    info.name = entryPath.filename();

    return info;
}

bool FindCommand::hasValidArgsAndFlags()
{
    if (args.size() < commandInfo.numArgs)
    {
        errorMessage = "No arguments passed to 'find' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    else if (args.size() > commandInfo.numArgs)
    {
        errorMessage = "Too many arguments passed to 'find' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    else if (flags.size() > commandInfo.numFlags)
    {
        errorMessage = "Too many flags passed to 'find' command. Use 'ogy help' to view the expected flags.\n";
        return false;
    }

    return true;
}