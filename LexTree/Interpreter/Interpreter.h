#pragma once
#include "../Parser/Expr.h"
#include "Value.h"
#include <stdexcept>

namespace lex
{
    class RuntimeError: public std::runtime_error
    {
    public:
        const Token token;

        RuntimeError(const Token& token, const std::string& message)
                : std::runtime_error(message), token(token) {}
    };

    class Interpreter : public ExprVisitor
    {
    public:
        Value interpret(const ExprPtr& expression);

        // Visit methods from ExprVisitor
        std::any visitBinaryExpr(Binary* expr) override;
        std::any visitGroupingExpr(Grouping* expr) override;
        std::any visitLiteralExpr(Literal* expr) override;
        std::any visitUnaryExpr(Unary* expr) override;
        std::any visitTernaryExpr(Ternary* expr) override;
        std::any visitVariableExpr(Variable* expr) override;

    private:
        // Helper methods for evaluation
        Value evaluate(const ExprPtr& expr);
        void check_number_operand(const Token& operator_token, const Value& operand);
        void check_number_operands(const Token& operator_token, const Value& left, const Value& right);
    };
}