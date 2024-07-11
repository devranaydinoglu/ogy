#pragma once

#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <cstring>

#include "../Command.h"
#include <benchmark/benchmark.h>

struct FileInfo;

using Path = std::filesystem::path;

class ListCommand : public Command
{
public:
    ListCommand(int argc, char** argv);
    void execute() override;
    void execute_st();
    void execute_mt();
    bool hasValidArgsAndFlags() override;

    static void bm_setFileInfo(benchmark::State& state)
    {
        std::string strCurrentPath = "/Users/devranaydinoglu/Documents/School";
        Path currentPath = strCurrentPath;
        struct stat fileStat;
        
        if (stat(currentPath.c_str(), &fileStat) != 0)
        {
            std::cout << "Error: " << std::strerror(errno) << "\n";
            return;
        }

        char** argvv;
        argvv[0] = const_cast<char*>("ogy");
        argvv[1] = const_cast<char*>("ls");
        argvv[2] = const_cast<char*>("-rec");
        ListCommand lc(3, argvv);
        
        for (auto _ : state)
        {
            lc.execute_mt();
        }
    }

private:
    CommonFileInfo setFileInfo(const struct stat& fileInfo, const Path& entryPath);
};

// Register as benchmark function for Google Benchmark
// BENCHMARK(ListCommand::bm_setFileInfo);
