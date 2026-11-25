#include "PantryDatabase.h"

#include <stdexcept>
#include <utility>

namespace {

class Statement {
public:
    Statement(sqlite3* database, const char* sql) : database(database) {
        sqlite3_stmt* prepared = nullptr;
        const int result = sqlite3_prepare_v2(database, sql, -1, &prepared, nullptr);
        statement.reset(prepared);
        check(result);
    }

    bool next() {
        const int result = sqlite3_step(statement.get());
        if (result == SQLITE_ROW) {
            return true;
        }
        if (result != SQLITE_DONE) {
            check(result);
        }
        return false;
    }

    void bind(int index, sqlite3_int64 value) {
        check(sqlite3_bind_int64(statement.get(), index, value));
    }

    void bind(int index, const std::string& value) {
        check(sqlite3_bind_text64(
            statement.get(), index, value.data(), value.size(),
            SQLITE_TRANSIENT, SQLITE_UTF8
        ));
    }

    void insert() {
        next();
        check(sqlite3_reset(statement.get()));
    }

    int integer(int column) const {
        return sqlite3_column_int(statement.get(), column);
    }

    sqlite3_int64 rowId(int column) const {
        return sqlite3_column_int64(statement.get(), column);
    }

    std::string text(int column) const {
        const auto* value = sqlite3_column_text(statement.get(), column);
        if (value == nullptr) {
            throw std::runtime_error("Missing text in the pantry database.");
        }
        return std::string(
            reinterpret_cast<const char*>(value),
            sqlite3_column_bytes(statement.get(), column)
        );
    }

private:
    sqlite3* database;
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> statement{
        nullptr, sqlite3_finalize
    };

    void check(int result) const {
        if (result != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(database));
        }
    }
};

} // namespace

PantryDatabase::PantryDatabase(const char* path) {
    sqlite3* handle = nullptr;
    const int result = sqlite3_open(path, &handle);
    database.reset(handle);

    if (result != SQLITE_OK) {
        throw std::runtime_error(
            handle ? sqlite3_errmsg(handle) : sqlite3_errstr(result)
        );
    }

    execute("PRAGMA foreign_keys = ON");
    execute("BEGIN IMMEDIATE");

    Statement version(database.get(), "PRAGMA user_version");
    if (version.next() && version.integer(0) > 1) {
        throw std::runtime_error("The pantry database was created by a newer version.");
    }

    execute(R"sql(
        CREATE TABLE IF NOT EXISTS inventory (
            id INTEGER PRIMARY KEY CHECK (id > 0),
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL CHECK (quantity >= 0),
            expiration_date TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS distributions (
            id INTEGER PRIMARY KEY,
            household_id INTEGER NOT NULL,
            staff_id INTEGER NOT NULL,
            date TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS distribution_items (
            distribution_id INTEGER NOT NULL REFERENCES distributions(id),
            food_item_id INTEGER NOT NULL,
            quantity INTEGER NOT NULL CHECK (quantity > 0),
            PRIMARY KEY (distribution_id, food_item_id)
        );
    )sql");
}

InventoryService PantryDatabase::load() const {
    Statement history(database.get(), R"sql(
        SELECT d.id, d.household_id, d.staff_id, d.date, i.food_item_id, i.quantity
        FROM distributions d
        JOIN distribution_items i ON i.distribution_id = d.id
        ORDER BY d.id, i.food_item_id
    )sql");

    std::vector<Distribution> distributions;
    sqlite3_int64 lastId = 0;

    while (history.next()) {
        const auto id = history.rowId(0);

        if (distributions.empty() || id != lastId) {
            distributions.emplace_back(
                history.integer(1), history.integer(2), history.text(3)
            );
            lastId = id;
        }

        distributions.back().addItem(history.integer(4), history.integer(5));
    }

    InventoryService service(std::move(distributions));
    Statement stock(database.get(),
                    "SELECT id, name, quantity, expiration_date FROM inventory");

    while (stock.next()) {
        FoodItem item(stock.integer(0), stock.text(1),
                      stock.integer(2), stock.text(3));

        if (!service.addFoodItem(item)) {
            throw std::runtime_error("Invalid food item in the pantry database.");
        }
    }

    Statement version(database.get(), "PRAGMA user_version");
    if (version.next() && version.integer(0) == 0) {
        service.addFoodItem(FoodItem(1, "Rice", 50, "2027-12-31"));
        service.addFoodItem(FoodItem(2, "Pasta", 30, "2027-12-31"));
        service.addFoodItem(FoodItem(3, "Beans", 20, "2027-12-31"));
    }

    return service;
}

void PantryDatabase::save(const InventoryService& service) {
    execute("DELETE FROM distribution_items; DELETE FROM distributions; DELETE FROM inventory;");

    Statement stock(database.get(), R"sql(
        INSERT INTO inventory (id, name, quantity, expiration_date)
        VALUES (?, ?, ?, ?)
    )sql");

    for (const auto& item : service.getInventoryItems()) {
        stock.bind(1, item.getId());
        stock.bind(2, item.getName());
        stock.bind(3, item.getQuantityAvailable());
        stock.bind(4, item.getExpirationDate());
        stock.insert();
    }

    Statement history(database.get(), R"sql(
        INSERT INTO distributions (id, household_id, staff_id, date)
        VALUES (?, ?, ?, ?)
    )sql");

    Statement details(database.get(), R"sql(
        INSERT INTO distribution_items (distribution_id, food_item_id, quantity)
        VALUES (?, ?, ?)
    )sql");

    sqlite3_int64 id = 0;
    for (const auto& distribution : service.getDistributions()) {
        ++id;
        history.bind(1, id);
        history.bind(2, distribution.getHouseholdId());
        history.bind(3, distribution.getStaffId());
        history.bind(4, distribution.getDate());
        history.insert();

        for (const auto& [itemId, quantity] : distribution.getItems()) {
            details.bind(1, id);
            details.bind(2, itemId);
            details.bind(3, quantity);
            details.insert();
        }
    }

    execute("PRAGMA user_version = 1; COMMIT;");
}

void PantryDatabase::execute(const char* sql) const {
    if (sqlite3_exec(database.get(), sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(database.get()));
    }
}