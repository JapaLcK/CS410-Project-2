#include "Distribution.h"

Distribution::Distribution(int householdId, int staffId, std::string date) 
{
    this->householdId = householdId;
    this->staffId = staffId;
    this->date = date;
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
    // if food id does not exist, return 0
    // if exists, search foodItemId in map.
}