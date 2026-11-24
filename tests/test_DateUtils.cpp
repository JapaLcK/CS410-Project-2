#include <gtest/gtest.h>

#include "DateUtils.h"

TEST(DateUtilsTest, AcceptsValidCalendarDates) {
    EXPECT_TRUE(isValidCalendarDate("2028-01-31"));
    EXPECT_TRUE(isValidCalendarDate("2028-04-30"));
    EXPECT_TRUE(isValidCalendarDate("2028-12-31"));
}

TEST(DateUtilsTest, RejectsInvalidFormats) {
    EXPECT_FALSE(isValidCalendarDate(""));
    EXPECT_FALSE(isValidCalendarDate("294-42-1"));
    EXPECT_FALSE(isValidCalendarDate("2028-2-03"));
    EXPECT_FALSE(isValidCalendarDate("2028/02/03"));
    EXPECT_FALSE(isValidCalendarDate("202a-02-03"));
    EXPECT_FALSE(isValidCalendarDate("2028-02-03extra"));
}

TEST(DateUtilsTest, RejectsInvalidYearMonthAndDay) {
    EXPECT_FALSE(isValidCalendarDate("0000-01-01"));
    EXPECT_FALSE(isValidCalendarDate("2028-00-01"));
    EXPECT_FALSE(isValidCalendarDate("2028-13-01"));
    EXPECT_FALSE(isValidCalendarDate("2028-01-00"));
    EXPECT_FALSE(isValidCalendarDate("2028-01-32"));
    EXPECT_FALSE(isValidCalendarDate("2028-04-31"));
    EXPECT_FALSE(isValidCalendarDate("2028-02-30"));
}

TEST(DateUtilsTest, HandlesLeapYears) {
    EXPECT_TRUE(isValidCalendarDate("2028-02-29"));
    EXPECT_FALSE(isValidCalendarDate("2027-02-29"));

    // Century years are leap years only when divisible by 400.
    EXPECT_TRUE(isValidCalendarDate("2000-02-29"));
    EXPECT_FALSE(isValidCalendarDate("1900-02-29"));
}

TEST(DateUtilsTest, AcceptsValidPastDate) {
    EXPECT_TRUE(isValidCalendarDate("2025-12-13"));
}