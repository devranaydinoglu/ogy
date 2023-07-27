#pragma once

#include <map>
#include <string>

enum class TextColor
{
    GREEN,
    YELLOW,
    MAGENTA,
    CYAN,
    GRAY,
    WHITE
};

enum class TextEmphasis
{
    NORMAL,
    BOLD,
    ITALIC,
    UNDERLINED,
    BOLD_UNDERLINED
};

class Formatter
{
public:
    inline static std::map<TextColor, std::string> colorsToStringMapping = {
        {TextColor::GREEN, "32"},
        {TextColor::YELLOW, "33"},
        {TextColor::MAGENTA, "35"},
        {TextColor::CYAN, "36"},
        {TextColor::GRAY, "90"},
        {TextColor::WHITE, "37"}
    };

    inline static std::map<TextEmphasis, std::string> emphasesToStringMapping = {
        {TextEmphasis::NORMAL, "0"},
        {TextEmphasis::BOLD, "1"},
        {TextEmphasis::ITALIC, "3"},
        {TextEmphasis::UNDERLINED, "4"},
        {TextEmphasis::BOLD_UNDERLINED, "1;4"}

    };
};