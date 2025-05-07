#pragma once

#include <map>
#include <any>
#include "../Interpreter/Value.h"
#include "../Lexer/Token.h"
#include "../Error_Handling/RunTimeError.h"
#include <stdexcept>

namespace lex
{
    class Environment
    {
    private:
        std::shared_ptr<Environment> parent;
        std::map<std::string, Value> values;

    public:
        Environment() : parent(nullptr) {} // default for global scope
        Environment(std::shared_ptr<Environment> parent) : parent(std::move(parent)) {} // for local scopes
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
            // check in parent environment
            if(parent)
            {
                return parent->get(name); // recursively get from parent environment
            }
            
            throw RuntimeError(name, "Undefined variable: " + name.lexeme);
        }

        void assign(Token name, const Value &value)
        {
          if(values.find(name.lexeme) != values.end())
          {
            values[name.lexeme] = value;
            return;
          }
          if (parent != nullptr)
          {
              parent->assign(name, value); // recursively assign in parent environment
              return;
          }
          else
          {
            throw RuntimeError(name, "Undefined variable: " + name.lexeme);
          }
        }
    };
}