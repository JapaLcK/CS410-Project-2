
#include "FoodItem.h"

FoodItem::FoodItem(int id, std::string name, int quantityAvailable,
                   std::string expirationDate) {
    this->id = id;
    this->name = name;
    this->quantityAvailable = quantityAvailable;
    this->expirationDate = expirationDate;
}

int FoodItem::getId() const {
    return id;
}

std::string FoodItem::getName() const {
    return name;
}

int FoodItem::getQuantityAvailable() const {
    return quantityAvailable;
}

std::string FoodItem::getExpirationDate() const {
    return expirationDate;
}

bool FoodItem::reduceQuantity(int quantity) {
    if (quantity <= 0 || quantity > quantityAvailable) {
        return false;
    }

    quantityAvailable -= quantity;
    return true;
}

void FoodItem::addQuantity(int quantity) {
    if (quantity > 0) {
        quantityAvailable += quantity;
    }
}
