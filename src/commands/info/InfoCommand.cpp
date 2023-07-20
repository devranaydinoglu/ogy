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
}

void InfoCommand::execute()
{
    struct stat fileInfo;
    
    // Check if valid file info has been returned
    if (stat(args[0].c_str(), &fileInfo) != 0) 
    {
        std::cout << "File error: " << strerror(errno) << "\n";
        return;
    }

    // Set file info string variables
    FileInfo info;
    // Get permissions for the file
    info.permissions = getPermissions(fileInfo);
    // Get number of hard links to the file
    info.numLinks = std::to_string(static_cast<int>(fileInfo.st_nlink));
    // Get number of hard links to the file
    info.owner = getpwuid(fileInfo.st_uid)->pw_name ? getpwuid(fileInfo.st_uid)->pw_name : "-";
    // Get file size in bytes or number of bytes allocated to directory
    info.size = std::to_string(static_cast<int>(fileInfo.st_size));
    // Get time and date of last modification
    info.lastModified = getLastModified(fileInfo);
    // Get file name from args
    info.name = args[0];
    
    // Get the length of the longest string to set the width of each column (to line them up)
    int permissionsMax = std::max(std::string("Permissions").length(), info.permissions.length());
    int numLinksMax = std::max(std::string("Links").length(), info.numLinks.length());
    int ownerMax = std::max(std::string("Owner").length(), info.owner.length());
    int sizeMax = std::max(std::string("Size").length(), info.size.length());
    int lastModifiedMax = std::max(std::string("Last Modified").length(), info.lastModified.length());
    int nameMax = std::max(std::string("File Name").length(), info.name.length());

    int padding = 3;

    // Print headers
    Printer::print("Permissions", permissionsMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Links", numLinksMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Owner", ownerMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Size",  sizeMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("Last Modified", lastModifiedMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    Printer::print("File Name", nameMax + padding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
    std::cout << std::endl;

    // Print info of each header
    Printer::print(info.permissions, permissionsMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.numLinks, numLinksMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.owner, ownerMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.size, sizeMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.lastModified, lastModifiedMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
    Printer::print(info.name, nameMax + padding, TextColor::CYAN, TextEmphasis::BOLD);
}

std::string InfoCommand::getLastModified(const struct stat &fileInfo)
{
    char* buffer = new char[31];
    std::strftime(buffer, sizeof buffer, "%a %d %b %Y at %H:%M:%S", std::localtime(&fileInfo.st_mtimespec.tv_sec));

    std::string lastModified = buffer;
    delete[] buffer;

    return lastModified;
}

std::string InfoCommand::getPermissions(const struct stat &fileInfo)
{
    std::string permsAsString;
    mode_t perm = fileInfo.st_mode;
    
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

bool InfoCommand::hasValidArgsAndFlags()
{
    if (args.size() < 1)
    {
        errorMessage = "No arguments passed to 'info' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (args.size() > 1)
    {
        errorMessage = "Too many arguments passed to 'info' command. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }
    if (flags.size() > 0)
    {
        errorMessage = "'info' command doesn't expect any flags. Use 'ogy help' to view the expected arguments.\n";
        return false;
    }

    return true;
}