/*
- This file is a demo file demonstrating the BASE code structure of AST-generator (this phase is autommated)
- READ ONLY and understanding the visitor pattern
- I also added a markdown file explaining this pattern inside utility/visitor_pattern.md :)
*/

#pragma once

#include "LexTree/Lexer/Token.h"
#include <memory>
#include <any>
#include <variant>

namespace lex
{
  class Expr;
  using ExprPtr = std::shared_ptr<Expr>;
  

  // visitor interface (for double dispatch)
  class ExprVisitor
  {
  public:
    virtual ~ExprVisitor() = default;

    // visit methods for each expression type
    virtual std::any visitBinaryExpr(class Binary* expr) = 0;
    virtual std::any visitGroupingExpr(class Grouping* expr) = 0;
    virtual std::any visitLiteralExpr(class Literal* expr) = 0;
    virtual std::any visitUnaryExpr(class Unary* expr) = 0;
  };

  class Expr
  {
  public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor* visitor) = 0;
  };

  class Binary: public Expr
  {
  public:
    const ExprPtr left;
    const Token operator_token;
    const ExprPtr right;

    Binary(ExprPtr left, Token operator_token, ExprPtr right)
      : left(std::move(left)), operator_token(std::move(operator_token)), right(std::move(right)) {}

    std::any accept(ExprVisitor * visitor) override
    {
      return visitor->visitBinaryExpr(this);
    }
  };

  class Grouping: public Expr
  {
  public:
    const ExprPtr expression;

    explicit Grouping(ExprPtr expression) : expression(std::move(expression)) {}

    std::any accept(ExprVisitor * visitor) override
    {
      return visitor->visitGroupingExpr(this);
    }
  };

  class Literal: public Expr
  {
  public:
    const LiteralValue value;

    explicit Literal(LiteralValue value): value(std::move(value)) {}

    std::any accept(ExprVisitor * visitor) override
    {
      return visitor->visitLiteralExpr(this);
    }
  };

  class Unary: public Expr
  {
  public:
    const Token operator_token;
    const ExprPtr right;

    Unary(Token operator_token, ExprPtr expression): operator_token(std::move(operator_token)), right(std::move(expression)) {}

    std::any accept(ExprVisitor * visitor) override
    {
      return visitor->visitUnaryExpr(this);
    }
  };


  // helper function to create shared pointers for each expression type
  inline ExprPtr make_binary(ExprPtr left, Token operator_token, ExprPtr right) 
  {
    return std::make_shared<Binary>(std::move(left), std::move(operator_token), std::move(right));
  }

  inline ExprPtr make_grouping(ExprPtr expression) 
  {
      return std::make_shared<Grouping>(std::move(expression));
  }

  inline ExprPtr make_literal(LiteralValue value) 
  {
      return std::make_shared<Literal>(std::move(value));
  }

  inline ExprPtr make_unary(Token operator_token, ExprPtr right) 
  {
      return std::make_shared<Unary>(std::move(operator_token), std::move(right));
  }
}


