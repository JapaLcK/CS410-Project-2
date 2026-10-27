#include <gtest/gtest.h>
#include "Distribution.h"
TEST(DistributionTest, AddsValidItem) {
    Distribution distribution(1, 2, "2026-10-27");
    

    EXPECT_TRUE(distribution.addItem(101, 3));
    EXPECT_EQ(distribution.getItemQuantity(101), 3);
    
}

TEST(DistributionTest, RejectsZeroQuantity)
{
    Distribution distribution(1, 2, "2026-10-27");

    EXPECT_FALSE(distribution.addItem(102, 0));
    EXPECT_EQ(distribution.getItemQuantity(102), 0);
}

TEST(DistributionTest, AccumulatesRepeatedItem) {
    Distribution distribution(1, 2, "2026-10-27");

    EXPECT_TRUE(distribution.addItem(102, 2));
    EXPECT_TRUE(distribution.addItem(102, 3));
    EXPECT_EQ(distribution.getItemQuantity(102), 5);
}

TEST(DistributionTest, RejectsNegativeQuanity) {
    Distribution distribution(1, 2, "2026-10-27");

    EXPECT_TRUE(distribution.addItem(103, 3));
    EXPECT_FALSE(distribution.addItem(103, -1));
    EXPECT_EQ(distribution.getItemQuantity(103), 3);
}