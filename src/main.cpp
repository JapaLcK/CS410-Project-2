#include <ctime>
#include <iostream>
#include <string>

#include "Distribution.h"
#include "FoodItem.h"
#include "InventoryService.h"

int main() {
    InventoryService service;

    // Example inventory for the demonstration.
    service.addFoodItem(FoodItem(1, "Rice", 50, "2027-12-31"));
    service.addFoodItem(FoodItem(2, "Pasta", 30, "2027-12-31"));
    service.addFoodItem(FoodItem(3, "Beans", 20, "2027-12-31"));

    std::time_t now = std::time(nullptr);
    char dateBuffer[11];

    if (std::strftime(dateBuffer, sizeof(dateBuffer),
                      "%Y-%m-%d", std::localtime(&now)) == 0) {
        std::cerr << "Could not determine the current date.\n";
        return 1;
    }

    std::string currentDate = dateBuffer;
    int householdId;
    int staffId;
    std::string date;

    std::cout << "Food Pantry - Register Distribution\n\n";
    std::cout << "Household ID: ";

    if (!(std::cin >> householdId) || householdId <= 0) {
        std::cout << "Invalid household ID.\n";
        return 1;
    }

    std::cout << "Staff ID: ";

    if (!(std::cin >> staffId) || staffId <= 0) {
        std::cout << "Invalid staff ID.\n";
        return 1;
    }

    std::cout << "Distribution date (YYYY-MM-DD): ";

    if (!(std::cin >> date)) {
        std::cout << "Invalid date input.\n";
        return 1;
    }

    Distribution distribution(householdId, staffId, date);

    std::cout << "\nAvailable food items:\n";
    std::cout << "1 - Rice: 50\n";
    std::cout << "2 - Pasta: 30\n";
    std::cout << "3 - Beans: 20\n";

    while (true) {
        int foodItemId;
        int quantity;

        std::cout << "\nFood item ID (0 to submit): ";

        if (!(std::cin >> foodItemId)) {
            std::cout << "Invalid food item ID.\n";
            return 1;
        }

        if (foodItemId == 0) {
            break;
        }

        std::cout << "Quantity: ";

        if (!(std::cin >> quantity)) {
            std::cout << "Invalid quantity input.\n";
            return 1;
        }

        if (!distribution.addItem(foodItemId, quantity)) {
            std::cout << "Request cancelled: quantity must be positive.\n";
            return 1;
        }
    }

    std::string message;

    if (service.processDistribution(distribution, currentDate, message)) {
        std::cout << "\nDistribution recorded successfully.\n";
    } else {
        std::cout << "\nDistribution rejected: " << message << "\n";
    }

    std::cout << "\nRemaining inventory:\n";

    for (int id = 1; id <= 3; id++) {
        const FoodItem* item = service.getFoodItem(id);

        if (item != nullptr) {
            std::cout << item->getName() << ": "
                      << item->getQuantityAvailable() << "\n";
        }
    }

    std::cout << "Recorded distributions: "
              << service.getDistributionCount() << "\n";

    return 0;
}