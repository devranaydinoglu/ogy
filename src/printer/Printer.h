#pragma once

#include <ios>
#include <iostream>
#include <iomanip>
#include <string_view>

#include "../formatter/Formatter.h"

/**
* Responsible for printing the output of commands
*/
class Printer
{
public:
    static void print(std::string_view str, int width, TextColor textColor, TextEmphasis textEmphasis)
    {
        std::cout << std::left << "\033[" << std::setw(0) << Formatter::emphasesToStringMapping[textEmphasis] << ";" 
        << Formatter::colorsToStringMapping[textColor] << "m" << std::setw(width) << std::setfill(' ') << str << "\033[0m";
    }
};
