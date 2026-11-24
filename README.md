# Community Food Pantry Inventory and Distribution System

A CS 410 project to help pantry volunteers track donated inventory and record food distributions to households.

## Project Goals

- Record donations and keep available quantities accurate.
- Help volunteers find items by category, expiration date, and storage location.
- Record distributions while preventing quantities from falling below zero.
- Flag items that are expired, near expiration, damaged, or unavailable.
- Provide useful inventory and distribution reports for pantry staff.

## Current Status

The console application provides a menu to view inventory, add food items,
record donations, register distributions, and exit. Users can perform multiple
actions in one session, with inventory updates reflected in subsequent actions.

Invalid input prompts users to try again without restarting the operation.
When adding food through the menu, users must enter a unique positive ID,
a nonblank name, a positive starting quantity, and a valid expiration
date that is not in the past.

Donations increase the stock of an existing food item. Distribution requests
require positive household and staff IDs, a valid calendar date, and available,
unexpired food. Repeated food item IDs accumulate their requested quantities.

Successful distributions reduce stock and create a distribution record.
InventoryService validates requests before changing inventory or recording
a distribution.

Shared helpers handle calendar-date validation and numeric input.
Automated tests cover these helpers and the existing distribution and
inventory services.

## Initial Product-Owner Questions

1. Which details must volunteers record for each donated item or batch?
2. What rules determine which items and quantities a household may receive?
3. Which household details are necessary, and who may access them?
4. How should expired, damaged, reserved, or otherwise unavailable items be handled?
5. Which reports and alerts would be most useful to pantry staff?

## Repository Structure

- `include/`: C++ class declarations and shared utility declarations.
- `src/`: C++ implementations and the application entry point, `main.cpp`.
- `tests/`: automated tests.
- `.github/workflows/`: continuous integration configuration.
- `CMakeLists.txt`: build and test configuration.

## Prerequisites

- A C++17-compatible compiler.
- CMake 3.16 or newer, available in PATH.
- A build tool such as Make or Ninja.
- Git and internet access to download GoogleTest during the first configuration.

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

Choose an option:
```

Choose an option by entering its number:

- **1. View inventory:** Display all current food items, including their IDs, names, quantities, and expiration dates.
- **2. Add food item:** Enter a unique positive food item ID, a nonblank name, a positive starting quantity, and an expiration date in `YYYY-MM-DD` format. The expiration date must be a real calendar date and cannot be in the past.
- **3. Record donation:** Enter an existing food item ID and a positive quantity to add to its stock.
- **4. Register distribution:** Enter positive household and staff IDs and a valid distribution date in `YYYY-MM-DD` format. Then enter food item IDs and requested quantities. Enter `0` as the food item ID to submit the distribution after adding at least one item.
- **5. Exit:** Close the application.

Invalid input displays an error message and prompts you to try again. Successful distributions reduce available stock and are recorded. Rejected distributions leave inventory and distribution records unchanged. Food expiration checks use the current system date.

After an operation finishes, the main menu appears again. You can perform multiple actions in one session, and inventory changes remain available throughout that session.

The application starts with this example inventory:

| Food Item ID | Name | Quantity | Expiration Date |
|---|---|---|---|
| 1 | Rice | 50 | 2027-12-31 |
| 2 | Pasta | 30 | 2027-12-31 |
| 3 | Beans | 20 | 2027-12-31 |

**Note:** Inventory and distribution records are stored only in memory. Changes are not saved after exiting the application.

## Example Usage

Complete these steps in one session:

| Step | Menu option | Input | Expected result |
|---|---|---|---|
| Add an item | 2 | ID: 4, name: Canned Soup, quantity: 20, expiration: 2028-12-31 | Item is added with 20 units |
| Record a donation | 3 | Food item ID: 4, quantity: 5 | Stock increases to 25 |
| View inventory | 1 | No additional input | Canned Soup shows 25 units |
| Register a distribution | 4 | Household ID: 1, staff ID: 1, date: 2026-11-20, food item ID: 4, quantity: 3, then food item ID: 0 | Distribution is recorded |
| View inventory | 1 | No additional input | Canned Soup shows 22 units |

Use an expiration date that is not in the past when running the example.
The food must also be unexpired when registering the distribution.

### Invalid Input and Retries

Invalid answers display an error message and repeat the relevant prompt:

- Enter `abc` or `2.5` at a numeric prompt: the numeric prompt appears again.
- Enter an existing ID when adding food: the ID prompt appears again.
- Enter a blank food name: the name prompt appears again.
- Enter `2028-02-30` as an expiration date: the date prompt appears again.
- Enter a past expiration date: the date prompt appears again.
- Enter an unknown food item ID when recording a donation or distribution:
  the food item ID prompt appears again.
- Request more units than are available: the quantity prompt appears again.

Previously entered valid fields are retained while the user corrects the
invalid answer. Inventory changes occur only when the operation succeeds.

For example, in a fresh session, choose option 4 and request 100 units of
Rice, which starts with 50 units. The application prompts for another
quantity without changing stock. Enter 5, then enter 0 at the food item ID
prompt to submit. After the distribution succeeds, Rice has 45 units.

Entering 0 submits a distribution only at the food item ID prompt.
It is not a valid requested quantity.

### Successful Distribution

Enter these values when prompted:

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

Expected result while the example food has not expired:

```text
Distribution recorded successfully.

Remaining inventory:
Rice: 45
Pasta: 27
Beans: 20
Recorded distributions: 1
```

Repeated food item IDs accumulate their quantities.

If no unexpired food is in stock when you choose option 4, the application
displays a message and returns to the main menu.

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

Console-input tests use simulated input and captured output.
The helper tests do not automate the complete interactive menu workflow;
use the example workflow above to check application integration manually.

## Continuous Integration

GitHub Actions builds the project and runs automated tests on pushes and pull requests. Check the pull request's Checks tab for the current CI result.

## Current Limitations

- Inventory and distribution records are stored only in memory and reset
  whenever the application restarts.
- The initial inventory is hardcoded for demonstration purposes.
- The example food expires after 2027-12-31; update the demo inventory dates
  for later demonstrations.
- Household and staff IDs must be positive but are not verified against
  existing records.
- Donations increase stock for an existing item without tracking separate
  batches or expiration dates.
- Distribution dates are validated as calendar dates but are not restricted
  to today.
- There is no explicit cancel or return-to-menu command during an operation.
  The menu reappears automatically after an operation finishes.
- Search, reports, and additional item-status features listed in the project
  goals are not implemented in the current console workflow.