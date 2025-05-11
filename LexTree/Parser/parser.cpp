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
        // statement -> print_statement | expression_statement | if_statement | while_statement | block
        if (match(TokenType::IF))
            return if_statement();
        if (match(TokenType::PRINT))
            return print_statement();
        if (match(TokenType::WHILE))
            return while_statement();
        if (match(TokenType::FOR))
            return for_statement();
        if (match(TokenType::LEFT_BRACE))
            return make_BlockStmt(this->block()); // wrapping in make_BlockStmt because block() returns a list of statements, which are not a node of AST

        return expression_statement();
    }

    std::vector<StmtPtr> Parser::block()
    {
        std::vector<StmtPtr> statements;

        while (!check(TokenType::RIGHT_BRACE) && !is_at_end())
        {
            auto stmt = declaration();
            if (stmt != nullptr)
            {
                statements.push_back(stmt);
            }
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    StmtPtr Parser::print_statement()
    {
        // print_statement -> "print" expression ";"

        ExprPtr value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return make_PrintStmt(value);
    }

    StmtPtr Parser::while_statement()
    {
        // while_statement -> "while" "(" expression ")" statement
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        ExprPtr condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        StmtPtr body = statement();
        return make_WhileStmt(condition, body);
    }

    StmtPtr Parser::for_statement()
    {
        // for_statement -> "for" "(" expression? ";" expression? ";" expression? ")" statement
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
        StmtPtr initializer = nullptr;
        if (match(TokenType::SEMICOLON))
        {
            // No initializer
        }
        else if (match(TokenType::VAR))
        {
            initializer = variable_declaration();
        }
        else
        {
            initializer = expression_statement();
        }

        ExprPtr condition = nullptr;
        if (!check(TokenType::SEMICOLON))
            condition = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        ExprPtr increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN))
            increment = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        StmtPtr body = statement();
        return make_ForStmt(initializer, condition, increment, body);
    }

    StmtPtr Parser::expression_statement()
    {
        // expression_statement -> expression ";"
        ExprPtr expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return make_ExpressionStmt(expr);
    }

    StmtPtr Parser::if_statement()
    {
        // if_statement -> "if" "(" expression ")" statement ( "else" statement )?
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        ExprPtr condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        StmtPtr then_branch = statement();
        StmtPtr else_branch = nullptr;
        if (match(TokenType::ELSE))
        {
            else_branch = statement();
        }
        return make_IfStmt(condition, then_branch, else_branch);
    }

    ExprPtr Parser::expression()
    {
        // expression -> assignment
        return assignment();
    }

    ExprPtr Parser::assignment()
    {
        // assignment -> IDENTIFIER "=" expression

        ExprPtr expr = logical_or();

        // Check if the next token is an assignment
        if (match(TokenType::EQUAL))
        {
            Token equals = previous();
            ExprPtr value = assignment(); // Recursive call to parse the right-hand side

            // Check if the left-hand side is a variable
            if (auto varExpr = std::dynamic_pointer_cast<Variable>(expr))
            {
                Token name = varExpr->name;
                return make_Assign(name, value);
            }

            // If it's not a variable, throw an error
            throw error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    ExprPtr Parser::logical_or()
    {
        ExprPtr expr = logical_and();
        while (match(TokenType::OR))
        {
            Token op = previous();
            ExprPtr right = logical_and();
            expr = make_Logical(expr, op, right);
        }

        return expr;
    }

    ExprPtr Parser::logical_and()
    {
        ExprPtr expr = comma();
        while (match(TokenType::AND))
        {
            Token op = previous();
            ExprPtr right = comma();
            expr = make_Logical(expr, op, right);
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