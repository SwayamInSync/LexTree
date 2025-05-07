cmake_minimum_required(VERSION 3.28)
project(LexTree)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(LexTree
        LexTree/LexTree.h
        LexTree/LexTree.cpp
        main.cpp
        LexTree/Lexer/TokenType.h
        LexTree/Lexer/Token.cpp
        LexTree/Lexer/Token.h
        LexTree/Lexer/Lexer.cpp
        LexTree/Lexer/Lexer.h
        LexTree/Parser/parser.cpp
        LexTree/Parser/parser.h
        LexTree/Parser/Environment.h
        LexTree/Interpreter/Interpreter.cpp
        LexTree/Interpreter/Interpreter.h
        LexTree/Interpreter/Value.h
)
