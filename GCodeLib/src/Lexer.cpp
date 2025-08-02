#include <string>
#include <charconv>
#include <vector>
#include <exception>
#include <stdexcept>

#include "..\inc\Lexer.h"

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> ts;
    uint32_t           checksum = 0;
    bool               beforeStar = true;
    std::string        checksumstring;

    while (m_pos < m_src.size())
    {
        char c = m_src[m_pos];

        if (beforeStar && c != '*')
        {
            bool trailingSpace =
                (c == ' ') && 
                (m_pos + 1 < m_src.size()) &&
                (m_src[m_pos + 1] == '*');

            if (!trailingSpace) {      
                checksumstring += c;
                checksum ^= static_cast<unsigned char>(c);
            }
        }

        if (is_space(c))
        {
            ++m_pos;
            continue;
        }

        if (c == ';')
        {
            ts.push_back({ TokenKind::Comment, 0, 0.0, m_src.substr(m_pos) });
            break;
        }

        if (c == '%')
        {
            ts.push_back({ TokenKind::Program, 0, 0.0, m_src.substr(m_pos, 1) });
            ++m_pos;
            break;
        }

        if (c == '(')
        {
            std::size_t start = m_pos++;

            while (m_pos < m_src.size() && m_src[m_pos] != ')')
                ++m_pos;

            if (m_pos < m_src.size() && m_src[m_pos] == ')')
                ++m_pos;

            ts.push_back({ TokenKind::Comment, 0, 0.0, m_src.substr(start, m_pos - start) });
            continue;
        }

        if (c == '*')
        {
            beforeStar = false;

            ts.push_back({ TokenKind::Star, '*', 0.0,
                            m_src.substr(m_pos, 1) });
            ++m_pos;    

            std::size_t start = m_pos;
            while (m_pos < m_src.size() && is_digit(m_src[m_pos]))
                ++m_pos;

            if (start == m_pos)
                throw std::runtime_error("Lexer: missing checksum digits");

            std::string_view digits = m_src.substr(start, m_pos - start);
            auto [value, len] = parse_number(digits);
            unsigned char given = static_cast<unsigned char>(value);

            ts.push_back({ TokenKind::Checksum, 0, value, digits });

            //if (given != checksum)
            //    throw std::runtime_error(
            //        "Lexer: checksum mismatch – given " +
            //        std::to_string(given) +
            //        ", calculated " + std::to_string(checksum) + "(" + checksumstring + ")");
            continue;
        }

        if (is_alpha(c))
        {
            switch (c | 0x20) {
            case 'n':
                ts.push_back({ TokenKind::LineNumber, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            case 'g':
                ts.push_back({ TokenKind::Command, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            case 'm':
                ts.push_back({ TokenKind::Command, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            case 't':
                ts.push_back({ TokenKind::Command, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            case 'o':
                ts.push_back({ TokenKind::Command, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            default:
                // need to handle all letters and choke on bad letters
                ts.push_back({ TokenKind::Param, static_cast<char>(std::toupper(c)), 0.0, m_src.substr(m_pos, 1) });
                break;
            }

            m_pos++;

            continue;
        }

        if (is_digit(c) || c == '+' || c == '-')
        {
            auto [num, len] = parse_number(m_src.substr(m_pos));

            if (beforeStar)
            {
                for (std::size_t i = 1; i < len; ++i)
                {
                    char d = m_src[m_pos + i];
                    checksumstring += d;
                    checksum ^= static_cast<unsigned char>(d);
                }
            }

            ts.push_back({ TokenKind::Number, 0, num, m_src.substr(m_pos, len) });
            m_pos += len;

            continue;
        }

        throw std::runtime_error(std::string("Lexer: unexpected char'") + c + "'");
    }

    if (beforeStar) {
        std::string buf = std::to_string(static_cast<int>(checksum));
        ts.push_back({ TokenKind::Star, '*', 0.0, buf });
        ts.push_back({ TokenKind::Checksum, 0, (float)checksum, buf });
    }

    ts.push_back({ TokenKind::EOL, 0, 0.0, {} });
    return ts;
}


std::pair<float, std::size_t> Lexer::parse_number(std::string_view sv)
{
    float val{};
    const char* first = sv.data();
    const char* last = first + sv.size();
    auto [ptr, ec] = std::from_chars(first, last, val);
    if (ec == std::errc{})
        return { val, static_cast<std::size_t>(ptr - first) };

    char* end{};
    val = std::strtod(first, &end);
    if (end == first)
        throw std::runtime_error("Lexer: bad number");

    return { val, static_cast<std::size_t>(end - first) };
}
