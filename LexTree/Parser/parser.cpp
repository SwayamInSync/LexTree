#include "parser.h"
#include "../LexTree.h"

namespace lex
{
    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    ExprPtr Parser::parse()
    {
        try
        {
            return expression();
        }
        catch (const ParseError&)
        {
            return nullptr;
        }
    }
    Token Parser::peek() const
    {
        return tokens[current];
    }
    Token Parser::previous() const
    {
        return tokens[current-1];
    }

    bool Parser::is_at_end() const
    {
        return current >= tokens.size() ||
               tokens[current].type == TokenType::EOF_TOKEN;
    }

    Token Parser::advance()
    {
        if(!is_at_end())
            ++current;
        return previous();
    }
    bool Parser::match(TokenType type)
    {
        if(check(type))
        {
            advance();
            return true;
        }
        return false;
    }
    bool Parser::match(std::initializer_list<TokenType> types)
    {
        for(TokenType type: types)
        {
            if(check(type))
            {
                advance();
                return true;
            }
        }

        return false;
    }

    bool Parser::check(TokenType type)
    {
        if(is_at_end())
            return false;
        return peek().type == type;
    }

    ParseError Parser::error(const lex::Token &token, const std::string &message)
    {
        LexTree::error(token.line, message);
        return ParseError(message);
    }

    Token Parser::consume(lex::TokenType type, const std::string &message)
    {
        if(check(type))
            return advance();

        throw error(peek(), message);
    }

    void Parser::synchronize()
    {
        advance();
        while(!is_at_end())
        {
            if(previous().type == TokenType::SEMICOLON)
                return;

            switch (peek().type)
            {
                case TokenType::CLASS:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
                default:
                    break;
            }
            advance();
        }
    }

    ExprPtr Parser::expression()
    {
        // expression -> equality
        return equality();
    }

    ExprPtr Parser::equality()
    {
        // equality → comparison ( ( "!=" | "==" ) comparison )*
        ExprPtr expr = comparison();

        while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
        {
            Token op = previous();
            ExprPtr right = comparison();
            expr = make_Binary(expr, op, right);
        }

        return expr;

    }

    ExprPtr Parser::comparison()
    {
        // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )*
        ExprPtr expr = term();

        while(match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                     TokenType::LESS, TokenType::LESS_EQUAL}))
        {
            Token op = previous();
            ExprPtr right = term();
            expr = make_Binary(expr, op, right);
        }

        return expr;
    }

    ExprPtr Parser::term()
    {
        // term → factor ( ( "-" | "+" ) factor )*
        ExprPtr expr = factor();

        while(match({TokenType::MINUS, TokenType::PLUS}))
        {
            Token op = previous();
            ExprPtr right = factor();
            expr = make_Binary(expr, op, right);
        }

        return expr;
    }

    ExprPtr Parser::factor()
    {
        // factor -> unary ( ( "/" | "*" ) unary )* ;
        ExprPtr expr = unary();

        while(match({TokenType::SLASH, TokenType::STAR}))
        {
            Token op = previous();
            ExprPtr right = unary();
            expr = make_Binary(expr, op, right);
        }

        return expr;
    }

    ExprPtr Parser::unary()
    {
        // unary → ( "!" | "-" ) unary | primary
        if(match({TokenType::BANG, TokenType::MINUS}))
        {
            Token op = previous();
            ExprPtr right = unary();
            return make_Unary(op, right);
        }

        return primary();
    }

    ExprPtr Parser::primary()
    {
        // primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"

        // Literals
        if (match(TokenType::FALSE))
            return make_Literal(false);
        if (match(TokenType::TRUE))
            return make_Literal(true);
        if (match(TokenType::NIL))
            return make_Literal(std::monostate{});

        if (match(TokenType::NUMBER))
        {
            return make_Literal(std::get<double>(previous().literal));
        }

        if (match(TokenType::STRING))
        {
            return make_Literal(std::get<std::string>(previous().literal));
        }

        // Grouping - expressions in parentheses
        if (match(TokenType::LEFT_PAREN))
        {
            ExprPtr expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return make_Grouping(expr);
        }

        // Error case
        throw error(peek(), "Expect expression.");
    }
}