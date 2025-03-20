# Utilities

These are some file that act as helper for implementing LexTree.

- `grammar.def` file contains the rules that define the Lex's grammar
- `generate_ast.cpp` file reads the `grammar.def` and automatically creates an AST implementation to use. In order to learn what this representation looks, checkout the `mock_ast.cpp` (It is the same file but written by hands) Automating as it gets tiring to write repetitive for each production rules. Also this is implemented using the "Visitor Pattern" code-design, to learn about that checkout `visitor_pattern_for_ast.md`
- `ASTPrinter` is a utility that prints the generated AST tree (just for debugging)
- `rpn_expressions.h` is again a utility that maps the expressions to their reverse polish notations