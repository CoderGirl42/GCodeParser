#pragma once

#include <string_view>
#include <unordered_map>
#include <optional>
#include <string>

enum class TokenKind
{
    Program,
    LineNumber,
    Command,
    Param,
    Number,
    Comment,
    Star,
    Checksum,
    EOL
};

struct Token
{
    TokenKind           kind;
    char                letter;
    float               number;
    std::string_view    slice;
};

struct GCodeCommand
{
    std::optional<int>					lineNo;
    char								letter;
    int									code;
    std::unordered_map<char, float>	    params;
    std::optional<std::string>			comment;
    std::optional<bool>                 is_program = false;
};