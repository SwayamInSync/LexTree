#pragma once

#include "TokenType.h"
#include <string>
#include <variant>

namespace lex
{
    using LiteralValue = std::variant<std::monostate, std::string, double, bool>;
    class Token
    {
    public:
        const TokenType type;
        const std::string lexeme;
        const LiteralValue literal;
        int line;

        // constructor
        Token(TokenType type, std::string lexeme, LiteralValue literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

        // helper function
        std::string to_string() const;
    };
}