#include <ctime>
#include <iostream>
#include <string>
#include <limits>
#include <sstream>

#include "Distribution.h"
#include "FoodItem.h"
#include "InventoryService.h"
#include "DateUtils.h"
#include "ConsoleInput.h"

void displayInventory(const InventoryService& service) {
    const auto items = service.getInventoryItems();

    if(items.empty()) {
        std::cout << "Inventory is empty.\n";
        return;
    }

    std::cout << "\nCurrent inventory:\n";

    for (const FoodItem& item : items) {
        std::cout << item.getId() << " - "
                  << item.getName()
                  << "\nQuantity: " << item.getQuantityAvailable()
                  << "\nExpires: " << item.getExpirationDate()
                  << "\n";
    }
}

void recordDonation(InventoryService& service) {
    if (service.getInventoryItems().empty()) {
        std::cout << "Inventory is empty. Add a food item first.\n";
        return;
    }

    std::cout << "\nRecord Donation\n";

    int foodItemId;
    const FoodItem* item = nullptr;

    while (true) {
        if (!readInteger("\nFood item ID: ", foodItemId, 1)) {
            return;
        }

        item = service.getFoodItem(foodItemId);

        if (item == nullptr) {
            std::cout << "That food item ID does not exist. "
                         "Please try again.\n";
            continue;
        }

        if (item->getQuantityAvailable() ==
            std::numeric_limits<int>::max()) {
            std::cout << "That item's stock is already at the maximum. "
                         "Please choose another item.\n";
            continue;
        }

        break;
    }

    while (true) {
        int quantity;

        if (!readInteger("Donation quantity: ", quantity, 1)) {
            return;
        }

        int maximumDonation =
            std::numeric_limits<int>::max() -
            item->getQuantityAvailable();

        if (quantity > maximumDonation) {
            std::cout << "The maximum donation for this item is "
                      << maximumDonation << ". Please try again.\n";
            continue;
        }

        if (!service.addDonation(foodItemId, quantity)) {
            std::cout << "Could not record the donation. "
                         "Please try again.\n";
            continue;
        }

        std::cout << "Donation recorded successfully.\n";
        std::cout << "Updated stock: "
                  << item->getQuantityAvailable() << "\n";
        break;
    }
}

void registerDistribution(InventoryService& service){
    std::time_t now = std::time(nullptr);
    char dateBuffer[11];

    if (std::strftime(dateBuffer, sizeof(dateBuffer),
                      "%Y-%m-%d", std::localtime(&now)) == 0) {
        std::cerr << "Could not determine the current date.\n";
        return;
    }

    std::string currentDate = dateBuffer;
    int householdId;
    int staffId;
    std::string date;

    std::cout << "Food Pantry - Register Distribution\n\n";

    if (!readInteger("Household ID: ", householdId, 1)) {
        return;
    }

    if (!readInteger("Staff ID: ", staffId, 1)) {
        return;
    }

    while (true) {
        std::cout << "Distribution date (YYYY-MM-DD): ";

        if (!std::getline(std::cin, date)) {
        return;
    }

        if (!isValidCalendarDate(date)) {
            std::cout << "Enter a valid date in YYYY-MM-DD format. "
                        "Please try again.\n";
            continue;
        }

        break;
    }

    Distribution distribution(householdId, staffId, date);

    displayInventory(service);

    while (true) {
        int foodItemId;
        int quantity;

        if (!readInteger("\nFood item ID (0 to submit): ",
                        foodItemId, 0)) {
            return;
        }

        if (foodItemId == 0) {
            if (distribution.getItems().empty()) {
                std::cout << "Add at least one item before submitting. "
                            "Please try again.\n";
                continue;
            }

            break;
        }

        const FoodItem* food = service.getFoodItem(foodItemId);

        if (food == nullptr) {
            std::cout << "That food item ID does not exist. "
                        "Please try again.\n";
            continue;
        }

        if (food->getExpirationDate() < currentDate) {
            std::cout << "That food item has expired. "
                        "Please try another food item ID.\n";
            continue;
        }

        // Include any quantity already requested for this item.
        int alreadyRequested = 0;
        const auto& requestedItems = distribution.getItems();
        auto it = requestedItems.find(foodItemId);

        if (it != requestedItems.end()) {
            alreadyRequested = it->second;
        }

        int remaining = food->getQuantityAvailable() - alreadyRequested;

        if (remaining <= 0) {
            std::cout << "No more stock is available for this request. "
                        "Please try another food item ID.\n";
            continue;
        }

        while (true) {
            if (!readInteger("Quantity: ", quantity, 1)) {
                return;
            }

            if (quantity > remaining) {
                std::cout << "Only " << remaining
                        << " units are available. Please try again.\n";
                continue;
            }

            if (!distribution.addItem(foodItemId, quantity)) {
                std::cout << "Could not add that quantity. "
                            "Please try again.\n";
                continue;
            }

            break;
        }
    }

    std::string message;

    if (service.processDistribution(distribution, currentDate, message)) {
        std::cout << "\nDistribution recorded successfully.\n";
    } else {
        std::cout << "\nDistribution rejected: " << message << "\n";
    }

    displayInventory(service);

    std::cout << "Recorded distributions: "
              << service.getDistributionCount() << "\n";       
}

void AddFoodItem(InventoryService& service) {
    int id;
    int quantity;
    std::string name;
    std::string expirationDate;

    std::cout << "\nAdd Food Item\n\n";

    while (true) {
        if (!readInteger("Food item ID: ", id, 1)) {
            return;
        }

        if (service.getFoodItem(id) != nullptr) {
            std::cout << "That food item ID already exists. "
                        "Please try again.\n";
            continue;
        }

        break;
    }

    while (true) {
        std::cout << "Food name: ";

        if (!std::getline(std::cin, name)) {
            return;
        }

        if (name.find_first_not_of(" \t\r\n\f\v") ==
            std::string::npos) {
            std::cout << "Food name cannot be blank. Please try again.\n";
            continue;
        }

        break;
    }

    if (!readInteger("Starting quantity: ", quantity, 1)) {
        return;
    }

    // Today's date.
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char dateBuffer[11];

    if (localTime == nullptr ||
        std::strftime(dateBuffer, sizeof(dateBuffer),
                    "%Y-%m-%d", localTime) == 0) {
        std::cerr << "Could not determine the current date.\n";
        return;
    }

    std::string currentDate = dateBuffer;

    while (true) {
    std::cout << "Expiration date (YYYY-MM-DD): ";

    if (!std::getline(std::cin, expirationDate)) {
        return;
    }

    if (!isValidCalendarDate(expirationDate)) {
        std::cout << "Enter a valid date in YYYY-MM-DD format. "
                 "Please try again.\n";
        continue;
    }   

    if (expirationDate < currentDate) {
        std::cout << "Cannot add food with an expiration date "
                     "in the past. Please try again.\n";
        continue;
    }

    break;

    }

    FoodItem item(id, name, quantity, expirationDate);

    if (service.addFoodItem(item)) {
        std::cout << "Food item added successfully.\n";
    } else {
        std::cout << "Could not add the item. Check for a duplicate ID, "
                     "a blank name, or an invalid expiration date.\n";
    }
}

int main() {
    InventoryService service;

    // Example inventory for the demonstration.
    service.addFoodItem(FoodItem(1, "Rice", 50, "2027-12-31"));
    service.addFoodItem(FoodItem(2, "Pasta", 30, "2027-12-31"));
    service.addFoodItem(FoodItem(3, "Beans", 20, "2027-12-31"));

    while (true) {
        std::cout << "\nFood Pantry Menu\n\n";
        std::cout << "1. View inventory\n";
        std::cout << "2. Add food item\n";
        std::cout << "3. Record donation\n";
        std::cout << "4. Register distribution\n";
        std::cout << "5. Exit\n";

        int choice;

        while (true) {
            if (!readInteger("\nChoose an option: ", choice, 1)) {
                return 0;
            }

            if (choice >= 1 && choice <= 5) {
                break;
            }

            std::cout << "Please choose 1, 2, 3, 4 or 5. Please try again.\n";
        }

        if (choice == 5) {
            break;
        }

        switch (choice) {
            case 1:
                displayInventory(service);
                break;

            case 2:
                AddFoodItem(service);
                break;

            case 3:
                recordDonation(service);
                break;    

            case 4:
                registerDistribution(service);
                break;

            default:
                std::cout << "Please choose 1, 2, 3, 4 or 5.\n";
                break;
        }

        if (!std::cin) {
            break;
        }
    }

    return 0;
}