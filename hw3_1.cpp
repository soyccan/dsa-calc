#include <iostream>
#include <string>

#include "parser.hpp"

int main()
{
    std::cin.tie(NULL);
    std::ios::sync_with_stdio(false);

    IntParser<int> parser;
    std::string line;
    while (std::getline(std::cin, line)) {
        parser.parse_infix(line);
        parser.infix_to_postfix();
        std::cout << "RESULT: " << parser.calculate() << '\n';
#ifndef NDEBUG
        std::cout << std::flush;
#endif
    }
    return 0;
}