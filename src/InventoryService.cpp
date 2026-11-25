
#include "InventoryService.h"
#include "DateUtils.h"
#include <utility>
#include <limits>

InventoryService::InventoryService(std::vector<Distribution> savedDistributions)
    : distributions(std::move(savedDistributions)) {}

const std::vector<Distribution>& InventoryService::getDistributions() const {
    return distributions;
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

bool InventoryService::removeFoodItem(int foodItemId) {
    return inventory.erase(foodItemId) != 0;
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


