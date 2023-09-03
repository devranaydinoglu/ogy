#pragma once

#include <filesystem>

#include "../Command.h"

struct FileInfo;

using Path = std::filesystem::path;

class ListCommand : public Command
{
public:
    ListCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    CommonFileInfo setFileInfo(const struct stat& fileInfo, const Path& entryPath);
};
