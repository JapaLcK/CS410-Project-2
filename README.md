# Community Food Pantry Inventory and Distribution System

A CS 410 project to help pantry volunteers track donated inventory and record food distributions to households.

## Project Goals

- Record donations and keep available quantities accurate.
- Help volunteers find items by category, expiration date, and storage location.
- Record distributions while preventing quantities from falling below zero.
- Flag items that are expired, near expiration, damaged, or unavailable.
- Provide useful inventory and distribution reports for pantry staff.

## Current Status

The console application provides a menu to view inventory, add and remove food
items, record donations, and register distributions. Users can perform multiple
actions in one session and see updated stock after each completed action.

Invalid input prompts users to try again. Enter `:cancel` at any input prompt
during an operation to discard that operation and return to the main menu.

When adding food, users must enter a unique positive ID, a nonblank name, a
positive starting quantity, and a valid expiration date that is today or later.
Donations increase the stock of an existing food item.

Distribution requests require positive household and staff IDs, a valid
calendar date, and sufficient stock of unexpired food. Repeated food item IDs
accumulate their requested quantities. `InventoryService` validates the entire
request before reducing stock or recording the distribution.

Inventory and distribution records are saved in a local SQLite database when
the application exits normally. They are loaded the next time the program starts
from the same directory.

## Initial Product-Owner Questions

1. Which details must volunteers record for each donated item or batch?
2. What rules determine which items and quantities a household may receive?
3. Which household details are necessary, and who may access them?
4. How should expired, damaged, reserved, or otherwise unavailable items be handled?
5. Which reports and alerts would be most useful to pantry staff?

## Repository Structure

- `include/` contains C++ class and utility declarations.
- `src/` contains C++ implementations and the application entry point, `main.cpp`.
- `tests/` contains automated tests.
- `.github/workflows/` contains the continuous integration configuration.
- `CMakeLists.txt` contains the build and test configuration.

## Prerequisites

- A C++17-compatible compiler.
- CMake 3.16 or newer, available in `PATH`.
- A build tool such as Make or Ninja.
- SQLite 3 development headers and library. On Ubuntu or Debian, install `libsqlite3-dev`.
- Git and internet access to download GoogleTest during the first test configuration.

## Configure and Build

Run these commands from the project root:

```bash
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

## Run the Application

On macOS or Linux, run this command from the project root after building:

```bash
./build/food_pantry
```

The application displays the following menu:

```text
Food Pantry Menu

1. View inventory
2. Add food item
3. Record donation
4. Register distribution
5. Exit
6. Remove food item
Type :cancel at any prompt to return to the main menu.

Choose an option:
```

- **1. View inventory** displays food item IDs, names, quantities, and expiration dates.
- **2. Add food item** asks for a unique positive ID, a nonblank name, a positive
  starting quantity, and an expiration date in `YYYY-MM-DD` format.
- **3. Record donation** asks for an existing food item ID and a positive quantity
  to add to its stock.
- **4. Register distribution** asks for a household ID, staff ID, and distribution
  date, followed by food item IDs and quantities. After adding at least one
  item, enter `0` at the food item ID prompt to submit the distribution.
- **5. Exit** saves the current inventory and distribution history and closes the application.
- **6. Remove food item** asks for an existing food item ID and confirmation.
  Enter `y` to remove the item and all its remaining stock or `n` to return
  to the main menu. Earlier distribution records retain the item ID.

### Cancel an Operation

Enter `:cancel` at any input prompt during an operation to return to the main
menu. For example, if you enter `:cancel` after adding items to a draft
distribution, the draft is discarded and the stock stays the same. Actions
completed earlier in the session remain in effect.

### Saved Data

The application uses `food-pantry.sqlite3` in the directory where it was started.
Choose **5. Exit** to save your work. Start the program from the same directory
to load the saved inventory and distribution records. To run a fresh demo,
launch the executable from a separate empty directory.

On the first run, the application starts with this example inventory:

| Food Item ID | Name | Quantity | Expiration Date |
|---|---|---:|---|
| 1 | Rice | 50 | 2027-12-31 |
| 2 | Pasta | 30 | 2027-12-31 |
| 3 | Beans | 20 | 2027-12-31 |

## Example Usage

Complete these steps with a fresh database:

| Step | Menu option | Input | Expected result |
|---|---:|---|---|
| Add an item | 2 | ID `4`, name `Canned Soup`, quantity `20`, expiration `2028-12-31` | Canned Soup has 20 units. |
| Record a donation | 3 | Item ID `4`, quantity `5` | Stock increases to 25. |
| View inventory | 1 | No additional input | Canned Soup shows 25 units. |
| Cancel a distribution | 4 | Household ID `1`, staff ID `1`, today's date, item ID `4`, quantity `10`, then `:cancel` at the next item ID prompt | The main menu returns and Canned Soup still has 25 units. |
| Register a distribution | 4 | Household ID `1`, staff ID `1`, today's date, item ID `4`, quantity `3`, then item ID `0` | The distribution is recorded and 22 units remain. |
| Remove an item | 6 | Item ID `2`, then `y` to confirm | Pasta is removed from the inventory. |
| Save and restart | 5, then 1 after restarting | Start the application from the same directory and view inventory | Canned Soup still has 22 units and Pasta is absent. |

Enter dates in `YYYY-MM-DD` format. If the example expiration date has passed,
use a later date when adding Canned Soup.

### Invalid Input and Retries

- Entering `abc` or `2.5` at a numeric prompt repeats the prompt.
- Entering an existing ID when adding food asks for a different ID.
- Entering a blank food name asks for a name again.
- Entering an invalid date such as `2028-02-30` or an expiration date in the
  past asks for another date.
- Entering an unknown item ID when donating, distributing, or removing food
  asks for another ID.
- Requesting more units than are available asks for another quantity.

Previously entered valid fields are kept while the user corrects the invalid
answer. Rejected requests leave inventory and distribution records unchanged.

For example, with the initial stock of 50 units of Rice, choose option `4`
and request 100 units. The application asks for another quantity. Enter `5`,
then enter `0` at the next food item ID prompt to submit. Rice now has 45 units.

If no unexpired food is in stock when you choose option `4`, the application
displays a message and returns to the main menu. Expiration checks use the
current system date.

### Successful Distribution

With the initial example inventory, choose option `4` and enter:

```text
Household ID: 1
Staff ID: 1
Distribution date (YYYY-MM-DD): 2026-10-29
Food item ID (0 to submit): 1
Quantity: 5
Food item ID (0 to submit): 2
Quantity: 3
Food item ID (0 to submit): 0
```

Expected output on a fresh database while the example food is unexpired:

```text
Distribution recorded successfully.

Current inventory:
1 - Rice
Quantity: 45
Expires: 2027-12-31
2 - Pasta
Quantity: 27
Expires: 2027-12-31
3 - Beans
Quantity: 20
Expires: 2027-12-31
Recorded distributions: 1
```

## Run Tests

After successfully building the project, run:

```bash
ctest --test-dir build --output-on-failure --no-tests=error
```

Automated tests cover:

- Valid, zero, and negative distribution quantities.
- Accumulating quantities for repeated food item IDs.
- Successful distributions updating stock and records.
- Rejected distributions leaving inventory and records unchanged.
- Food item validation and duplicate IDs.
- Donations, including unknown items, invalid quantities, and integer overflow.
- Retrieving inventory items with their current stock.
- Calendar-date formats, month lengths, and leap-year rules.
- Numeric input validation, retry messages, and end-of-input handling.

Use the **Example Usage** steps to manually verify cancellation, item removal,
and persistence after a restart.

## Continuous Integration

GitHub Actions builds the project and runs automated tests on pushes and pull
requests. Check the pull request's **Checks** tab for the current CI result.

## Current Limitations

- Changes are saved when the application exits normally. A forced shutdown can
  discard changes made during the active session.
- One application session at a time can edit a given database.
- The initial inventory is hardcoded for demonstration purposes. The sample
  items expire after `2027-12-31`.
- Household and staff IDs must be positive. The application stores them as
  entered without looking up household or staff records.
- Donations increase stock for an existing item without tracking separate
  batches or expiration dates.
- Distribution dates can be any valid calendar date.
- Search, reports, and additional item-status features listed in the project
  goals are outside the current console workflow.