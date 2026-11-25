
#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <string>
#include <map>
#include <vector>

#include "FoodItem.h"
#include "Distribution.h"

class InventoryService {
private:
    // The map stores each item by ID and keeps the IDs sorted.
    std::map<int, FoodItem> inventory;
    std::vector<Distribution> distributions;

public:
    bool addFoodItem(const FoodItem& item);

    bool removeFoodItem(int foodItemId);

    bool addDonation(int foodItemId, int quantity);

    bool processDistribution(
        const Distribution& distribution,
        const std::string& currentDate,
        std::string& errorMessage
    );

    InventoryService() = default;
    explicit InventoryService(std::vector<Distribution> savedDistributions);

    const std::vector<Distribution>& getDistributions() const;


    const FoodItem* getFoodItem(int foodItemId) const;

    // Returns a snapshot of all items ordered by ID.
    std::vector<FoodItem> getInventoryItems() const;

    int getDistributionCount() const;
};

#endif
