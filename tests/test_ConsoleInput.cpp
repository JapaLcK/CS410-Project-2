#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "ConsoleInput.h"

TEST(ConsoleInputTest, AcceptsPositiveInteger) {
    std::istringstream input("12\n");
    std::ostringstream output;
    int value = 0;

    EXPECT_TRUE(readInteger("Quantity: ", value, 1, input, output));
    EXPECT_EQ(value, 12);
}

TEST(ConsoleInputTest, AcceptsZeroWhenMinimumIsZero) {
    std::istringstream input("0\n");
    std::ostringstream output;
    int value = -1;

    EXPECT_TRUE(readInteger("Food item ID: ", value, 0, input, output));
    EXPECT_EQ(value, 0);
}

TEST(ConsoleInputTest, RetriesInvalidInputsUntilValid) {
    std::istringstream input("abc\n2.5\n20abc\n-1\n0\n\n5\n");
    std::ostringstream output;
    int value = 0;

    EXPECT_TRUE(readInteger("Quantity: ", value, 1, input, output));
    EXPECT_EQ(value, 5);

    // Each invalid answer should produce a message and repeat the prompt.
    const std::string retry =
        "Please enter a whole number of at least 1. "
        "Please try again.\n";

    std::string expected = "Quantity: ";

    for (int i = 0; i < 6; ++i) {
        expected += retry + "Quantity: ";
    }

    EXPECT_EQ(output.str(), expected);
}

TEST(ConsoleInputTest, RejectsNumberTooLargeForIntAndRetries) {
    std::istringstream input("999999999999999999999999999999\n7\n");
    std::ostringstream output;
    int value = 0;

    EXPECT_TRUE(readInteger("Quantity: ", value, 1, input, output));
    EXPECT_EQ(value, 7);
    EXPECT_NE(output.str().find("Please try again."), std::string::npos);
}

TEST(ConsoleInputTest, ReturnsFalseWhenInputEnds) {
    std::istringstream input("");
    std::ostringstream output;
    int value = 42;

    EXPECT_FALSE(readInteger("Quantity: ", value, 1, input, output));
    EXPECT_EQ(value, 42);
}

TEST(ConsoleInputTest, ReturnsFalseWhenInputEndsAfterInvalidAnswer) {
    std::istringstream input("abc\n");
    std::ostringstream output;
    int value = 42;

    EXPECT_FALSE(readInteger("Quantity: ", value, 1, input, output));
    EXPECT_EQ(value, 42);
    EXPECT_NE(output.str().find("Please try again."), std::string::npos);
}