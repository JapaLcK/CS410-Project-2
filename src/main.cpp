#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>

#include "ConsoleInput.h"
#include "DateUtils.h"
#include "Distribution.h"
#include "FoodItem.h"
#include "InventoryService.h"
#include <exception>
#include "PantryDatabase.h"



std::string getCurrentDate() {
    const std::time_t now = std::time(nullptr);
    const std::tm* localTime = std::localtime(&now);
    char dateBuffer[11];

    if (localTime == nullptr ||
        std::strftime(dateBuffer, sizeof(dateBuffer),
                      "%Y-%m-%d", localTime) == 0) {
        std::cerr << "Could not determine the current date.\n";
        return {};
    }

    return dateBuffer;
}

void displayInventory(const InventoryService& service) {
    const auto items = service.getInventoryItems();

    if (items.empty()) {
        std::cout << "Inventory is empty.\n";
        return;
    }

    std::cout << "\nCurrent inventory:\n";

    for (const FoodItem& item : items) {
        std::cout << item.getId() << " - " << item.getName()
                  << "\nQuantity: " << item.getQuantityAvailable()
                  << "\nExpires: " << item.getExpirationDate() << '\n';
    }
}

void recordDonation(InventoryService& service) {
    if (service.getInventoryItems().empty()) {
        std::cout << "Inventory is empty. Add a food item first.\n";
        return;
    }

    std::cout << "\nRecord Donation\n";

    int foodItemId;
    const FoodItem* item;

    while (true) {
        if (!readInteger("\nFood item ID: ", foodItemId, 1)) {
            return;
        }

        item = service.getFoodItem(foodItemId);

        if (item == nullptr) {
            std::cout << "That food item ID does not exist. Please try again.\n";
            continue;
        }

        if (item->getQuantityAvailable() == std::numeric_limits<int>::max()) {
            std::cout << "That item's stock is already at the maximum. "
                         "Please choose another item.\n";
            continue;
        }

        break;
    }

    const int maximumDonation =
        std::numeric_limits<int>::max() - item->getQuantityAvailable();

    int quantity;
    while (readInteger("Donation quantity: ", quantity, 1)) {
        if (quantity > maximumDonation) {
            std::cout << "The maximum donation for this item is "
                      << maximumDonation << ". Please try again.\n";
            continue;
        }

        if (!service.addDonation(foodItemId, quantity)) {
            std::cout << "Could not record the donation. Please try again.\n";
            continue;
        }

        std::cout << "Donation recorded successfully.\n"
                  << "Updated stock: " << item->getQuantityAvailable() << '\n';
        return;
    }
}

void registerDistribution(InventoryService& service) {
    const std::string currentDate = getCurrentDate();
    if (currentDate.empty()) {
        return;
    }

    const auto items = service.getInventoryItems();
    const bool hasAvailableFood = std::any_of(
        items.begin(), items.end(), [&currentDate](const FoodItem& item) {
            return item.getQuantityAvailable() > 0 &&
                   item.getExpirationDate() >= currentDate;
        });

    if (!hasAvailableFood) {
        std::cout << "No unexpired food is in stock for distribution.\n";
        return;
    }

    std::cout << "Food Pantry - Register Distribution\n\n";

    int householdId;
    int staffId;

    if (!readInteger("Household ID: ", householdId, 1) ||
        !readInteger("Staff ID: ", staffId, 1)) {
        return;
    }

    std::string date;
    while (true) {
        if (!readLine("Distribution date (YYYY-MM-DD): ", date)) {
            return;
        }

        if (isValidCalendarDate(date)) {
            break;
        }

        std::cout << "Enter a valid date in YYYY-MM-DD format. "
                     "Please try again.\n";
    }

    Distribution distribution(householdId, staffId, date);
    displayInventory(service);

    while (true) {
        int foodItemId;

        if (!readInteger("\nFood item ID (0 to submit): ", foodItemId, 0)) {
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
            std::cout << "That food item ID does not exist. Please try again.\n";
            continue;
        }

        if (food->getExpirationDate() < currentDate) {
            std::cout << "That food item has expired. "
                         "Please try another food item ID.\n";
            continue;
        }

        const int remaining = food->getQuantityAvailable() -
                              distribution.getItemQuantity(foodItemId);

        if (remaining <= 0) {
            std::cout << "No more stock is available for this request. "
                         "Please try another food item ID.\n";
            continue;
        }

        while (true) {
            int quantity;

            if (!readInteger("Quantity: ", quantity, 1)) {
                return;
            }

            if (quantity > remaining) {
                std::cout << "Only " << remaining
                          << " units are available. Please try again.\n";
                continue;
            }

            if (!distribution.addItem(foodItemId, quantity)) {
                std::cout << "Could not add that quantity. Please try again.\n";
                continue;
            }

            break;
        }
    }

    std::string message;

    if (service.processDistribution(distribution, currentDate, message)) {
        std::cout << "\nDistribution recorded successfully.\n";
    } else {
        std::cout << "\nDistribution rejected: " << message << '\n';
    }

    displayInventory(service);
    std::cout << "Recorded distributions: "
              << service.getDistributionCount() << '\n';
}

void addFoodItem(InventoryService& service) {
    std::cout << "\nAdd Food Item\n\n";

    int id;
    while (true) {
        if (!readInteger("Food item ID: ", id, 1)) {
            return;
        }

        if (service.getFoodItem(id) == nullptr) {
            break;
        }

        std::cout << "That food item ID already exists. Please try again.\n";
    }

    std::string name;
    while (true) {
        if (!readLine("Food name: ", name)) {
            return;
        }

        if (name.find_first_not_of(" \t\r\n\f\v") != std::string::npos) {
            break;
        }

        std::cout << "Food name cannot be blank. Please try again.\n";
    }

    int quantity;
    if (!readInteger("Starting quantity: ", quantity, 1)) {
        return;
    }

    const std::string currentDate = getCurrentDate();
    if (currentDate.empty()) {
        return;
    }

    std::string expirationDate;
    while (true) {
        if (!readLine("Expiration date (YYYY-MM-DD): ", expirationDate)) {
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

    if (service.addFoodItem(FoodItem(id, name, quantity, expirationDate))) {
        std::cout << "Food item added successfully.\n";
    } else {
        std::cout << "Could not add the item. Check for a duplicate ID, "
                     "a blank name, or an invalid expiration date.\n";
    }
}

void removeFoodItem(InventoryService& service) {
    if (service.getInventoryItems().empty()) {
        std::cout << "Inventory is empty.\n";
        return;
    }

    displayInventory(service);

    int id;
    const FoodItem* item = nullptr;

    while (item == nullptr) {
        if (!readInteger("Food item ID to remove: ", id, 1)) {
            return;
        }

        item = service.getFoodItem(id);
        if (item == nullptr) {
            std::cout << "That food item ID does not exist. Please try again.\n";
        }
    }

    const std::string prompt =
        "Remove " + item->getName() + " and all its remaining stock? (y/n): ";

    std::string answer;
    while (readLine(prompt, answer)) {
        if (answer == "y" || answer == "Y") {
            service.removeFoodItem(id);
            std::cout << "Food item removed successfully.\n";
            return;
        }

        if (answer == "n" || answer == "N") {
            std::cout << "Removal canceled.\n";
            return;
        }

        std::cout << "Please enter y or n.\n";
    }
}

void runMenu(InventoryService& service) {
    while (true) {
        std::cout << "\nFood Pantry Menu\n\n"
                  << "1. View inventory\n"
                  << "2. Add food item\n"
                  << "3. Record donation\n"
                  << "4. Register distribution\n"
                  << "5. Exit\n"
                  << "6. Remove food item\n"
                  << "Type :cancel at any prompt to return to the main menu.\n";

        int choice;
        if (!readInteger("\nChoose an option: ", choice, 1)) {
            if (!std::cin) {
                return;
            }

            continue;
        }

        switch (choice) {
            case 1:
                displayInventory(service);
                break;
            case 2:
                addFoodItem(service);
                break;
            case 3:
                recordDonation(service);
                break;
            case 4:
                registerDistribution(service);
                break;
            case 5:
                return;
            case 6:
                removeFoodItem(service);
                break;
            default:
                std::cout << "Please choose an option from 1 to 6.\n";
                break;
        }

        if (!std::cin) {
            return;
        }
    }
}

int main() {
    try {
        PantryDatabase database("food-pantry.sqlite3");
        InventoryService service = database.load();

        runMenu(service);

        database.save(service);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Pantry database error: " << error.what() << '\n';
        return 1;
    }
}