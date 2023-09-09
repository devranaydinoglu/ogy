cmake_minimum_required(VERSION 3.20)

# Project details
project(ogy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /usr/local/ogy/bin)

set(SOURCE_FILES 
    src/ogy.cpp
    src/commands/Command.cpp
    src/commands/info/InfoCommand.cpp
    src/commands/list/ListCommand.cpp
    src/commands/help/HelpCommand.cpp
    src/commands/find/FindCommand.cpp
)

add_executable(ogy ${SOURCE_FILES})
