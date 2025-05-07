#include "Interpreter.h"
#include "../LexTree.h"
#include <iostream>
#include <any>

namespace lex
{
    // Convert value to string for printing
    std::string value_to_string(const Value &value)
    {
        if (std::holds_alternative<std::monostate>(value))
        {
            return "nil";
        }
        else if (std::holds_alternative<bool>(value))
        {
            return std::get<bool>(value) ? "true" : "false";
        }
        else if (std::holds_alternative<double>(value))
        {
            std::string text = std::to_string(std::get<double>(value));
            // Remove trailing zeros
            if (text.find('.') != std::string::npos)
            {
                text = text.substr(0, text.find_last_not_of('0') + 1);
                if (text.back() == '.')
                    text = text.substr(0, text.size() - 1);
            }
            return text;
        }
        else if (std::holds_alternative<std::string>(value))
        {
            return std::get<std::string>(value);
        }
        return "unknown";
    }

    void Interpreter::check_number_operand(const Token &operator_token, const Value &operand)
    {
        if (std::holds_alternative<double>(operand))
            return;
        throw RuntimeError(operator_token, "Operand must be a number.");
    }

    void Interpreter::check_number_operands(const Token &operator_token, const Value &left, const Value &right)
    {
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            return;
        throw RuntimeError(operator_token, "Operands must be numbers.");
    }

    void Interpreter::interpret(const std::vector<StmtPtr> &statements)
    {
        for (const auto &statement : statements)
        {
            try
            {
                execute(statement);
            }
            catch (const RuntimeError &error)
            {
                LexTree::runtimeError(error);
            }
        }
    }

    void Interpreter::execute(const StmtPtr &stmt)
    {
        stmt->accept(this);
    }

    void Interpreter::executeBlock(const std::vector<StmtPtr> &statements, std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previous = this->environment;
        try
        {
            this->environment = environment;
            // execute each statement in the block in the new environment
            for (const auto &statement : statements)
            {
                execute(statement);
            }
        }
        catch (const std::exception &e)
        {
            this->environment = previous; // restore the previous environment
            throw;                        // rethrow the exception
        }

        this->environment = previous; // restore the previous environment
    }

    Value Interpreter::evaluate(const ExprPtr &expression)
    {
        return std::any_cast<Value>(expression->accept(this));
    }

    void Interpreter::visitExpressionStmt(ExpressionStmt *stmt)
    {
        evaluate(stmt->expression);
        return;
    }

    void Interpreter::visitPrintStmt(PrintStmt *stmt)
    {
        Value value = evaluate(stmt->expression);
        std::cout << value_to_string(value) << std::endl;
        return;
    }

    void Interpreter::visitVariableStmt(VariableStmt *stmt)
    {
        Value value;
        if (stmt->initializer != nullptr)
        {
            value = evaluate(stmt->initializer);
        }

        environment->define(stmt->name.lexeme, value);
    }

    void Interpreter::visitBlockStmt(BlockStmt *stmt)
    {
        std::shared_ptr<Environment> block_environment = std::make_shared<Environment>(environment); // create a new env with the current environment as enclosing
        executeBlock(stmt->statements, block_environment);
        // block_environment will be destroyed when it goes out of scope
        // no need to restore the environment here, as it will be done automatically
    }

    // Expressions returns the evaluated value

    std::any Interpreter::visitGroupingExpr(lex::Grouping *expr)
    {
        return evaluate(expr->expression);
    }

    std::any Interpreter::visitLiteralExpr(lex::Literal *expr)
    {
        if (std::holds_alternative<std::monostate>(expr->value))
            return Value(std::monostate{});
        else if (std::holds_alternative<double>(expr->value))
            return Value(std::get<double>(expr->value));
        else if (std::holds_alternative<std::string>(expr->value))
            return Value(std::get<std::string>(expr->value));
        else if (std::holds_alternative<bool>(expr->value))
            return Value(std::get<bool>(expr->value));
        return Value(std::monostate{});
    }

    std::any Interpreter::visitUnaryExpr(lex::Unary *expr)
    {
        Value right = evaluate(expr->right);
        switch (expr->operator_token.type)
        {
        case TokenType::BANG:
            return Value(!is_truthy(right));
        case TokenType::MINUS:
            check_number_operand(expr->operator_token, right);
            return Value(-std::get<double>(right));
        default:
            // Unreachable
            return Value(std::monostate{});
        }
    }

    std::any Interpreter::visitBinaryExpr(lex::Binary *expr)
    {
        Value left = evaluate(expr->left);
        Value right = evaluate(expr->right);

        switch (expr->operator_token.type)
        {
        // Arithmetic operations
        case TokenType::MINUS:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) - std::get<double>(right));
        case TokenType::SLASH:
            check_number_operands(expr->operator_token, left, right);
            // Check for division by zero
            if (std::get<double>(right) == 0.0)
            {
                throw RuntimeError(expr->operator_token, "Division by zero.");
            }
            return Value(std::get<double>(left) / std::get<double>(right));
        case TokenType::STAR:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) * std::get<double>(right));
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                return Value(std::get<double>(left) + std::get<double>(right));

            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                return Value(std::get<std::string>(left) + std::get<std::string>(right));

            // Allow string concatenation with other types
            if (std::holds_alternative<std::string>(left))
                return Value(std::get<std::string>(left) + value_to_string(right));

            if (std::holds_alternative<std::string>(right))
                return Value(value_to_string(left) + std::get<std::string>(right));

            throw RuntimeError(expr->operator_token,
                               "Operands must be two numbers or two strings.");
            // Comparison operations
        case TokenType::GREATER:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) > std::get<double>(right));
        case TokenType::GREATER_EQUAL:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) >= std::get<double>(right));
        case TokenType::LESS:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) < std::get<double>(right));
        case TokenType::LESS_EQUAL:
            check_number_operands(expr->operator_token, left, right);
            return Value(std::get<double>(left) <= std::get<double>(right));

            // Equality operations
        case TokenType::BANG_EQUAL:
            return Value(!values_equal(left, right));
        case TokenType::EQUAL_EQUAL:
            return Value(values_equal(left, right));

        case TokenType::COMMA:
            // Comma operator returns the value of the right-hand operand
            return Value(right);

        default:
            // Unreachable
            return Value(std::monostate{});
        }
    }

    std::any Interpreter::visitTernaryExpr(Ternary *expr)
    {
        Value condition = evaluate(expr->condition);
        if (is_truthy(condition))
            return evaluate(expr->then_branch);
        return evaluate(expr->else_branch);
    }

    std::any Interpreter::visitVariableExpr(Variable *expr)
    {
        Value value = environment->get(expr->name); // this just retrieves the value from the environment
        if (std::holds_alternative<std::monostate>(value))
        {
            throw RuntimeError(expr->name, "Uninitialized variable: " + expr->name.lexeme);
        }
        return value;
    }

    std::any Interpreter::visitAssignExpr(Assign *expr)
    {
        Value value = evaluate(expr->value);
        environment->assign(expr->name, value);
        return value;
    }
}
