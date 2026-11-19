#include <ctime>
#include <iostream>
#include <string>
#include <limits>

#include "Distribution.h"
#include "FoodItem.h"
#include "InventoryService.h"

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
    std::cout << "Household ID: ";

    if (!(std::cin >> householdId) || householdId <= 0) {
        std::cout << "Invalid household ID.\n";
        return;
    }

    std::cout << "Staff ID: ";

    if (!(std::cin >> staffId) || staffId <= 0) {
        std::cout << "Invalid staff ID.\n";
        return;
    }

    std::cout << "Distribution date (YYYY-MM-DD): ";

    if (!(std::cin >> date)) {
        std::cout << "Invalid date input.\n";
        return;
    }

    Distribution distribution(householdId, staffId, date);

    displayInventory(service);

    while (true) {
        int foodItemId;
        int quantity;

        std::cout << "\nFood item ID (0 to submit): ";

        if (!(std::cin >> foodItemId)) {
            std::cout << "Invalid food item ID.\n";
            return;
        }

        if (foodItemId == 0) {
            break;
        }

        std::cout << "Quantity: ";

        if (!(std::cin >> quantity)) {
            std::cout << "Invalid quantity input.\n";
            return;
        }

        if (!distribution.addItem(foodItemId, quantity)) {
            std::cout << "Request cancelled: quantity must be positive.\n";
            return;
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

    std::cout << "Food item ID: ";
    if (!(std::cin >> id) || id <= 0) {
        std::cout << "Invalid ID. Please enter a positive number.\n";
        return;
    }

    // Remove the newline left by reading the ID.
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(), '\n'
    );

    std::cout << "Food name: ";
    std::getline(std::cin, name);

    std::cout << "Starting quantity: ";
    if (!(std::cin >> quantity) || quantity <= 0) {
        std::cout << "Invalid quantity. Please enter one or more.\n";
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

    if (!(std::cin >> expirationDate)) {
        if (std::cin.eof() || std::cin.bad()) {
            return;
        }

        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );

        std::cout << "Invalid date input. Please try again.\n";
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
        std::cout << "4. Register distribution\n";
        std::cout << "5. Exit\n";
        std::cout << "\nChoose an option: ";

        int choice;

        if (!(std::cin >> choice)) {
            // Exit if input has ended.
            if (std::cin.eof() || std::cin.bad()) {
                break;
            }

            // Deletes the error and invalid input.
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );

            std::cout << "Please enter a menu number.\n";
            continue;
        }

        // Deletes the rest of the input line.
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );

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

            case 4:
                registerDistribution(service);
                break;

            default:
                std::cout << "Please choose 1, 2, 4 or 5.\n";
                break;
        }

        // Recovers if the action encountered invalid input.
        if (std::cin.eof() || std::cin.bad()) {
            break;
        }

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
        }
    }

    return 0;
}