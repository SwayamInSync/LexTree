cmake_minimum_required(VERSION 3.28)
project(LexTree)

set(CMAKE_CXX_STANDARD 20)

add_executable(LexTree
        LexTree.h
        LexTree.cpp
        main.cpp
        Lexer/TokenType.h
        Lexer/Token.cpp
        Lexer/Token.h
        Lexer/Lexer.cpp
        Lexer/Lexer.h
)
