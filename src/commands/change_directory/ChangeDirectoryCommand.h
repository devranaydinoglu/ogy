#pragma once

#include <string>

#include "rapidjson/document.h"
#include "../Command.h"

namespace rj = rapidjson;

class ChangeDirectoryCommand : public Command
{
private:
    std::string alias;
    std::string path;

public:
    ChangeDirectoryCommand(int argc, char** argv);
    void execute() override;
    bool hasValidArgsAndFlags() override;

private:
    /**
     * Check whether each arg is an alias or path
    */
    void determineArgs(const rj::Document& doc);

    void readConfigFile(rj::Document& doc, const char* fileName);

    /**
     * Search for a value and return it based on the provided key
    */
    std::string recursiveValueSearch(const rj::Value& val, const char* key);

    bool containsKey(const rj::Value& val, const char* key);
    bool containsValue(const rj::Value& val, const char* value);
    bool isValidPath(std::string addedPath);
    std::string getCurrentPath();
};