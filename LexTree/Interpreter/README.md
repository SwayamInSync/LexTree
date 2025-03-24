# Lex Interpreter Design

> This document is AI written for ya'll

The Lex interpreter is the component of our language implementation that evaluates the Abstract Syntax Tree (AST) and produces runtime values. This document explains the design choices, implementation details, and execution flow of the interpreter.

## Table of Contents

1. [Design Overview](#design-overview)
2. [Value Representation](#value-representation)
3. [Visitor Pattern Implementation](#visitor-pattern-implementation)
4. [Expression Evaluation](#expression-evaluation)
5. [Error Handling](#error-handling)
6. [Example: Evaluating an Expression](#example-evaluating-an-expression)

## Design Overview

The interpreter follows a tree-walk approach, where it traverses the AST and evaluates each node according to its type. This design offers several advantages:

- **Simplicity**: The implementation is straightforward and closely follows the language grammar.
- **Flexibility**: It's easy to add new features and language constructs.
- **Debuggability**: The execution process is transparent and easy to trace.

While tree-walking interpreters are not the most performant approach (compared to bytecode VMs or JIT compilers), they are ideal for educational purposes and for languages where development speed is prioritized over execution speed.

## Value Representation

In C++, we need a way to represent values of different types that can be determined at runtime. We use `std::variant` for this purpose:

```cpp
// Runtime value representation
using Value = std::variant<
    std::monostate,  // nil
    bool,            // boolean
    double,          // number
    std::string      // string
>;
```

This approach offers several benefits:

1. **Type Safety**: `std::variant` provides type-safe access to its contents.
2. **Performance**: It's more efficient than dynamic allocation since the value is stored inline.
3. **Pattern Matching**: We can use `std::visit` or `std::holds_alternative` for type checking.

Helper functions like `is_truthy` and `values_equal` simplify common operations on these values.

## Visitor Pattern Implementation

The interpreter uses the Visitor pattern to traverse and evaluate the AST. This pattern separates the algorithm (evaluation) from the object structure (AST nodes).

### How the Visitor Pattern Works

1. Each expression class in the AST implements an `accept` method.
2. The `accept` method calls the appropriate `visit` method on the visitor.
3. The interpreter class implements the visitor interface.

This creates a double dispatch mechanism where the execution depends on both the expression type and the visitor type.

```cpp
// Base Expression class
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor* visitor) = 0;
};

// Concrete expression class
class Binary : public Expr {
public:
    // ...
    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitBinaryExpr(this);
    }
    // ...
};

// Interpreter visitor
class Interpreter : public ExprVisitor {
public:
    // ...
    std::any visitBinaryExpr(Binary* expr) override {
        // Evaluation logic for binary expressions
    }
    // ...
};
```

### Post-Order Traversal

The interpreter performs a post-order traversal of the AST, meaning it evaluates child nodes before their parents. This is essential for expressions where the result depends on evaluating sub-expressions first.

For example, in a binary expression like `a + b`, we first evaluate `a`, then `b`, and finally perform the addition.

## Expression Evaluation

The core of the interpreter is its expression evaluation logic. Here's how each expression type is handled:

### Literals

Literal expressions (numbers, strings, booleans, nil) simply return their value.

```cpp
std::any Interpreter::visitLiteralExpr(Literal* expr) {
    return Value(expr->value);
}
```

### Grouping Expressions

Grouping expressions evaluate their inner expression.

```cpp
std::any Interpreter::visitGroupingExpr(Grouping* expr) {
    return evaluate(expr->expression);
}
```

### Unary Expressions

Unary expressions evaluate their operand and then apply the operator.

```cpp
std::any Interpreter::visitUnaryExpr(Unary* expr) {
    Value right = evaluate(expr->right);

    switch (expr->operator_token.type) {
        case TokenType::BANG:
            return Value(!is_truthy(right));
        case TokenType::MINUS:
            check_number_operand(expr->operator_token, right);
            return Value(-std::get<double>(right));
        // ...
    }
}
```

### Binary Expressions

Binary expressions evaluate both operands and then apply the operator.

```cpp
std::any Interpreter::visitBinaryExpr(Binary* expr) {
    Value left = evaluate(expr->left);
    Value right = evaluate(expr->right);

    switch (expr->operator_token.type) {
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return Value(std::get<double>(left) + std::get<double>(right));
            }
            // String concatenation cases...
        // Other operators...
    }
}
```

### Ternary Expressions

Ternary expressions (condition ? then_expr : else_expr) evaluate the condition first, then either the then branch or the else branch.

```cpp
std::any Interpreter::visitTernaryExpr(Ternary* expr) {
    Value condition = evaluate(expr->condition);
    
    if (is_truthy(condition)) {
        return evaluate(expr->then_branch);
    } else {
        return evaluate(expr->else_branch);
    }
}
```

## Error Handling

The interpreter includes robust error handling for runtime errors:

1. **Runtime Error Class**: A custom exception class that captures both the error message and the token where the error occurred.
2. **Type Checking**: Methods like `check_number_operand` verify operand types before operations.
3. **Error Reporting**: The `LexTree::runtimeError` method reports errors with line numbers.
4. **Error Recovery**: The REPL allows continued execution after errors.

```cpp
class RuntimeError : public std::runtime_error {
public:
    const Token token;
    
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

Value Interpreter::interpret(const ExprPtr& expression) {
    try {
        Value value = evaluate(expression);
        return value;
    } catch (const RuntimeError& error) {
        LexTree::runtimeError(error);
        return std::monostate{};
    }
}
```

## Example: Evaluating an Expression

Let's trace through the evaluation of a simple expression: `5 + 3 * 4`

### AST Construction

The parser constructs the following AST:

```
    +
   / \
  5   *
     / \
    3   4
```

In our representation, this looks like:

```
Binary(
  Literal(5),
  "+",
  Binary(
    Literal(3),
    "*",
    Literal(4)
  )
)
```

### Evaluation Process

1. Start at the root (Binary `+` expression)
2. Evaluate the left operand: Literal `5` → 5
3. Evaluate the right operand (Binary `*` expression)
   a. Evaluate the left operand: Literal `3` → 3
   b. Evaluate the right operand: Literal `4` → 4
   c. Perform the operation: 3 * 4 = 12
4. Perform the root operation: 5 + 12 = 17

The result is 17.

### Detailed Traversal

1. `interpret` calls `evaluate` on the root expression
2. `evaluate` calls `accept` on the Binary `+` node
3. `accept` calls `visitBinaryExpr` on the interpreter
4. `visitBinaryExpr` calls `evaluate` on the left child (Literal `5`)
5. `evaluate` calls `accept` on the Literal `5` node
6. `accept` calls `visitLiteralExpr` which returns 5
7. Back in `visitBinaryExpr`, `evaluate` is called on the right child (Binary `*`)
8. This process repeats for the `*` expression and its children
9. Once both operands are evaluated, `visitBinaryExpr` performs the addition

This demonstrates the post-order traversal, where children are fully evaluated before their parent operations are performed.
