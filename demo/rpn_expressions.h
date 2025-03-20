#pragma once

#include "expr.h"
#include <string>
#include <sstream>

namespace lex
{
  class RPNPrinter: ExprVisitor
  {
  public:
    std::string print(Expr * expr)
    {
      // entrance point
      return std::any_cast<std::string>(expr->accept(this));
    }

    std::any visitBinaryExpr(Binary * expr) override
    {
      std::stringstream ss;

      // first the left operand
      ss << std::any_cast<std::string>(expr->left->accept(this));
      ss << " ";

      // now right operand;
      ss << std::any_cast<std::string>(expr->right->accept(this));
      ss << " ";

      // now operation
      ss << expr->operator_token.lexeme;
      ss << " ";
      return ss.str();

    }

    std::any visitGroupingExpr(Grouping * expr) override
    {
      // order is determined by position of operators so not needed
      return expr->expression->accept(this);
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
          std::stringstream ss;
          ss << std::get<double>(expr->value);
          return ss.str();
      }
      else if (std::holds_alternative<bool>(expr->value)) {
          return std::get<bool>(expr->value) ? std::string("true") : std::string("false");
      }
      
      return std::string("unknown");
    }

    std::any visitUnaryExpr(Unary* expr) override
    {
      std::stringstream ss;

      // first the operator
      ss << std::any_cast<std::string>(expr->right->accept(this));
      ss << " ";

      // then the operand
      ss << expr->operator_token.lexeme;
      return ss.str();

    }
  };
}