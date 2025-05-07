#pragma once

#include <map>
#include <any>
#include "../Interpreter/Value.h"
#include "../Lexer/Token.h"

namespace lex
{
    class Environment
    {
    private:
        std::map<std::string, Value> values;

    public:
        void define(const std::string &name, const Value &value)
        {
            values[name] = value;
        }

        Value get(Token name)
        {
            auto it = values.find(name.lexeme);
            if (it != values.end())
            {
                return it->second;
            }
            throw std::runtime_error("Undefined variable: " + name.lexeme);
        }

        void assign(Token name, const Value &value)
        {
          if(values.find(name.lexeme) != values.end())
          {
            values[name.lexeme] = value;
          }
          else
          {
            throw std::runtime_error("Undefined variable: " + name.lexeme);
          }
        }
    };
}