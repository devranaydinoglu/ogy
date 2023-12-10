#include <cstddef>
#include <iostream>

#include "commands/Command.h"
#include <benchmark/benchmark.h>

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cout << "No commands passed. Use 'ogy help' to view the available commands.\n";
    }
    else if (argc > 1)
    {
        Command com(argc, argv);
        com.determineCommand();
    }

}

//Google Benchmark's main function
// BENCHMARK_MAIN();
