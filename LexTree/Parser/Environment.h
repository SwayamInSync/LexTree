#pragma once

#include <map>
#include <any>
#include "../Interpreter/Value.h"
#include "../Lexer/Token.h"
#include <stdexcept>

namespace lex
{
    class Environment
    {
    private:
        std::shared_ptr<Environment> enclosing;
        std::map<std::string, Value> values;

    public:
        Environment() : enclosing(nullptr) {} // default for global scope
        Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {} // for local scopes
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
            // check in enclosing environment
            if(enclosing)
            {
                return enclosing->get(name);
            }
            throw std::runtime_error("Undefined variable: " + name.lexeme);
        }

        void assign(Token name, const Value &value)
        {
          if(values.find(name.lexeme) != values.end())
          {
            values[name.lexeme] = value;
            return;
          }
          if (enclosing != nullptr)
          {
              enclosing->assign(name, value);
              return;
          }
          else
          {
            throw std::runtime_error("Undefined variable: " + name.lexeme);
          }
        }
    };
}