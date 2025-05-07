#include "LexTree.h"
#include "Lexer/Lexer.h"
#include "Parser/parser.h"
#include "../utility/ASTPrinter.h"
#include "Interpreter/Interpreter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace lex
{
    bool LexTree::hadError = false;
    bool LexTree::hadRuntimeError = false;
    Interpreter LexTree::interpreter;

    void LexTree::runFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Could not open file: " << path << std::endl;
            exit(74);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        run(buffer.str());

        if (hadError)
            exit(65);
        if (hadRuntimeError)
            exit(70);
    }

    void LexTree::runPrompt()
    {
        std::string line;

        while (true)
        {
            std::cout << "> ";
            if (!std::getline(std::cin, line))
                break;
            if (line == "exit" || line == "quit" || line == ":q")
            {
                break;
            }

            run(line);
            hadError = false;
            hadRuntimeError = false;
        }
    }

    void LexTree::run(const std::string &source)
    {
        Lexer lexer = Lexer(source);
        std::vector<Token> tokens = lexer.scan_tokens();
        // for (const auto &token : tokens)
        //     std::cout << token.lexeme << " ";
        // std::cout << std::endl;
        Parser parser = Parser(tokens);
        std::vector<StmtPtr> statements = parser.parse();

        if (hadError)
            return;

        // Print AST structure
        // ASTPrinter printer;
        // std::cout << printer.print(expression.get()) << std::endl;

        // Interpret the expression
        interpreter.interpret(statements);

        // Only print the result if there was no runtime error
        // if (!hadRuntimeError)
        // {
        //     std::cout << "Result: " << value_to_string(result) << std::endl;
        // }
    }

    void LexTree::error(int line, const std::string &message)
    {
        report(line, "", message);
    }

    void LexTree::runtimeError(const RuntimeError &error)
    {
        std::cerr << error.what() << "\n[line " << error.token.line << "]" << std::endl;
        hadRuntimeError = true;
    }

    void LexTree::report(int line, const std::string &where,
                         const std::string &message)
    {
        std::cerr << "[line " << line << "] Error" << where
                  << ": " << message << std::endl;
        hadError = true;
    }
} // namespace lexx