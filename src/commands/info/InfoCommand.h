#pragma once

#include "../Command.h"

class InfoCommand : public Command
{
public:
    InfoCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    CommonFileInfo setFileInfo(const struct stat& fileInfo);
};
