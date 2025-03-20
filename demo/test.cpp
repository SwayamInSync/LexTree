#include "mock_ast.h"
#include "printer.h"
#include <iostream>

// Example of creating and printing an AST
// This would represent the expression: (1 + 2) * -3

void example_ast_usage() {
    using namespace lex;
    
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
}