#include "Distribution.h"

Distribution::Distribution(int householdId, int staffId, std::string date) 
{
    // "this->x" is the member variable stored in the object; plain "x" is the parameter.
    this->householdId = householdId; // save the household ID in this object
    this->staffId = staffId;         // save the staff member ID in this object
    this->date = date;               // save the distribution date in this object
}

bool Distribution::addItem(int foodItemId, int quantity) {
    if (quantity <= 0) {
        return false;
    }else{
        items[foodItemId] += quantity;
        return true;
    }
}

int Distribution::getHouseholdId() const {
    return householdId;
}

int Distribution::getStaffId() const {
    return staffId;
}

std::string Distribution::getDate() const {
    return date;
}

int Distribution::getItemQuantity(int foodItemId) const {
    auto it = items.find(foodItemId);

    if (it == items.end()) {
        return 0;
    }

    return it->second;
}

const std::unordered_map<int, int>& Distribution::getItems() const {
    return items;
}