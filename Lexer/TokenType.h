#pragma once

#include <unordered_map>
#include <string>

namespace lex
{
    enum class TokenType
    {
        // single-character tokens

        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        // one or two character tokens
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // identifiers
        IDENTIFIER, STRING, NUMBER,

        // keywords
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR, PRINT, RETURN,
        SUPER, THIS, TRUE, VAR, WHILE,

        // End Of File Token
        EOF_TOKEN,

    };

    inline std::unordered_map<std::string, TokenType> create_keyword_map()
    {
        std::unordered_map<std::string, TokenType> keywords;
        keywords[std::string("and")] = TokenType::AND;
        keywords[std::string("class")] = TokenType::CLASS;
        keywords[std::string("else")] = TokenType::ELSE;
        keywords[std::string("false")] = TokenType::FALSE;
        keywords[std::string("for")] = TokenType::FOR;
        keywords[std::string("fun")] = TokenType::FUN;
        keywords[std::string("if")] = TokenType::IF;
        keywords[std::string("nil")] = TokenType::NIL;
        keywords[std::string("or")] = TokenType::OR;
        keywords[std::string("print")] = TokenType::PRINT;
        keywords[std::string("return")] = TokenType::RETURN;
        keywords[std::string("super")] = TokenType::SUPER;
        keywords[std::string("this")] = TokenType::THIS;
        keywords[std::string("true")] = TokenType::TRUE;
        keywords[std::string("var")] = TokenType::VAR;
        keywords[std::string("while")] = TokenType::WHILE;
        return keywords;
    }


    inline const std::unordered_map<std::string, TokenType>& getKeywords()
    {
        static const std::unordered_map<std::string, TokenType> keywords = create_keyword_map();
        return keywords;
    }

    const char* tokentype_to_string(TokenType type);

}