cmake_minimum_required(VERSION 3.28)
project(LexTree)

set(CMAKE_CXX_STANDARD 20)

add_executable(LexTree
        LexTree/LexTree.h
        LexTree/LexTree.cpp
        main.cpp
        LexTree/Lexer/TokenType.h
        LexTree/Lexer/Token.cpp
        LexTree/Lexer/Token.h
        LexTree/Lexer/Lexer.cpp
        LexTree/Lexer/Lexer.h
        LexTree/Parser/Parser.cpp
        LexTree/Parser/Parser.h
        LexTree/Interpreter/Interpreter.cpp
        LexTree/Interpreter/Interpreter.h
)
