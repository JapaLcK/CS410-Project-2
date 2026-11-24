#include "ConsoleInput.h"

#include <sstream>

bool readInteger(
    const std::string& prompt,
    int& value,
    int minimum,
    std::istream& input,
    std::ostream& output
) {
    std::string line;

    while (true) {
        output << prompt;

        if (!std::getline(input, line)) {
            return false; // Input ended or could not be read.
        }

        std::istringstream stream(line);
        int number;
        char extra;

        if ((stream >> number) &&
            !(stream >> extra) &&
            number >= minimum) {
            value = number;
            return true;
        }

        output << "Please enter a whole number of at least "
               << minimum << ". Please try again.\n";
    }
}