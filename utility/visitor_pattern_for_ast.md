# Understanding the Visitor Pattern in Abstract Syntax Trees

> This is obviously AI written, I only write code not docs :)

## Introduction

When building a programming language interpreter or compiler, one of the key data structures is the Abstract Syntax Tree (AST). The AST represents the syntactic structure of the source code in a tree-like format, making it easier to analyze and manipulate. However, operating on an AST presents a challenge: we need to perform different operations on different node types, while also supporting the addition of new operations without modifying existing code.

This is where the Visitor Pattern comes in. The Visitor Pattern is an elegant solution to this problem, and it's the approach we're using in our Lex language implementation.

## The Visitor Pattern Explained

The Visitor Pattern is a behavioral design pattern that allows us to separate algorithms from the objects they operate on. In the context of an AST, it enables us to add new operations without changing the classes of the AST nodes we operate on.

The pattern involves two primary components:

1. **Visitors**: Classes that implement operations to be performed on elements of an object structure.
2. **Elements**: Classes that can be "visited" and that provide an "accept" method that takes a visitor as an argument.

The key insight of the Visitor Pattern is the concept of "double dispatch." In standard method calls, the method executed depends on:
- The object the method is invoked on
- The method's name and parameters

With the Visitor Pattern, the method executed depends on:
- The type of the element being visited
- The type of the visitor

## Implementation in Our Lex Language AST

Let's walk through how we've implemented the Visitor Pattern in our Lex language AST:

### 1. The Visitor Interface

We start by defining an interface for visitors that can operate on our AST:

```cpp
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    // One visit method for each expression type
    virtual std::any visitBinaryExpr(class Binary* expr) = 0;
    virtual std::any visitGroupingExpr(class Grouping* expr) = 0;
    virtual std::any visitLiteralExpr(class Literal* expr) = 0;
    virtual std::any visitUnaryExpr(class Unary* expr) = 0;
};
```

This interface declares a set of visit methods, one for each type of expression in our AST. Each method takes a pointer to an expression of a specific type.

### 2. The Expression Base Class

Next, we define an abstract base class for all expressions in our AST:

```cpp
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor* visitor) = 0;
};
```

The critical piece here is the `accept` method, which takes a visitor as an argument. This is the method that enables the double dispatch.

### 3. Concrete Expression Classes

Each concrete expression type implements the `accept` method to call the appropriate visit method on the visitor:

```cpp
class Binary : public Expr {
public:
    Binary(ExprPtr left, Token operator_token, ExprPtr right)
        : left(std::move(left)), operator_token(std::move(operator_token)), right(std::move(right)) {}

    std::any accept(ExprVisitor* visitor) override {
        return visitor->visitBinaryExpr(this);
    }

    const ExprPtr left;
    const Token operator_token;
    const ExprPtr right;
};
```

Notice how the `accept` method for `Binary` calls `visitBinaryExpr` on the visitor, passing itself (`this`) as the argument. This is the mechanism that enables the double dispatch.

### 4. Concrete Visitors

Finally, we create concrete visitor classes that implement the visitor interface to perform specific operations on the AST:

```cpp
class ASTPrinter : public ExprVisitor {
public:
    std::string print(Expr* expr) {
        return std::any_cast<std::string>(expr->accept(this));
    }

    std::any visitBinaryExpr(Binary* expr) override {
        return parenthesize(expr->operator_token.lexeme,
                           expr->left.get(), expr->right.get());
    }

    // ... other visit methods ...
};
```

The `ASTPrinter` is one example of a concrete visitor that traverses the AST to produce a string representation.

## The Flow of Control

To understand how this all works together, let's trace the flow of control when printing an AST:

1. We start by calling `printer.print(expression)` with the root of our AST.
2. Inside `print`, we call `expression->accept(this)`, passing the printer as the visitor.
3. Since `expression` might be a `Binary`, its `accept` method calls `visitor->visitBinaryExpr(this)`.
4. This calls `ASTPrinter::visitBinaryExpr`, which formats the binary expression as a string.
5. To format the left and right sub-expressions, it recursively calls `accept` on each one.
6. This process continues through the entire AST, with each node calling the appropriate visit method on the printer.

## Benefits of the Visitor Pattern

Using the Visitor Pattern in our AST provides several advantages:

1. **Separation of Concerns**: Expression classes focus on representing the structure, while visitors focus on operations.
2. **Extensibility**: We can add new operations (visitors) without changing the expression classes.
3. **Type Safety**: The compiler ensures that each visitor method handles the correct expression type.
4. **Encapsulation**: Each visitor encapsulates a cohesive operation, making the code more modular.

## Practical Applications

In the context of our Lex language, the Visitor Pattern enables us to implement multiple operations on our AST:

- **Printing**: For debugging and visualization
- **Evaluation**: To interpret the AST and execute the program
- **Type Checking**: To validate the semantic correctness of the program
- **Code Generation**: To translate the AST into machine code or another language

Each of these operations is implemented as a separate visitor, keeping the codebase clean and maintainable.

## Conclusion

The Visitor Pattern is a powerful tool in the implementation of programming language interpreters and compilers. By separating the structure of the AST from the operations performed on it, we gain flexibility, modularity, and maintainability.

In our Lex language implementation, the Visitor Pattern serves as the backbone for traversing and operating on the AST, enabling us to build a robust and extensible interpreter.