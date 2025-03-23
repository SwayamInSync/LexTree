// This is auto-generated file, using the utility/generate_ast.cpp
#pragma once

#include "../Lexer/Token.h"
#include <memory>
#include <utility>
#include <any>

namespace lex {

// Forward declaration for recursive expressions
class Expr;
using ExprPtr = std::shared_ptr<Expr>;

// Visitor interface (for double dispatch)
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    // Declare a visit method for each expression type
    virtual std::any visitBinaryExpr(class Binary* expr) = 0;
    virtual std::any visitGroupingExpr(class Grouping* expr) = 0;
    virtual std::any visitLiteralExpr(class Literal* expr) = 0;
    virtual std::any visitUnaryExpr(class Unary* expr) = 0;
    virtual std::any visitTernaryExpr(class Ternary* expr) = 0;
    virtual std::any visitVariableExpr(class Variable* expr) = 0;
};

// Base Expression class
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor* visitor) = 0;
};

// Binary expression
class Binary : public Expr {
public:
    Binary(ExprPtr left, Token operator_token, ExprPtr right)
        : left(std::move(left)), operator_token(std::move(operator_token)), right(std::move(right)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitBinaryExpr(this);
    }

    const ExprPtr left;
    const Token operator_token;
    const ExprPtr right;
};

// Grouping expression
class Grouping : public Expr {
public:
    Grouping(ExprPtr expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitGroupingExpr(this);
    }

    const ExprPtr expression;
};

// Literal expression
class Literal : public Expr {
public:
    Literal(LiteralValue value)
        : value(std::move(value)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitLiteralExpr(this);
    }

    const LiteralValue value;
};

// Unary expression
class Unary : public Expr {
public:
    Unary(Token operator_token, ExprPtr right)
        : operator_token(std::move(operator_token)), right(std::move(right)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitUnaryExpr(this);
    }

    const Token operator_token;
    const ExprPtr right;
};

// Ternary expression
class Ternary : public Expr {
public:
    Ternary(ExprPtr condition, ExprPtr then_branch, ExprPtr else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitTernaryExpr(this);
    }

    const ExprPtr condition;
    const ExprPtr then_branch;
    const ExprPtr else_branch;
};

// Variable expression
class Variable : public Expr {
public:
    Variable(Token name)
        : name(std::move(name)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitVariableExpr(this);
    }

    const Token name;
};

// Helper functions to create shared pointers for each expression type
inline ExprPtr make_Binary(ExprPtr left, Token operator_token, ExprPtr right) {
    return std::make_shared<Binary>(std::move(left), std::move(operator_token), std::move(right));
}

inline ExprPtr make_Grouping(ExprPtr expression) {
    return std::make_shared<Grouping>(std::move(expression));
}

inline ExprPtr make_Literal(LiteralValue value) {
    return std::make_shared<Literal>(std::move(value));
}

inline ExprPtr make_Unary(Token operator_token, ExprPtr right) {
    return std::make_shared<Unary>(std::move(operator_token), std::move(right));
}

inline ExprPtr make_Ternary(ExprPtr condition, ExprPtr then_branch, ExprPtr else_branch) {
    return std::make_shared<Ternary>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

inline ExprPtr make_Variable(Token name) {
    return std::make_shared<Variable>(std::move(name));
}

} // namespace lex
