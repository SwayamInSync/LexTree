#pragma once

#include <vector>
#include <memory>
#include "Expr.h"

namespace lex
{
    class Stmt;
    using StmtPtr = std::shared_ptr<Stmt>;

    class StmtVisitor
    {
    public:
        virtual ~StmtVisitor() = default;

        // statements themselves don't have any precedence ordering
        virtual void visitExpressionStmt(class ExpressionStmt *stmt) = 0;   // Expression statement
        virtual void visitPrintStmt(class PrintStmt *stmt) = 0;      // Print statement
        virtual void visitVariableStmt(class VariableStmt *stmt) = 0; // Variable declaration statement
        virtual void visitBlockStmt(class BlockStmt * stmt) = 0;      // Block statement
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

    class VariableStmt: public Stmt
    {
      public:
        const Token name;
        const ExprPtr initializer;

        VariableStmt(Token name, ExprPtr initializer)
            : name(std::move(name)), initializer(std::move(initializer))
        {
        }

        void accept(StmtVisitor *visitor) override
        {
            visitor->visitVariableStmt(this);
        }
    };

    class BlockStmt : public Stmt
    {
    public:
        const std::vector<StmtPtr> statements;

        explicit BlockStmt(std::vector<StmtPtr> statements)
            : statements(std::move(statements))
        {
        }

        void accept(StmtVisitor *visitor) override
        {
            visitor->visitBlockStmt(this);
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
    inline StmtPtr make_VariableStmt(Token name, ExprPtr initializer)
    {
        return std::make_shared<VariableStmt>(std::move(name), std::move(initializer));
    }

    inline StmtPtr make_BlockStmt(std::vector<StmtPtr> statements)
    {
        return std::make_shared<BlockStmt>(std::move(statements));
    }
}