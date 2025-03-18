#include "Token.h"
#include <sstream>

namespace lex
{
    std::string Token::to_string() const
    {
        std::ostringstream oss;
        oss << tokentype_to_string(type) << " " << lexeme << " ";

        if(std::holds_alternative<std::string>(literal))
            oss << std::get<std::string>(literal);
        else if(std::holds_alternative<double>(literal))
            oss << std::get<double>(literal);
        else if(std::holds_alternative<bool>(literal))
            oss << (std::get<bool>(literal) ? "true" : "false");
        else
            oss << "nil";

    }
}