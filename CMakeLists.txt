cmake_minimum_required(VERSION 3.20)

# Project details
project(ogy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES 
    src/Ogy.cpp
    src/commands/Command.cpp
    src/commands/info/InfoCommand.cpp
    src/commands/list/ListCommand.cpp
    src/commands/help/HelpCommand.cpp
    src/commands/find/FindCommand.cpp
    src/commands/change_directory/ChangeDirectoryCommand.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(benchmark REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
