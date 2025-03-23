# Parser Implementation for Lex Language
> Again obviously this markdown is AI written, I only write code :)

This document explains the design and implementation of the parser for the Lex programming language. The parser transforms a stream of tokens (produced by the lexer) into an Abstract Syntax Tree (AST) representing the structure of the program.

## Table of Contents
1. [Grammar Definition](#grammar-definition)
2. [Grammar Stratification](#grammar-stratification)
3. [Recursive Descent Parsing](#recursive-descent-parsing)
4. [Parser Implementation](#parser-implementation)
5. [Error Handling Strategy](#error-handling-strategy)
6. [Example Parsing Walkthrough](#example-parsing-walkthrough)

## Grammar Definition

The grammar for Lex expressions defines the syntax rules that determine how expressions can be formed in the language. Initially, our grammar could be defined simply as:

```
expression → literal | unary | binary | grouping ;
literal    → NUMBER | STRING | "true" | "false" | "nil" ;
grouping   → "(" expression ")" ;
unary      → ("-" | "!") expression ;
binary     → expression operator expression ;
operator   → "==" | "!=" | "<" | "<=" | ">" | ">=" | "+" | "-" | "*" | "/" ;
```

However, this grammar is ambiguous because it doesn't specify precedence or associativity rules.

## Grammar Stratification

To eliminate ambiguity, we use a technique called *grammar stratification*. We restructure our grammar to encode operator precedence directly into the grammar rules, with each rule only referencing rules of higher precedence.

Here is our stratified, unambiguous grammar:

```
expression     → comma ;
comma          → conditional ( "," conditional )* ;
conditional    → equality ( "?" expression ":" conditional )? ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil" | IDENTIFIER
               | "(" expression ")" ;
```

### Precedence Levels (Lowest to Highest)

1. Comma (`,`)
2. Conditional (`?:`)
3. Equality (`==`, `!=`)
4. Comparison (`>`, `>=`, `<`, `<=`)
5. Terms (addition and subtraction: `+`, `-`)
6. Factors (multiplication and division: `*`, `/`)
7. Unary operations (`-`, `!`)
8. Primary expressions (literals, variables, and parenthesized expressions)

### AST Expression Types

Our AST now includes these expression types:
1. Binary - for binary operators (arithmetic, comparison, etc.)
2. Grouping - for parenthesized expressions
3. Literal - for literal values (numbers, strings, booleans, nil)
4. Unary - for unary operators (negation, logical not)
5. Ternary - for conditional expressions (? :)
6. Variable - for variable references

Each rule only matches expressions at its precedence level or higher, ensuring that higher precedence operations bind more tightly.

## Recursive Descent Parsing

We implement our parser using the recursive descent parsing technique. This approach:

1. Creates a set of mutually recursive functions (or methods) where each function corresponds to a grammar rule
2. Each function parses a specific syntactic construct and returns the corresponding AST node
3. The functions call each other according to the grammar structure, creating a top-down parsing process

Recursive descent parsing is particularly well-suited for our stratified grammar because the grammar's structure directly maps to function calls in the implementation.

## Parser Implementation

Our parser is implemented as a C++ class with the following key components:

### Token Management

```cpp
std::vector<Token> tokens;
int current = 0;
```

Similar to the lexer's character-by-character processing, the parser maintains a position in the token stream.

### Grammar Rule Methods

Each grammar rule becomes a method that produces a corresponding expression node:

```cpp
ExprPtr expression();
ExprPtr equality();
ExprPtr comparison();
ExprPtr term();
ExprPtr factor();
ExprPtr unary();
ExprPtr primary();
```

### Helper Methods

Various helper methods support the parsing process:

```cpp
Token peek() const;         // Look at current token
Token previous() const;     // Look at previous token
bool isAtEnd() const;       // Check if at end of tokens
Token advance();            // Move to next token
bool check(TokenType type); // Check token type without consuming
bool match(...);            // Check and consume token if it matches
Token consume(...);         // Consume token or report error
```

### Implementation Pattern for Binary Operators

All the binary operator methods (`equality()`, `comparison()`, `term()`, `factor()`) follow the same pattern:

```cpp
ExprPtr Parser::term()
{
    // term → factor ( ( "-" | "+" ) factor )*
    ExprPtr expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
        Token op = previous();
        ExprPtr right = factor();
        expr = make_Binary(expr, op, right);
    }

    return expr;
}
```

This pattern:
1. Parses a left operand using the higher precedence rule
2. Checks for operators at the current precedence level
3. If found, parses a right operand and creates a binary expression
4. Continues in a loop, accumulating operations from left to right (ensuring left associativity)

## Error Handling Strategy

Our parser implements a robust error handling strategy known as "panic mode recovery":

### 1. Error Detection and Reporting

When a syntax error is detected:

```cpp
ParseError error(const Token& token, const std::string& message)
{
    LexTree::error(token.line, message);
    return ParseError(message);
}
```

### 2. Exception Handling

We use exceptions to unwind the call stack when an error is encountered:

```cpp
ExprPtr Parser::parse()
{
    try
    {
        return expression();
    }
    catch (const ParseError&)
    {
        return nullptr;
    }
}
```

### 3. Synchronization for Statement Parsing

For statement-level parsing (to be implemented later), we include a synchronization method:

```cpp
void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}
```

This method discards tokens until it finds a statement boundary, enabling the parser to recover from errors and continue parsing the rest of the program.

## Example Parsing Walkthrough

To understand how the parser works, let's trace through parsing the expression `5 + 3 * 4`:

1. `expression()` calls `equality()`
2. `equality()` doesn't find equality operators, calls `comparison()`
3. `comparison()` doesn't find comparison operators, calls `term()`
4. `term()` parses the first factor (`5`)
5. `term()` finds a `+` operator
6. `term()` calls `factor()` to parse the right side of the `+` operator
7. `factor()` parses `3` as the left side of the next expression
8. `factor()` finds a `*` operator
9. `factor()` calls `unary()` to parse the right side of the `*` operator
10. `unary()` and `primary()` parse `4`
11. `factor()` creates a binary expression: `3 * 4`
12. `term()` creates a binary expression: `5 + (3 * 4)`
13. The final AST has `+` at the root, with `5` as left child and `*` as right child, which has children `3` and `4`

This corresponds to the output: `(+ 5 (* 3 4))` from the AST printer, which correctly reflects the precedence of operations where multiplication is performed before addition.
