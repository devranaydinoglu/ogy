#include <iostream>
#include <string>
#include <filesystem>
#include <sys/errno.h>
#include <sys/stat.h>
#include <pwd.h>

#include "ListCommand.h"
#include "../../printer/Printer.h"

using Path = std::filesystem::path;
using DirIterator = std::filesystem::directory_iterator;

ListCommand::ListCommand(int argc, char** argv)
    : Command(argc, argv)
{
}

void ListCommand::execute()
{
    Path currentPath = std::filesystem::current_path();
    bool printHeader = true;
    int filesCounter = 0;

    for (const auto& entry : DirIterator(currentPath))
    {
        std::string fileName = entry.path().filename();
        
        struct stat fileStat;
        
        // Check if valid file info has been returned
        if (stat(fileName.c_str(), &fileStat) != 0)
        {
            std::cout << "File error: " << strerror(errno) << "\n";
            return;
        }
        
        if (fileName[0] == '.' && !containsFlag("-all")) continue;

        FileInfo info = setFileInfo(fileStat);

        int permissionsMax;
        int numLinksMax;
        int ownerMax;
        int sizeMax;
        int lastModifiedMax;
        int nameMax;
        int padding = 3;

        filesCounter++;

        if (printHeader)
        {
            // Get the length of the longest string to set the width of each column (to line them up)
            permissionsMax = std::max(std::string("Permissions").length(), info.permissions.length());
            numLinksMax = std::max(std::string("Links").length(), info.numLinks.length());
            ownerMax = std::max(std::string("Owner").length(), info.owner.length());
            sizeMax = std::max(std::string("Size").length(), info.size.length());
            lastModifiedMax = std::max(std::string("Last Modified").length(), info.lastModified.length());
            nameMax = std::max(std::string("File Name").length(), fileName.length());

            // Print headers
            Printer::print(" ", 2 + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("Permissions", permissionsMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("Links", numLinksMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("Owner", ownerMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("Size",  sizeMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("Last Modified", lastModifiedMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            Printer::print("File Name", nameMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
            std::cout << std::endl;

            printHeader = false;
        }

            Printer::print(std::to_string(filesCounter), 2 + padding, TextColor::GRAY, TextEmphasis::BOLD);
            // Print info of each header
            Printer::print(info.permissions, permissionsMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            Printer::print(info.numLinks, numLinksMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            Printer::print(info.owner, ownerMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            Printer::print(info.size, sizeMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            Printer::print(info.lastModified, lastModifiedMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            Printer::print(fileName, nameMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
            std::cout << std::endl;
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

FileInfo ListCommand::setFileInfo(const struct stat& fileStat)
{
    FileInfo info;
    // Get permissions for the file
    info.permissions = getPermissions(fileStat);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileStat.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileStat.st_uid)->pw_name ? getpwuid(fileStat.st_uid)->pw_name : "-";
    // Get file size in bytes or number of bytes allocated to directory
    info.size = std::to_string(static_cast<int>(fileStat.st_size));
    // Get time and date of last modification
    info.lastModified = getLastModified(fileStat);
    
    return info;
}

bool ListCommand::hasValidArgsAndFlags()
{
    if (args.size() > 0)
    {
        errorMessage = "Too many arguments passed to 'ls' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (flags.size() > 1)
    {
        errorMessage = "Too many flags passed to 'ls' command. Use 'ogy help' to view the expected flags.\n";
        return false;
    }

    return true;
}