# Lexer in LexTree Interpreter

## Lexer

The lexer is the first stage of our interpreter. It takes raw source code as input and breaks it down into a sequence of tokens.
> It has nothing to do with the name 'lex' its just names sound relatable

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

## Lexical Analysis Process

The lexer's job is to transform a string of characters into a list of tokens through a process called "scanning" or "lexical analysis". Here's how our lexer works:

### 1. Main Scanning Loop

At its core, the lexer uses a loop that:
1. Starts at the beginning of the source code
2. Examines the current character
3. Determines what kind of token it might be part of
4. Consumes any additional characters needed to complete that token
5. Adds the token to a list
6. Repeats until it reaches the end of the source code

```
function scanTokens():
    while not at end of source:
        start = current position
        scanToken()
    
    add EOF token
    return tokens
```

### 2. Character Consumption

The lexer uses three primary methods for consuming characters:

- **advance()**: Consumes the current character and returns it
- **peek()**: Looks at the current character without consuming it (lookahead)
- **match(expected)**: Consumes the current character only if it matches the expected character (conditional advance)

These methods allow for flexible pattern recognition:

```
function advance():
    current++
    return source[current-1]

function peek():
    if atEnd(): return '\0'
    return source[current]

function match(expected):
    if atEnd() or source[current] != expected: return false
    current++
    return true
```

### 3. Token Types and Recognition

#### Single-Character Tokens

Simple tokens like parentheses, braces, and operators are recognized directly:

```
function scanToken():
    c = advance()
    
    switch c:
        case '(': addToken(LEFT_PAREN)
        case ')': addToken(RIGHT_PAREN)
        case '{': addToken(LEFT_BRACE)
        case '}': addToken(RIGHT_BRACE)
        case ',': addToken(COMMA)
        case '.': addToken(DOT)
        case '-': addToken(MINUS)
        case '+': addToken(PLUS)
        case ';': addToken(SEMICOLON)
        case '*': addToken(STAR)
        // ...and so on
```

#### Two-Character Operators

For operators like `!=`, `==`, `<=`, and `>=`, the lexer checks the next character:

```
case '!':
    addToken(match('=') ? BANG_EQUAL : BANG)
case '=':
    addToken(match('=') ? EQUAL_EQUAL : EQUAL)
case '<':
    addToken(match('=') ? LESS_EQUAL : LESS)
case '>':
    addToken(match('=') ? GREATER_EQUAL : GREATER)
```

#### Whitespace and Comments

Whitespace is ignored, and comments are recognized and skipped:

```
case ' ':
case '\r':
case '\t':
    // Ignore whitespace
case '\n':
    line++
case '/':
    if match('/'):
        // Comment goes until end of line
        while peek() != '\n' and not atEnd():
            advance()
    else:
        addToken(SLASH)
```

#### String Literals

Strings start with a double quote and continue until another double quote:

```
function string():
    while peek() != '"' and not atEnd():
        if peek() == '\n': line++
        advance()
    
    if atEnd():
        error("Unterminated string")
        return
    
    // Consume the closing "
    advance()
    
    // Extract the string value (without quotes)
    value = source.substring(start+1, current-1)
    addToken(STRING, value)
```

#### Number Literals

Numbers consist of digits and an optional decimal point:

```
function number():
    while isDigit(peek()):
        advance()
    
    // Look for fractional part
    if peek() == '.' and isDigit(peekNext()):
        // Consume the dot
        advance()
        
        while isDigit(peek()):
            advance()
    
    // Convert the lexeme to a numeric value
    value = parseDouble(source.substring(start, current))
    addToken(NUMBER, value)
```

#### Identifiers and Keywords

Identifiers start with a letter or underscore and can contain letters, digits, or underscores:

```
function identifier():
    while isAlphaNumeric(peek()):
        advance()
    
    // Check if it's a reserved keyword
    text = source.substring(start, current)
    type = keywords.get(text, IDENTIFIER)
    addToken(type)
```

### 4. Lexical Analysis Design Principles

#### Maximal Munch Rule

The lexer follows the "maximal munch" principle: when two rules could match a sequence of characters, the one that consumes the most characters wins. For example, in `orchid`, we match the entire word as an identifier rather than as the keyword `or` followed by `chid`.

#### Error Handling

The lexer reports errors but continues scanning to find as many errors as possible in a single pass. This provides a better user experience than stopping at the first error.

#### Line Tracking

The lexer keeps track of line numbers for error reporting. It increments the line counter when it encounters a newline character.

### Conclusion

The lexer in our LexTree interpreter is a crucial first step in the compilation process. It takes raw source code and transforms it into a sequence of tokens that the parser can understand. By breaking down the code into meaningful units, it lays the foundation for all subsequent phases of interpretation.