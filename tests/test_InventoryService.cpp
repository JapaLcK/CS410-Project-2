
#include <gtest/gtest.h>

#include <limits>

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

TEST(InventoryServiceTest, AddsFoodItemAndRejectsDuplicateId) {
    InventoryService service;
    // A new item can start at zero stock and have a leap-day expiration date.
    ASSERT_TRUE(service.addFoodItem(FoodItem(101, "Canned Soup", 0, "2028-02-29")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, "Cereal", 20, "2029-12-31")));

    // The rejected duplicate must leave the original item in place.
    const FoodItem* item = service.getFoodItem(101);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->getName(), "Canned Soup");
    EXPECT_EQ(item->getQuantityAvailable(), 0);
    EXPECT_EQ(item->getExpirationDate(), "2028-02-29");
}

TEST(InventoryServiceTest, RejectsInvalidFoodItems) {
    InventoryService service;

    // Try invalid IDs, quantities, names, and dates.
    EXPECT_FALSE(service.addFoodItem(FoodItem(0, "Soup", 10, "2028-12-31")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(-1, "Soup", 10, "2028-12-31")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, "Soup", -1, "2028-12-31")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, "", 10, "2028-12-31")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, " \t", 10, "2028-12-31")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, "Soup", 10, "2028-2-29")));
    EXPECT_FALSE(service.addFoodItem(FoodItem(101, "Soup", 10, "2027-02-29")));
    EXPECT_TRUE(service.getInventoryItems().empty());
}

TEST(InventoryServiceTest, RejectsInvalidDonationsWithoutChangingStock) {
    InventoryService service;
    ASSERT_TRUE(service.addFoodItem(FoodItem(101, "Canned Soup", 10, "2028-12-31")));

    // A donation must have a positive quantity and refer to an existing item.
    EXPECT_FALSE(service.addDonation(101, 0));
    EXPECT_FALSE(service.addDonation(101, -1));
    EXPECT_FALSE(service.addDonation(999, 5));

    EXPECT_EQ(service.getFoodItem(101)->getQuantityAvailable(), 10);
    EXPECT_EQ(service.getFoodItem(999), nullptr);

    // Stock can reach the largest int, but the next unit must be rejected.
    const int maxStock = std::numeric_limits<int>::max();
    ASSERT_TRUE(service.addFoodItem(FoodItem(102, "Cereal", maxStock - 1, "2028-12-31")));
    ASSERT_TRUE(service.addDonation(102, 1));
    EXPECT_FALSE(service.addDonation(102, 1));
    EXPECT_EQ(service.getFoodItem(102)->getQuantityAvailable(), maxStock);
}

TEST(InventoryServiceTest, ListsAllInventoryItemsInIdOrderWithCurrentStock) {
    InventoryService service;
    EXPECT_TRUE(service.getInventoryItems().empty());
    ASSERT_TRUE(service.addFoodItem(FoodItem(102, "Cereal", 10, "2028-12-31")));
    ASSERT_TRUE(service.addFoodItem(FoodItem(101, "Canned Soup", 0, "2028-12-31")));

    // Insertion order is 102, then 101. The list should start at 101.
    const auto beforeDonation = service.getInventoryItems();
    ASSERT_EQ(beforeDonation.size(), 2u);
    EXPECT_EQ(beforeDonation[0].getId(), 101);
    EXPECT_EQ(beforeDonation[0].getQuantityAvailable(), 0);
    EXPECT_EQ(beforeDonation[1].getId(), 102);

    // A fresh list shows the donation, and the earlier snapshot keeps its old value.
    ASSERT_TRUE(service.addDonation(101, 5));
    const auto afterDonation = service.getInventoryItems();
    ASSERT_EQ(afterDonation.size(), 2u);
    EXPECT_EQ(afterDonation[0].getId(), 101);
    EXPECT_EQ(afterDonation[0].getQuantityAvailable(), 5);
    EXPECT_EQ(afterDonation[1].getQuantityAvailable(), 10);
    EXPECT_EQ(beforeDonation[0].getQuantityAvailable(), 0);
}
