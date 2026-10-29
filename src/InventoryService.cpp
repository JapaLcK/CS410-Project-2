
#include "InventoryService.h"

bool InventoryService::addFoodItem(const FoodItem& item) {
    if (item.getId() <= 0 ||
        item.getQuantityAvailable() < 0) {
        return false;
    }

    auto result = inventory.emplace(item.getId(), item);
    return result.second;
}

bool InventoryService::addDonation(int foodItemId, int quantity) {
    auto it = inventory.find(foodItemId);

    if (it == inventory.end() || quantity <= 0) {
        return false;
    }

    it->second.addQuantity(quantity);
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
