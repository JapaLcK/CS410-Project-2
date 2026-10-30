
#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include <string>

class FoodItem {
private:
    int id;
    std::string name;
    int quantityAvailable;
    std::string expirationDate;

public:
    FoodItem(int id, std::string name, int quantityAvailable,
             std::string expirationDate);

    int getId() const;
    std::string getName() const;
    int getQuantityAvailable() const;
    std::string getExpirationDate() const;

    bool reduceQuantity(int quantity);
    void addQuantity(int quantity);
};

#endif
