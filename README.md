# LexTree: A minimal tree-walk interpreter for Lex language

> Thinking to make this readme like an article as I implement this but let's see where it goes :)

## Lexer

The lexer (also called scanner) is the first stage of our interpreter. It takes raw source code as input and breaks it down into a sequence of tokens.

### Understanding Tokens

A token is a meaningful unit of code. Think of source code as a sentence and tokens as the individual words and punctuation. For example, in the Lex statement `var x = 10;`, there are 5 tokens:

1. The `var` keyword
2. The `x` identifier
3. The `=` equal sign
4. The `10` number literal
5. The `;` semicolon

Each token contains several important pieces of information:

- **TokenType**: Categorizes what kind of token it is (keyword, operator, etc.)
- **Lexeme**: The actual string of characters from the source
- **Literal Value**: For tokens like numbers or strings, the actual runtime value
- **Line Number**: For error reporting

#### Difference between Lexeme and Literal

- **Lexeme**: The raw text from the source code (e.g., `"hello"` including quotation marks)
- **Literal**: The runtime value that the lexeme represents (e.g., the string `hello` without quotation marks)

### Example

Consider this line of Lex code:
```
if (score >= 100) print "You win!";
```
The tokens would be:

| Token Type | Lexeme   | Literal Value | Line |
|------------|----------|---------------|------|
| IF         | "if"     | null          | 1    |
| LEFT_PAREN | "("      | null          | 1    |
| IDENTIFIER | "score"  | null          | 1    |
| GREATER_EQUAL | ">="  | null          | 1    |
| NUMBER     | "100"    | 100.0         | 1    |
| RIGHT_PAREN| ")"      | null          | 1    |
| PRINT      | "print"  | null          | 1    |
| STRING     | "\"You win!\"" | "You win!" | 1  |
| SEMICOLON  | ";"      | null          | 1    |

The lexer processes the source code character by character, recognizes patterns, and generates these tokens which will later be processed by the parser.