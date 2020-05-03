#include <iomanip>
#include <iostream>
#include <string>

#include "parser.hpp"

int main()
{
    std::cin.tie(NULL);
    std::ios::sync_with_stdio(false);

    FloatParser<double> parser;
    std::string line;
    while (std::getline(std::cin, line)) {
        parser.parse_infix(line);
        parser.infix_to_postfix();
        std::cout << "RESULT: " << std::fixed << std::setprecision(6)
                  << parser.calculate() << '\n';
#ifndef NDEBUG
        std::cout << std::flush;
#endif
    }
    return 0;
}