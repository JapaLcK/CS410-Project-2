
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

TEST(InventoryServiceTest, RejectsInvalidDistributionDatesWithoutChangingStock) {
    // Cover an impossible day, a missing leading zero, and a non-leap century year.
    for (const char* date : {"2026-02-30", "2026-2-03", "1900-02-29"}) {
        // Show which input caused the failure if an assertion fails.
        SCOPED_TRACE(date);

        // Start each case with 10 units of unexpired rice.
        InventoryService service;
        ASSERT_TRUE(service.addFoodItem(FoodItem(1, "Rice", 10, "2029-12-31")));

        // Request 3 units so the date is the only invalid part of the request.
        Distribution distribution(1, 2, date);
        ASSERT_TRUE(distribution.addItem(1, 3));

        // Verify that the service rejects the request and explains why.
        std::string message;
        EXPECT_FALSE(service.processDistribution(distribution, "2028-03-01", message));
        EXPECT_EQ(message, "Distribution date must be a valid date in YYYY-MM-DD format.");

        // A rejected request must leave the stock and distribution records intact.
        EXPECT_EQ(service.getFoodItem(1)->getQuantityAvailable(), 10);
        EXPECT_EQ(service.getDistributionCount(), 0);
    }
}

TEST(InventoryServiceTest, AcceptsDistributionOnLeapDay) {
    // Start with 10 units of unexpired rice.
    InventoryService service;
    ASSERT_TRUE(service.addFoodItem(FoodItem(1, "Rice", 10, "2029-12-31")));

    // February 29 exists in 2028. Request 3 units on that date.
    Distribution distribution(1, 2, "2028-02-29");
    ASSERT_TRUE(distribution.addItem(1, 3));

    // Verify that the request succeeds, deducts the stock, and creates one record.
    std::string message;
    EXPECT_TRUE(service.processDistribution(distribution, "2028-03-01", message));
    EXPECT_EQ(service.getFoodItem(1)->getQuantityAvailable(), 7);
    EXPECT_EQ(service.getDistributionCount(), 1);
}
