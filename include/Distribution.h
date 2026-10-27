#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H
#include <string>
#include <unordered_map>
// family received such foods, in such quantity,
// on a certain date, by an staff member.

class Distribution
{
    private:
        int householdId;
        int staffId;
        std::string date;
        std::unordered_map<int, int> items;
    
    public:
    Distribution(int householdId, int staffId, std::string date);
    bool addItem(int foodItemId, int quantity); 
    int getHouseholdId() const;
    int getStaffId() const;
    std::string getDate() const; 
    int getItemQuantity(int foodItemId) const:
     
    
    

};

#endif
