#pragma once

#include "Token.h"
#include "TokenType.h"
#include<string>
#include<vector>

namespace lex
{
    class Lexer
    {
    private:
        std::string source;
        std::vector<Token> tokens;

        int start = 0;
        int current = 0;
        int line = 1;

        // helper methods
        bool is_at_end() const;
        void scan_token();
        char advance();
        void addToken(TokenType type);
        void addToken(TokenType type, LiteralValue literal);
        bool match(char expected);
        char peek() const;
        char peek_next() const;
        void string();
        bool is_digit(char c) const;
        bool is_alpha(char c) const;
        bool is_alphanumeric(char c) const;
        void number();
        void identifier();


    public:
        explicit Lexer(const std::string & source);

        // scan all tokens from source
        std::vector<Token> scan_tokens();

    };
} // namespace lex