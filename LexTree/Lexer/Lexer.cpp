#include "../LexTree.h"
#include "Lexer.h"

namespace lex
{
    Lexer::Lexer(const std::string & source) : source(source) {}

    bool Lexer::is_at_end() const
    {
        return current >= static_cast<int>(source.length());
    }

    std::vector<Token> Lexer::scan_tokens()
    {
        while(!is_at_end())
        {
            // currently at beginning of next lexeme
            start = current;
            scan_token();
        }

        tokens.emplace_back(TokenType::EOF_TOKEN, "", std::monostate{}, line);
        return tokens;
    }

    void Lexer::scan_token()
    {
        char c = advance();

        switch (c) {
            // Single-character tokens
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '{': addToken(TokenType::LEFT_BRACE); break;
            case '}': addToken(TokenType::RIGHT_BRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case '-': addToken(TokenType::MINUS); break;
            case '+': addToken(TokenType::PLUS); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '*': addToken(TokenType::STAR); break;

            /*
             * Operators that could be one or two characters
             * I agree that compound operators also need such handling, but we are ignoring them for now
            */
            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;
            case '/':
                if (match('/'))
                {
                    // A comment goes until the end of the line (not adding this into tokens)
                    while (peek() != '\n' && !is_at_end())
                    {
                        advance();
                    }
                }
                else if(match('*'))
                {
                  multiline_comment();
                }
                else
                {
                    addToken(TokenType::SLASH); // treating as division operator
                }
                break;
            case '"':
                string();
                break;

            // Whitespace handling
            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++; // Increment line counter for newlines
                break;

            default:
                if(is_digit(c))
                    number();
                else if(is_alpha(c))
                    identifier();
                else
                    LexTree::error(line, "Unexpected character."); // still scanning continues
                break;
        }
    }

    char Lexer::advance()
    {
        return source[current++]; // return the current and increment current
    }

    void Lexer::addToken(TokenType type)
    {
        addToken(type, std::monostate{});
    }

    void Lexer::addToken(TokenType type, LiteralValue literal)
    {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, std::move(literal), line);
    }

    bool Lexer::match(char expected)
    {
        if (is_at_end()) return false;
        if (source[current] != expected) return false;

        current++;
        return true;
    }
    char Lexer::peek() const
    {
            if (is_at_end()) return '\0';
            return source[current];
    }

    char Lexer::peek_next() const
    {
        if(current + 1 >= source.length())
            return '\0';
        return source[current+1];
    }

    void Lexer::string()
    {
        /*
         * Lex has multi-line string support as "<l1>\n<l2>" basically no special symbol required like (""")
        */

        while(peek() != '"' && !is_at_end())
        {
            if(peek() == '\n')
                line++;
            advance();
        }

        if(is_at_end())
        {
            LexTree::error(line, "Unterminated string.");
            return;
        }

        // Consume the closing "
        advance();

        // trimming the surrounding quotes
        std::string value = source.substr(start+1, current - start - 2);
        addToken(TokenType::STRING, std::string(value));
    }

    bool Lexer::is_digit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    void Lexer::number()
    {
        while (is_digit(peek()))
            advance();

        // looking for fractional part
        if (peek() == '.' && is_digit(peek_next()))
        {
            advance();

            // consuming fractional part
            while (is_digit(peek()))
                advance();
        }

        double value = std::stod(source.substr(start, current-start));
        addToken(TokenType::NUMBER, value);
    }

    bool Lexer::is_alpha(char c) const
    {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    bool Lexer::is_alphanumeric(char c) const
    {
        return is_alpha(c) || is_digit(c);
    }


    void Lexer::identifier()
    {
        while (is_alphanumeric(peek()))
            advance();

        std::string text = source.substr(start, current-start);
        TokenType type = TokenType::IDENTIFIER; // default to identifier

        const auto& keywords = getKeywords();
        auto it = keywords.find(text);
        if (it != keywords.end())
        {
            type = it->second;
        }
        addToken(type);
    }

    void Lexer::multiline_comment() 
    {
      int nestLevel = 1;
      
      while (!is_at_end() && nestLevel > 0) {
          if (peek() == '/' && peek_next() == '*') {
              // Found a nested comment start
              advance(); // consume '/'
              advance(); // consume '*'
              nestLevel++;
          } else if (peek() == '*' && peek_next() == '/') {
              // Found a comment end
              advance(); // consume '*'
              advance(); // consume '/'
              nestLevel--;
          } else if (peek() == '\n') {
              line++; // Track line numbers
              advance();
          } else {
              advance();
          }
      }
      
      if (nestLevel > 0) {
          LexTree::error(line, "Unterminated block comment.");
      }
  }
}