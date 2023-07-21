#pragma once

#include "../Command.h"

/**
* Used to store data of a file
*/
struct FileInfo
{
    std::string permissions;
    std::string numLinks;
    std::string owner;
    std::string size;
    std::string lastModified;
    std::string name;
};

class InfoCommand : public Command
{
public:
    InfoCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    std::string getPermissions(const struct stat& fileInfo);
    std::string getLastModified(const struct stat& fileInfo);
    FileInfo setFileInfo(const struct stat& fileInfo);
};
