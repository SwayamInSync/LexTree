#pragma once

#include <string>
#include <vector>

namespace lex
{
    class LexTree {
    public:
        static bool hadError;

        static void report(int line, const std::string &where,
                           const std::string &message);

        static void runFile(const std::string &path);

        static void runPrompt();

        static void run(const std::string &source);

        static void error(int line, const std::string &message);
    };
} // namespace lex