#ifndef PANTRY_DATABASE_H
#define PANTRY_DATABASE_H

#include <memory>
#include <sqlite3.h>

#include "InventoryService.h"

class PantryDatabase {
public:
    explicit PantryDatabase(const char* path);
    InventoryService load() const;
    void save(const InventoryService& service);

private:
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> database{
        nullptr, sqlite3_close
    };

    void execute(const char* sql) const;
};

#endif