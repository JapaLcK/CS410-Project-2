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

    // if food id does not exist, return 0
    if (items.find(foodItemId) == items.end()) {
        return 0;
    }
    // if exists, search foodItemId in map.
    return it->second; //search in attribute items defined in .h
}