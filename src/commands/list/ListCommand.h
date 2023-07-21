#pragma once

#include "../Command.h"
#include "../info/InfoCommand.h"

class ListCommand : public Command
{
public:
    ListCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    std::string getPermissions(const struct stat& fileInfo);
    std::string getLastModified(const struct stat& fileInfo);
    FileInfo setFileInfo(const struct stat& fileInfo);
};