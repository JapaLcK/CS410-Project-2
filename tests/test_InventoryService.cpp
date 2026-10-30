
#include <gtest/gtest.h>

#include "InventoryService.h"

TEST(InventoryServiceTest, SuccessfulDistributionUpdatesStock) {
    InventoryService service;
    service.addFoodItem(
        FoodItem(101, "Canned Soup", 10, "2026-12-31"));

    Distribution distribution(1, 2, "2026-10-27");
    distribution.addItem(101, 3);

    std::string error;
    EXPECT_TRUE(service.processDistribution(
        distribution, "2026-10-27", error));

    ASSERT_NE(service.getFoodItem(101), nullptr);
    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 7);
    EXPECT_EQ(service.getDistributionCount(), 1);
}

TEST(InventoryServiceTest, RejectsInsufficientStockWithoutChangingAnyStock) {
    InventoryService service;
    service.addFoodItem(
        FoodItem(101, "Canned Soup", 10, "2026-12-31"));
    service.addFoodItem(
        FoodItem(102, "Cereal", 3, "2026-12-31"));

    Distribution distribution(1, 2, "2026-10-27");
    distribution.addItem(101, 4);
    distribution.addItem(102, 5);

    std::string error;
    EXPECT_FALSE(service.processDistribution(
        distribution, "2026-10-27", error));

    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 10);
    EXPECT_EQ(service.getFoodItem(102)->getQuantityAvailable(), 3);
    EXPECT_EQ(service.getDistributionCount(), 0);
    EXPECT_EQ(error, "Insufficient inventory.");
}

TEST(InventoryServiceTest, RejectsExpiredFoodWithoutChangingStock) {
    InventoryService service;
    service.addFoodItem(
        FoodItem(101, "Canned Soup", 10, "2026-10-26"));
    service.addFoodItem(
        FoodItem(102, "Cereal", 8, "2026-12-31"));

    Distribution distribution(1, 2, "2026-10-27");
    distribution.addItem(101, 2);
    distribution.addItem(102, 2);

    std::string error;
    EXPECT_FALSE(service.processDistribution(
        distribution, "2026-10-27", error));

    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 10);
    EXPECT_EQ(service.getFoodItem(102)->getQuantityAvailable(), 8);
    EXPECT_EQ(service.getDistributionCount(), 0);
    EXPECT_EQ(error, "Food item has expired.");
}

TEST(InventoryServiceTest, RejectsUnknownFoodWithoutChangingStock) {
    InventoryService service;
    service.addFoodItem(
        FoodItem(101, "Canned Soup", 10, "2026-12-31"));

    Distribution distribution(1, 2, "2026-10-27");
    distribution.addItem(101, 2);
    distribution.addItem(999, 1);

    std::string error;
    EXPECT_FALSE(service.processDistribution(
        distribution, "2026-10-27", error));

    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 10);
    EXPECT_EQ(service.getDistributionCount(), 0);
    EXPECT_EQ(error, "Food item does not exist.");
}

TEST(InventoryServiceTest, AddsDonationToInventory) {
    InventoryService service;
    service.addFoodItem(
        FoodItem(101, "Canned Soup", 10, "2026-12-31"));

    EXPECT_TRUE(service.addDonation(101, 5));
    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 15);
}
