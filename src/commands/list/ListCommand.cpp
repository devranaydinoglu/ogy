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
    commandInfo.numFlags = 1;
}

void ListCommand::execute()
{
    Path currentPath = std::filesystem::current_path();
    bool printHeader = true;
    
    std::vector<FileInfo> filesInfo;

    for (const auto& entry : DirIterator(currentPath, std::filesystem::directory_options::skip_permission_denied))
    {
        
        struct stat fileStat;
        
        // Check if valid file info has been returned
        if (stat(entry.path().c_str(), &fileStat) != 0)
        {
            std::cout << "File error: " << strerror(errno) << "\n";
            return;
        }
        
        std::string fileName = entry.path().filename();

        if (fileName[0] == '.' && !containsFlag("-all")) continue;

        filesInfo.emplace_back(setFileInfo(fileStat, entry.path()));

    }

    if (filesInfo.size() < 1) return;

    Printer::print("Current Path: ", 0, TextColor::GRAY, TextEmphasis::NORMAL);
    Printer::print(currentPath.string() + "\n", 0, TextColor::WHITE, TextEmphasis::NORMAL);

    int permissionsMax = 11;
    int numLinksMax = 5;
    int ownerMax = 5;
    int sizeMax = 4;
    int lastModifiedMax = 13;
    int nameMax = 9;

    // Get the length of the longest string to set the width of each column (to line them up)
    for (const auto& info : filesInfo)
    {
        permissionsMax = std::max(permissionsMax, static_cast<int>(info.permissions.length()));
        numLinksMax = std::max(numLinksMax, static_cast<int>(info.numLinks.length()));
        ownerMax = std::max(ownerMax, static_cast<int>(info.owner.length()));
        sizeMax = std::max(sizeMax, static_cast<int>(info.size.length()));
        lastModifiedMax = std::max(lastModifiedMax, static_cast<int>(info.lastModified.length()));
        nameMax = std::max(nameMax, static_cast<int>(info.name.length()));
    }

    int padding = 2;

    // Print headers
    Printer::print(" ", 2 + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Permissions", permissionsMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Links", numLinksMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Owner", ownerMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Size",  sizeMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Last Modified", lastModifiedMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("File Name", nameMax, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    std::cout << std::endl;

    for (size_t i = 0; i < filesInfo.size(); i++)
    {
        Printer::print(std::to_string(i + 1), 2 + padding, TextColor::GRAY, TextEmphasis::BOLD);
        // Print info of each header
        Printer::print(filesInfo[i].permissions, permissionsMax + padding, TextColor::GREEN, TextEmphasis::BOLD);
        Printer::print(filesInfo[i].numLinks, numLinksMax + padding, TextColor::MAGENTA, TextEmphasis::BOLD);
        Printer::print(filesInfo[i].owner, ownerMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
        Printer::print(filesInfo[i].size, sizeMax + padding, TextColor::YELLOW, TextEmphasis::BOLD);
        Printer::print(filesInfo[i].lastModified, lastModifiedMax + padding, TextColor::GREEN, TextEmphasis::BOLD);
        Printer::print(filesInfo[i].name, nameMax, TextColor::MAGENTA, TextEmphasis::BOLD);
        std::cout << "\n";
    }
}

std::string ListCommand::getLastModified(const struct stat &fileStat)
{
    char buffer[30];
    std::strftime(buffer, sizeof buffer, "%a %d %b %Y at %H:%M", std::localtime(&fileStat.st_mtimespec.tv_sec));

    std::string lastModified = buffer;

    return lastModified;
}

std::string ListCommand::getPermissions(const struct stat &fileStat)
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

FileInfo ListCommand::setFileInfo(const struct stat& fileStat, const Path& entryPath)
{
    FileInfo info;
    // Get permissions for the file
    info.permissions = getPermissions(fileStat);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileStat.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileStat.st_uid)->pw_name ? getpwuid(fileStat.st_uid)->pw_name : "-";

    // Get file or directory size in bytes
    off_t totalSize = 0;
    mode_t perm = fileStat.st_mode;

    if (S_ISDIR(perm))
    {
        for (const auto& entry : RecDirIterator(entryPath, std::filesystem::directory_options::skip_permission_denied))
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
    info.lastModified = getLastModified(fileStat);
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
