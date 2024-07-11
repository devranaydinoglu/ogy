#pragma once

#include <cstring>
#include <string>
#include <filesystem>

#include <sys/ioctl.h>
#include <unistd.h>

#include "../Command.h"
#include "../info/InfoCommand.h"
#include "../../printer/Printer.h"

using DirIterator = std::filesystem::directory_iterator;
using RecDirIterator = std::filesystem::recursive_directory_iterator;
using Path = std::filesystem::path;

class FindCommand : public Command
{
public:
    FindCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    CommonFileInfo setFileInfo(const struct stat& fileInfo, const Path& entryPath);

    template <typename T>
    void findFiles()
    {
        Path currentPath = std::filesystem::current_path();
        int index = 1;
        bool found = false;

        for (const auto& entry : T(currentPath, std::filesystem::directory_options::skip_permission_denied))
        {
            // Convert the provided find term and file name to lowercase for comparison
            std::string findTerm = args[0];
            std::transform(findTerm.begin(), findTerm.end(), findTerm.begin(), ::tolower);
            
            std::string fileName = entry.path().filename().string();
            std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);

            if (fileName.find(findTerm) != std::string::npos)
            {
                struct stat fileStat;
            
                // Check if valid file info has been returned
                if (stat(entry.path().c_str(), &fileStat) != 0)
                {
                    std::cout << "Error: " << std::strerror(errno) << "\n";
                    return;
                }
                
                // Set file info
                CommonFileInfo info = setFileInfo(fileStat, entry.path());
                
                // Get the length of the longest string to set the width of each column (to line them up)
                CommonFileInfoPadding padding = Command::getCommonFileInfoPadding(info);

                // Ensure that header length for the path can't be longer than the terminal width
                struct winsize winSize;
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
                int minPathInfoPadding = std::min(static_cast<int>(entry.path().string().length()), static_cast<int>(winSize.ws_col));
                int pathPadding = std::max(static_cast<int>(std::string("File Path").length()), minPathInfoPadding);
                
                // Print common headers
                Printer::print(" ", defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
                Command::printCommonHeaders(padding);

                // Print common file info
                Printer::print(std::to_string(index), defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD);
                Command::printCommonFileInfo(info, padding);
                
                // Print file path header
                Printer::print(" ", defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD);
                Printer::print("File Path", pathPadding, TextColor::GRAY, TextEmphasis::BOLD_UNDERLINED);
                std::cout << "\n";

                // Print file path info
                Printer::print(" ", defaultPadding + 2, TextColor::GRAY, TextEmphasis::BOLD);
                Printer::print(entry.path().string(), pathPadding, TextColor::CYAN, TextEmphasis::BOLD);
                std::cout << "\n";

                index++;
                found = true;
            }
        }

        if (!found) Printer::print("No file(s) found\n", 0, TextColor::WHITE, TextEmphasis::BOLD);
    }
};