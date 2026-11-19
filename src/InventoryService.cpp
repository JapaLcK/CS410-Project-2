
#include "InventoryService.h"

#include <limits>

namespace {

bool isValidCalendarDate(const std::string& date) {
    // YYYY-MM-DD has exactly 10 characters, with hyphens at positions 4 and 7.
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    // The two hyphens were checked above. Every other character must be a digit.
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9')) {
            return false;
        }
    }

    // Convert the three numeric fields so we can check the calendar date.
    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));

    // Reject year zero, months outside 1–12, and days below 1.
    if (year == 0 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    // Use the usual month lengths and add a day to February in leap years.
    const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // Century years are leap years only when divisible by 400.
    const bool leapYear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    const int extraDay = month == 2 && leapYear ? 1 : 0;

    // Months start at 1, while array indexes start at 0.
    return day <= daysPerMonth[month - 1] + extraDay;
}

}

bool InventoryService::addFoodItem(const FoodItem& item) {
    // Validate the item before inserting it into inventory.
    if (item.getId() <= 0 ||
        item.getQuantityAvailable() < 0 ||
        item.getName().find_first_not_of(" \t\n\r\f\v") == std::string::npos ||
        !isValidCalendarDate(item.getExpirationDate())) {
        return false;
    }

    // emplace leaves an existing item alone and reports whether it inserted the new one.
    return inventory.emplace(item.getId(), item).second;
}

bool InventoryService::addDonation(int foodItemId, int quantity) {
    // Find the item by ID and require a positive donation quantity.
    auto it = inventory.find(foodItemId);
    if (it == inventory.end() || quantity <= 0) {
        return false;
    }

    // Check how many more units fit in an int before adding any stock.
    FoodItem& item = it->second;
    const int remainingSpace = std::numeric_limits<int>::max() - item.getQuantityAvailable();
    if (quantity > remainingSpace) {
        return false;
    }

    // Update stock only after all checks pass.
    item.addQuantity(quantity);
    return true;
}

const FoodItem* InventoryService::getFoodItem(
    int foodItemId) const {

    auto it = inventory.find(foodItemId);

    if (it == inventory.end()) {
        return nullptr;
    }

    return &(it->second);
}

std::vector<FoodItem> InventoryService::getInventoryItems() const {
    // Copy each item in ID order so the menu gets a snapshot of current stock.
    std::vector<FoodItem> items;
    for (const auto& entry : inventory) {
        items.push_back(entry.second);
    }
    return items;
}

int InventoryService::getDistributionCount() const {
    return static_cast<int>(distributions.size());
}

bool InventoryService::processDistribution(
    const Distribution& distribution,
    const std::string& currentDate,
    std::string& errorMessage) {

    const auto& requestedItems = distribution.getItems();

    if (requestedItems.empty()) {
        errorMessage = "Distribution contains no items.";
        return false;
    }

    // Reject invalid dates before updating stock or recording the distribution.
    if (!isValidCalendarDate(distribution.getDate())) {
        errorMessage = "Distribution date must be a valid date in YYYY-MM-DD format.";
        return false;
    }

    
    for (const auto& request : requestedItems) {
        int foodItemId = request.first;
        int quantity = request.second;

        auto it = inventory.find(foodItemId);

        if (it == inventory.end()) {
            errorMessage = "Food item does not exist.";
            return false;
        }

        const FoodItem& food = it->second;

        if (quantity <= 0) {
            errorMessage = "Quantity must be positive.";
            return false;
        }

        if (food.getExpirationDate() < currentDate) {
            errorMessage = "Food item has expired.";
            return false;
        }

        if (food.getQuantityAvailable() < quantity) {
            errorMessage = "Insufficient inventory.";
            return false;
        }
    }

    
    for (const auto& request : requestedItems) {
        int foodItemId = request.first;
        int quantity = request.second;

        inventory.at(foodItemId).reduceQuantity(quantity);
    }

    // Records only successful distributions.
    distributions.push_back(distribution);

    errorMessage = "Distribution completed successfully.";
    return true;
}
