#pragma once

#include <map>
#include <any>
#include "../Lexer/Token.h"

namespace lex
{
    class Environment
    {
    private:
        std::map<std::string, std::any> values;

    public:
        void define(const std::string &name, const std::any &value)
        {
            values[name] = value;
        }

        std::any get(Token name)
        {
            if (values.find(name.lexeme) != values.end())
            {
                return values[name.lexeme];
            }
            throw std::runtime_error("Undefined variable: " + name.lexeme);
        }
    };
}