#ifndef CONSOLE_INPUT_H
#define CONSOLE_INPUT_H

#include <iostream>
#include <string>

bool readInteger(
    const std::string& prompt,
    int& value,
    int minimum,
    std::istream& input = std::cin,
    std::ostream& output = std::cout
);

#endif