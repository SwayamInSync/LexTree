#pragma once

#include "Expr.h"

namespace lex
{
    class Stmt;
    using StmtPtr = std::shared_ptr<Stmt>;

    class StmtVisitor
    {
    public:
        virtual ~StmtVisitor() = default;

        virtual void visitExpressionStmt(class ExpressionStmt *stmt) = 0;
        virtual void visitPrintStmt(class PrintStmt *stmt) = 0;
    };

    // Base Statement class
    class Stmt
    {
    public:
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor *visitor) = 0;
    };

    // subclasses
    class ExpressionStmt : public Stmt
    {
    public:
        const ExprPtr expression;

        ExpressionStmt(ExprPtr expression)
            : expression(std::move(expression))
        {
        }

        void accept(StmtVisitor *visitor) override
        {
            visitor->visitExpressionStmt(this);
        }
    };

    class PrintStmt : public Stmt
    {
    public:
        const ExprPtr expression;

        PrintStmt(ExprPtr expression)
            : expression(std::move(expression))
        {
        }

        void accept(StmtVisitor *visitor) override
        {
            visitor->visitPrintStmt(this);
        }
    };

    // helper functions to create shared pointers for each statement type
    inline StmtPtr make_ExpressionStmt(ExprPtr expression)
    {
        return std::make_shared<ExpressionStmt>(std::move(expression));
    }
    inline StmtPtr make_PrintStmt(ExprPtr expression)
    {
        return std::make_shared<PrintStmt>(std::move(expression));
    }
}