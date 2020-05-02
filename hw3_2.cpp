#include <iostream>
#include <string>

#include "parser.hpp"

int main()
{
    FloatParser<double> parser;
    std::string line;
    while (std::getline(std::cin, line)) {
        parser.parse_infix(line);
        parser.infix_to_postfix();
        std::cout << "RESULT: " << parser.calculate() << '\n';
    }
    return 0;
}