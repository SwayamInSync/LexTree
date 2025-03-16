#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class LexTree {
private:

public:
    static void runFile(const std::string& path) {
        try {
            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open file: " + path);
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                                (std::istreambuf_iterator<char>()));
            run(content);

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(74);
        }
    }

    static void runPrompt() {
        std::cout << "Hit CTRL+D to exit" << std::endl;
        try {
            std::string line;
            while (true) {
                std::cout << "> ";
                if (!std::getline(std::cin, line)) break;
                run(line);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in REPL: " << e.what() << std::endl;
        }
    }

    static void run(const std::string& source) {
        std::cout << source << std::endl;
    }

};


int main(int argc, char* argv[]) {
    try {
        if (argc > 2) {
            std::cout << "Usage: lox [script]" << std::endl;
            return 64;
        } else if (argc == 2) {
            LexTree::runFile(argv[1]);
        } else {
            LexTree::runPrompt();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 70;
    }

    return 0;
}