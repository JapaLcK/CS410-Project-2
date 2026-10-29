# Community Food Pantry Inventory and Distribution System

A CS 410 project to help pantry volunteers track donated inventory and record food distributions to households.

## Project Goals

- Record donations and keep available quantities accurate.
- Help volunteers find items by category, expiration date, and storage location.
- Record distributions while preventing quantities from falling below zero.
- Flag items that are expired, near expiration, damaged, or unavailable.
- Provide useful inventory and distribution reports for pantry staff.

## Current Status

The console application collects household ID, staff ID, distribution date, food item IDs, and quantities.

Distribution rejects non-positive quantities and accumulates quantities for repeated food item IDs. InventoryService manages stock, accepts donations, and validates distribution requests. Successful distributions reduce inventory and are recorded. Rejected requests leave inventory and records unchanged.

The application displays confirmation or the service's rejection reason, remaining inventory, and the number of recorded distributions.

## Initial Product-Owner Questions

1. Which details must volunteers record for each donated item or batch?
2. What rules determine which items and quantities a household may receive?
3. Which household details are necessary, and who may access them?
4. How should expired, damaged, reserved, or otherwise unavailable items be handled?
5. Which reports and alerts would be most useful to pantry staff?

## Repository Structure

- `include/`: C++ class declarations.
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

On macOS or Linux with the build above:

```bash
./build/food_pantry
```

Enter a positive household ID, a positive staff ID, and a distribution date in YYYY-MM-DD format. Then enter food item IDs and quantities. Enter 0 as the food item ID to submit the request.

The application starts with this example inventory:

| Food Item ID | Name | Quantity | Expiration Date |
|---|---|---|---|
| 1 | Rice | 50 | 2027-12-31 |
| 2 | Pasta | 30 | 2027-12-31 |
| 3 | Beans | 20 | 2027-12-31 |

InventoryService rejects empty distributions, unknown food items, non-positive quantities, expired food, and insufficient stock. Expiration checks use the current system date.

## Example Usage

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

### Rejected Distribution

Restart the application. Enter household ID 1, staff ID 1, and date 2026-10-29. Request 100 units of Rice (item ID 1), then enter 0 to submit.

Expected result while the example food has not expired:

```text
Distribution rejected: Insufficient inventory.

Remaining inventory:
Rice: 50
Pasta: 30
Beans: 20
Recorded distributions: 0
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure --no-tests=error
```

All 9 automated tests passed locally after application integration. They cover:

- Adding valid quantities.
- Rejecting zero and negative quantities.
- Accumulating quantities for repeated food item IDs.
- Updating stock after a successful distribution.
- Rejecting insufficient stock, expired food, and unknown items without changing stock.
- Adding donations to inventory.

The console workflow was also manually checked with successful and rejected distributions.

## Continuous Integration

GitHub Actions builds the project and runs automated tests on pushes and pull requests. Check the pull request's Checks tab for the current CI result.

## Current Limitations

- Inventory and distribution records are stored only in memory and reset on every application restart.
- The application processes one distribution per run.
- The initial inventory is hardcoded for demonstration purposes.
- Distribution dates are accepted as text without format or calendar validation.
- Household and staff IDs are checked for positive values but are not verified against existing records.
- Invalid numeric input or non-positive quantities end the application rather than prompting again.
- The example food expires after 2027-12-31; update the demo inventory dates for later demonstrations.
- Search, reports, and additional item-status features listed in the project goals are not implemented in the current console workflow.