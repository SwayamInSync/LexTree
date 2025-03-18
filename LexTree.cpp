#include "LexTree.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace lex
{
    bool LexTree::hadError = false;

    void LexTree::runFile(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << path << std::endl;
            exit(74);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        run(buffer.str());

        if (hadError) exit(65);
    }

    void LexTree::runPrompt() {
        std::string line;

        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            if (line == "exit" || line == "quit" || line == ":q") {
                break;
            }

            run(line);
            hadError = false; // to avoid killing complete session
        }
    }

    void LexTree::run(const std::string &source) {
        std::cout << source << "\n";
        error(2, "yo");
    }

    void LexTree::error(int line, const std::string &message) {
        report(line, "", message);
    }

    void LexTree::report(int line, const std::string &where,
                         const std::string &message) {
        std::cerr << "[line " << line << "] Error" << where
                  << ": " << message << std::endl;
        hadError = true;
    }
} // namespace lexx