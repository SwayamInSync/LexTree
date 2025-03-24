#pragma once

#include <string>
#include <vector>
#include "Interpreter/Interpreter.h"

namespace lex
{
    class LexTree {
    public:
        static bool hadError;
        static bool hadRuntimeError;

        static void report(int line, const std::string &where,
                           const std::string &message);

        static void runFile(const std::string &path);

        static void runPrompt();

        static void run(const std::string &source);

        static void error(int line, const std::string &message);

        static void runtimeError(const RuntimeError& error);
    };
} // namespace lex