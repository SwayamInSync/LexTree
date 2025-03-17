#include "LexTree.h"
#include <iostream>

int main(int argc, const char ** argv)
{
    if (argc > 2)
    {
        std::cout << "Usage: lextree [script]" << std::endl;
        return 64;
    }
    else if (argc == 2)
    {
        LexTree::runFile(argv[1]);
    }
    else
    {
        LexTree::runPrompt();
    }

    return 0;
}