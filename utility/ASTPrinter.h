#pragma once

#include "expr.h"
#include <string>
#include <any>
#include <sstream>

namespace lex
{
  class ASTPrinter: public ExprVisitor
  {
  private:
    std::string parenthesize(const std::string & name, Expr * expr)
    {
      std::ostringstream oss;
      oss << "(" << name << " ";
      oss << std::any_cast<std::string>(expr->accept(this));
      oss << ")";
      return oss.str();
    }

    std::string parenthesize(const std::string& name, Expr* left, Expr* right) 
    {
      std::ostringstream oss;
      oss << "(" << name << " ";
      oss << std::any_cast<std::string>(left->accept(this)) << " ";
      oss << std::any_cast<std::string>(right->accept(this));
      oss << ")";
      return oss.str();
    }

  public:
    std::string print(Expr * expr)
    {
      return std::any_cast<std::string>(expr->accept(this));
    }

    std::any visitBinaryExpr(Binary* expr) override
    {
      return parenthesize(expr->operator_token.lexeme, expr->left.get(), expr->right.get());
    }
    std::any visitGroupingExpr(Grouping* expr) override {
      return parenthesize("group", expr->expression.get());
    }

    std::any visitLiteralExpr(Literal* expr) override 
    {
        if (std::holds_alternative<std::monostate>(expr->value)) {
            return std::string("nil");
        }
        else if (std::holds_alternative<std::string>(expr->value)) {
            return std::get<std::string>(expr->value);
        }
        else if (std::holds_alternative<double>(expr->value)) {
            std::ostringstream oss;
            oss << std::get<double>(expr->value);
            return oss.str();
        }
        else if (std::holds_alternative<bool>(expr->value)) {
            return std::get<bool>(expr->value) ? std::string("true") : std::string("false");
        }
        
        return std::string("unknown literal");
    }

    std::any visitUnaryExpr(Unary* expr) override 
    {
        return parenthesize(expr->operator_token.lexeme, expr->right.get());
    }
  };
}