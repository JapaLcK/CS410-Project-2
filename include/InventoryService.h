
#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "FoodItem.h"
#include "Distribution.h"

class InventoryService {
private:
    std::unordered_map<int, FoodItem> inventory;
    std::vector<Distribution> distributions;

public:
    bool addFoodItem(const FoodItem& item);

    bool addDonation(int foodItemId, int quantity);

    bool processDistribution(
        const Distribution& distribution,
        const std::string& currentDate,
        std::string& errorMessage
    );

    const FoodItem* getFoodItem(int foodItemId) const;

    int getDistributionCount() const;
};

#endif
