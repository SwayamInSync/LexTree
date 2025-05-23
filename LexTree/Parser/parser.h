#pragma once

#include "../Lexer/TokenType.h"
#include "../Lexer/Token.h"
#include "Expr.h"
#include "Stmt.h"
#include <vector>
#include <string>
#include <stdexcept>

namespace lex
{
    class ParseError : public std::runtime_error
    {
    public:
        explicit ParseError(const std::string &message) : std::runtime_error(message) {}
    };

    class Parser
    {
    private:
        std::vector<Token> tokens;
        int current = 0;

        // Production rules
        // statements
        StmtPtr declaration();
        StmtPtr variable_declaration();
        StmtPtr statement();
        StmtPtr print_statement();
        StmtPtr expression_statement();
        std::vector<StmtPtr> block();
        StmtPtr if_statement();
        StmtPtr while_statement();
        StmtPtr for_statement();

        // expressions
        ExprPtr expression();
        ExprPtr assignment();
        ExprPtr logical_or();
        ExprPtr logical_and();
        ExprPtr comma();
        ExprPtr conditional();
        ExprPtr equality();
        ExprPtr comparison();
        ExprPtr term();
        ExprPtr factor();
        ExprPtr unary();
        ExprPtr primary();

        // utility functions
        Token peek() const;
        Token previous() const;
        bool is_at_end() const;
        Token advance();
        bool match(TokenType type);
        bool match(std::initializer_list<TokenType> types);
        bool check(TokenType type);

        // error handling
        ParseError error(const Token &token, const std::string &message);
        Token consume(TokenType type, const std::string &message);
        void synchronize();

    public:
        explicit Parser(const std::vector<Token> &tokens);
        std::vector<StmtPtr> parse();
    };
}