#pragma once

#include "../Command.h"

class HelpCommand : public Command
{
public:
    HelpCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;
};
