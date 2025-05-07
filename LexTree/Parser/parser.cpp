#include "parser.h"
#include "../LexTree.h"

namespace lex
{
    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}
    std::vector<StmtPtr> Parser::parse()
    {
        std::vector<StmtPtr> statements;
        while (!is_at_end())
        {
            auto stmt = declaration();
            if (stmt != nullptr)
            {
                statements.push_back(stmt);
            }
        }

        return statements;
    }
    Token Parser::peek() const
    {
        return tokens[current];
    }
    Token Parser::previous() const
    {
        return tokens[current - 1];
    }

    bool Parser::is_at_end() const
    {
        return current >= tokens.size() ||
               tokens[current].type == TokenType::EOF_TOKEN;
    }

    Token Parser::advance()
    {
        if (!is_at_end())
            ++current;
        return previous();
    }
    bool Parser::match(TokenType type)
    {
        if (check(type))
        {
            advance(); // Consume the token and move to the next one
            return true;
        }
        return false;
    }
    bool Parser::match(std::initializer_list<TokenType> types)
    {
        for (TokenType type : types)
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }

        return false;
    }

    bool Parser::check(TokenType type)
    {
        if (is_at_end())
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
        if (check(type))
            return advance();

        throw error(peek(), message);
    }

    void Parser::synchronize()
    {
        advance();
        while (!is_at_end())
        {
            if (previous().type == TokenType::SEMICOLON)
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

    StmtPtr Parser::declaration()
    {
        // declaration -> print_statement | expression_statement | variable_declaration
        try
        {
            if (match(TokenType::VAR))
                return variable_declaration();
            return statement();
        }
        catch (const ParseError &)
        {
            synchronize();
            return nullptr;
        }
    }

    StmtPtr Parser::variable_declaration()
    {
        // variable_declaration -> "var" IDENTIFIER ( "=" expression )? ";"
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        ExprPtr initializer = nullptr;

        if (match(TokenType::EQUAL))
        {
            initializer = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return make_VariableStmt(name, initializer);
    }

    StmtPtr Parser::statement()
    {
        // statement -> print_statement | expression_statement
        if (match(TokenType::PRINT))
            return print_statement();

        return expression_statement();
    }

    StmtPtr Parser::print_statement()
    {
        // print_statement -> "print" expression ";"

        ExprPtr value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return make_PrintStmt(value);
    }

    StmtPtr Parser::expression_statement()
    {
        // expression_statement -> expression ";"
        ExprPtr expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return make_ExpressionStmt(expr);
    }

    ExprPtr Parser::expression()
    {
        // expression -> assignment
        return assignment();
    }

    ExprPtr Parser::assignment()
    {
      // assignment -> IDENTIFIER "=" expression
      // issue we don't have a way to check if the identifier is valid (L-value, R-value)
      // trick

      ExprPtr expr = comma();

      // Check if the next token is an assignment
      if (match(TokenType::EQUAL))
      {
        Token equals = previous();
        ExprPtr value = assignment(); // Recursive call to parse the right-hand side

        // Check if the left-hand side is a variable
        if(auto varExpr = std::dynamic_pointer_cast<Variable>(expr))
        {
          Token name = varExpr->name;
          return make_Assign(name, value);
        }

        // If it's not a variable, throw an error
        throw error(equals, "Invalid assignment target.");
      }

      return expr;
    }

    ExprPtr Parser::comma()
    {
        // comma → conditional ( "," conditional )*
        ExprPtr expr = conditional();

        while (match(TokenType::COMMA))
        {
            Token op = previous();
            ExprPtr right = conditional();
            expr = make_Binary(expr, op, right);
        }

        return expr;
    }

    ExprPtr Parser::conditional()
    {
        // conditional → equality ("?" expression ":" conditional)?

        // Check for conditional operator without left operand
        if (check(TokenType::QUESTION))
        {
            Token op = advance();
            error(op, "Conditional operator cannot be used without a condition.");
            // Try to parse the rest of the conditional to continue
            ExprPtr thenBranch = expression();
            consume(TokenType::COLON, "Expect ':' after then branch of conditional expression.");
            ExprPtr elseBranch = conditional();
            return elseBranch; // Just return something to continue parsing
        }

        ExprPtr expr = equality();
        if (match(TokenType::QUESTION))
        {
            ExprPtr thenBranch = expression();
            consume(TokenType::COLON, "Expect ':' after then branch of conditional expression.");
            ExprPtr elseBranch = conditional();
            expr = make_Ternary(expr, thenBranch, elseBranch);
        }

        return expr;
    }

    ExprPtr Parser::equality()
    {
        // equality → comparison ( ( "!=" | "==" ) comparison )*
        ExprPtr expr = comparison();

        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
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

        while (match({TokenType::GREATER, TokenType::GREATER_EQUAL,
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

        while (match({TokenType::MINUS, TokenType::PLUS}))
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

        while (match({TokenType::SLASH, TokenType::STAR}))
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
        if (match({TokenType::BANG, TokenType::MINUS}))
        {
            Token op = previous();
            ExprPtr right = unary();
            return make_Unary(op, right);
        }

        return primary();
    }

    ExprPtr Parser::primary()
    {
        // primary → NUMBER | STRING | "true" | "false" | "nil" | IDENTIFIER | "(" expression ")"

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

        if (match(TokenType::IDENTIFIER))
        {

            return make_Variable(previous());
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