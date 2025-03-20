#include "expr.h"
#include "printer.h"
#include "rpn_expressions.h"
#include <iostream>


using namespace lex;

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


void example_rpn_conversion() 
{
  // Example to demonstrate RPN conversion
  // This converts the expression: (1 + 2) * (4 - 3)
  // to RPN: 1 2 + 4 3 - *
  
  // Create tokens for operators
  Token plus(TokenType::PLUS, "+", std::monostate{}, 1);
  Token star(TokenType::STAR, "*", std::monostate{}, 1);
  Token minus(TokenType::MINUS, "-", std::monostate{}, 1);
  
  // Create literal expressions for the numbers
  auto one = make_literal(1.0);
  auto two = make_literal(2.0);
  auto three = make_literal(3.0);
  auto four = make_literal(4.0);
  
  // Build the expression: (1 + 2) * (4 - 3)
  auto addition = make_binary(one, plus, two);
  auto subtraction = make_binary(four, minus, three);
  auto multiplication = make_binary(
      make_grouping(addition),
      star,
      make_grouping(subtraction)
  );
  
  // Convert to RPN
  RPNPrinter rpnPrinter;
  std::string result = rpnPrinter.print(multiplication.get());
  std::cout << "Infix expression: (1 + 2) * (4 - 3)" << std::endl;
  std::cout << "RPN result: " << result << std::endl;
  // Expected output: 1 2 + 4 3 - *
}

void example_ast_usage() 
{
    // Example of creating and printing an AST
    // This would represent the expression: (1 + 2) * -3
    
    // Create tokens for operators and literals
    Token plus(TokenType::PLUS, "+", std::monostate{}, 1);
    Token star(TokenType::STAR, "*", std::monostate{}, 1);
    Token minus(TokenType::MINUS, "-", std::monostate{}, 1);
    
    // Create literal expressions for the numbers
    auto one = make_literal(1.0);
    auto two = make_literal(2.0);
    auto three = make_literal(3.0);
    
    // Build the expression: (1 + 2)
    auto addition = make_binary(one, plus, two);
    
    // Wrap in grouping: (1 + 2)
    auto grouping = make_grouping(addition);
    
    // Create unary expression: -3
    auto negation = make_unary(minus, three);
    
    // Create the final expression: (1 + 2) * -3
    auto expression = make_binary(grouping, star, negation);
    
    // Print the AST
    ASTPrinter printer;
    std::string result = printer.print(expression.get());
    std::cout << "AST: " << result << std::endl;
    // Expected output: (* (group (+ 1 2)) (- 3))
}

int main()
{
  example_ast_usage();
  example_rpn_conversion();
}