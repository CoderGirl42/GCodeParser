#pragma once

#include "Compiler.h"

class Lexer
{
public:
    explicit Lexer(std::string_view src) : m_src(src) {}

    std::vector<Token> tokenize();
private:
    static bool is_space(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
    static bool is_alpha(char c) { return (c | 0x20) >= 'a' && (c | 0x20) <= 'z'; }
    static bool is_digit(char c) { return c >= '0' && c <= '9' || c=='.'; }

    static std::pair<float, std::size_t> parse_number(std::string_view sv);

    std::string_view m_src;
    std::size_t m_pos = 0;
};