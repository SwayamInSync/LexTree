#pragma once
#include "../Parser/Expr.h"
#include "../Parser/Stmt.h"
#include "../Parser/Environment.h"
#include "../Error_Handling/RunTimeError.h"
#include "Value.h"
#include <vector>
#include <stdexcept>

namespace lex
{
    class Interpreter : public ExprVisitor, public StmtVisitor
    {
    public:
        void interpret(const std::vector<StmtPtr> &statements);

        // Visit methods from ExprVisitor
        std::any visitBinaryExpr(Binary *expr) override;
        std::any visitGroupingExpr(Grouping *expr) override;
        std::any visitLiteralExpr(Literal *expr) override;
        std::any visitUnaryExpr(Unary *expr) override;
        std::any visitTernaryExpr(Ternary *expr) override;
        std::any visitVariableExpr(Variable *expr) override;
        std::any visitAssignExpr(Assign *expr) override;
        std::any visitLogicalExpr(Logical *expr) override;

        // Visit methods from StmtVisitor
        void visitExpressionStmt(ExpressionStmt *stmt) override;
        void visitPrintStmt(PrintStmt *stmt) override;
        void visitVariableStmt(VariableStmt *stmt) override;
        void visitBlockStmt(BlockStmt *stmt) override;
        void visitIfStmt(IfStmt *stmt) override;

    private:
        std::shared_ptr<Environment> environment = std::make_shared<Environment>();

        // Helper methods for evaluation
        void execute(const StmtPtr &stmt);
        void executeBlock(const std::vector<StmtPtr> &statements, std::shared_ptr<Environment> environment);
        Value evaluate(const ExprPtr &expr);
        void check_number_operand(const Token &operator_token, const Value &operand);
        void check_number_operands(const Token &operator_token, const Value &left, const Value &right);
    };
}