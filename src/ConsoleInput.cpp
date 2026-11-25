#include "ConsoleInput.h"

#include <sstream>

bool readLine(
    const std::string& prompt,
    std::string& line,
    std::istream& input,
    std::ostream& output
) {
    output << prompt;

    if (!std::getline(input, line)) {
        return false;
    }

    if (line == ":cancel") {
        output << "Canceled. Returning to the main menu.\n";
        return false;
    }

    return true;
}

bool readInteger(
    const std::string& prompt,
    int& value,
    int minimum,
    std::istream& input,
    std::ostream& output
) {
    std::string line;

    while (readLine(prompt, line, input, output)) {
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

    return false;

}