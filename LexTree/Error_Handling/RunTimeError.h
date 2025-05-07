#pragma once

#include <stdexcept>
#include <string>
#include "../Lexer/Token.h"
#include "../Interpreter/Value.h"


namespace lex
{
  class RuntimeError : public std::runtime_error
    {
    public:
        const Token token;

        RuntimeError(const Token &token, const std::string &message)
            : std::runtime_error(message), token(token) {}
    };
}