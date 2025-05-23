#pragma once

#include "../Lexer/Token.h"
#include <memory>
#include <any>
#include <utility>

namespace lex
{
    class Expr;
    using ExprPtr = std::shared_ptr<Expr>;

    class ExprVisitor
    {
    public:
        virtual ~ExprVisitor() = default;

        virtual std::any visitBinaryExpr(class Binary *expr) = 0;
        virtual std::any visitGroupingExpr(class Grouping *expr) = 0;
        virtual std::any visitLiteralExpr(class Literal *expr) = 0;
        virtual std::any visitUnaryExpr(class Unary *expr) = 0;
        virtual std::any visitTernaryExpr(class Ternary *expr) = 0;
        virtual std::any visitVariableExpr(class Variable *expr) = 0;
        virtual std::any visitAssignExpr(class Assign *expr) = 0;
        virtual std::any visitLogicalExpr(class Logical *expr) = 0;
    };

    // Base Expression class
    class Expr
    {
    public:
        virtual ~Expr() = default;
        virtual std::any accept(ExprVisitor *visitor) = 0;
    };

    // subclasses
    class Binary : public Expr
    {
    public:
        const ExprPtr left;
        const Token operator_token;
        const ExprPtr right;

        Binary(ExprPtr left, Token operator_token, ExprPtr right)
            : left(std::move(left)), operator_token(std::move(operator_token)), right(std::move(right))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitBinaryExpr(this);
        }
    };

    class Grouping : public Expr
    {
    public:
        const ExprPtr expression;

        Grouping(ExprPtr expression)
            : expression(std::move(expression))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitGroupingExpr(this);
        }
    };

    class Literal : public Expr
    {
    public:
        const LiteralValue value;

        Literal(LiteralValue value) : value(std::move(value))
        {
        }
        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitLiteralExpr(this);
        }
    };

    class Unary : public Expr
    {
    public:
        const Token operator_token;
        const ExprPtr right;

        Unary(Token operator_token, ExprPtr right)
            : operator_token(std::move(operator_token)), right(std::move(right))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitUnaryExpr(this);
        }
    };

    class Ternary : public Expr
    {
    public:
        const ExprPtr condition;
        const ExprPtr then_branch;
        const ExprPtr else_branch;

        Ternary(ExprPtr condition, ExprPtr then_branch, ExprPtr else_branch)
            : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitTernaryExpr(this);
        }
    };

    class Variable : public Expr
    {
    public:
        const Token name;

        Variable(Token name)
            : name(std::move(name))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitVariableExpr(this);
        }
    };

    class Assign : public Expr
    {
    public:
        const Token name;
        const ExprPtr value;

        Assign(Token name, ExprPtr value)
            : name(std::move(name)), value(std::move(value))
        {
        }

        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitAssignExpr(this);
        }
    };

    class Logical : public Expr
    {
    public:
        const ExprPtr left;
        const Token operator_token;
        const ExprPtr right;
        Logical(ExprPtr left, Token operator_token, ExprPtr right)
            : left(std::move(left)), operator_token(std::move(operator_token)), right(std::move(right))
        {
        }
        std::any accept(ExprVisitor *visitor) override
        {
            return visitor->visitLogicalExpr(this);
        }
    };

    // helper functions to create shared pointers for each expression type

    inline ExprPtr make_Binary(ExprPtr left, Token operator_token, ExprPtr right)
    {
        return std::make_shared<Binary>(std::move(left), std::move(operator_token), std::move(right));
    }

    inline ExprPtr make_Grouping(ExprPtr expression)
    {
        return std::make_shared<Grouping>(std::move(expression));
    }

    inline ExprPtr make_Literal(LiteralValue value)
    {
        return std::make_shared<Literal>(std::move(value));
    }

    inline ExprPtr make_Unary(Token operator_token, ExprPtr right)
    {
        return std::make_shared<Unary>(std::move(operator_token), std::move(right));
    }

    inline ExprPtr make_Ternary(ExprPtr condition, ExprPtr then_branch, ExprPtr else_branch)
    {
        return std::make_shared<Ternary>(std::move(condition), std::move(then_branch), std::move(else_branch));
    }

    inline ExprPtr make_Variable(Token name)
    {
        return std::make_shared<Variable>(std::move(name));
    }

    inline ExprPtr make_Assign(Token name, ExprPtr value)
    {
        return std::make_shared<Assign>(std::move(name), std::move(value));
    }
    inline ExprPtr make_Logical(ExprPtr left, Token operator_token, ExprPtr right)
    {
        return std::make_shared<Logical>(std::move(left), std::move(operator_token), std::move(right));
    }
}