# Community Food Pantry Inventory and Distribution System

A CS 410 project to help pantry volunteers track donated inventory and record food distributions to households.

## Project goals

- Record donations and keep available quantities accurate.
- Help volunteers find items by category, expiration date, and storage location.
- Record distributions while preventing quantities from falling below zero.
- Flag items that are expired, near expiration, damaged, or unavailable.
- Provide useful inventory and distribution reports for pantry staff.

## Current Status

The Distribution class records household ID, staff ID, date, and food item quantities. It rejects nonpositive quantities and accumulates quantities when the same item is added again.

Inventory validation and the complete application workflow are still pending.

## Initial product-owner questions

1. Which details must volunteers record for each donated item or batch?
2. What rules determine which items and quantities a household may receive?
3. Which household details are necessary, and who may access them?
4. How should expired, damaged, reserved, or otherwise unavailable items be handled?
5. Which reports and alerts would be most useful to pantry staff?

## Repository Structure

- `include/`: C++ class declarations.
- `src/`: C++ implementations.
- `tests/`: automated tests.
- `docs/`: project documentation.
- `.github/workflows/`: continuous integration configuration.
- `CMakeLists.txt`: build and test configuration.

## Requirements

- A C++17-compatible compiler
- CMake 3.16 or newer
- Git and internet access to download GoogleTest during configuration

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

Four automated tests verify:

- Adding a valid quantity.
- Rejecting zero quantity.
- Accumulating quantities for the same item.
- Rejecting negative quantity without changing the existing quantity.

## Continuous Integration

GitHub Actions builds the project and runs the tests on pushes and pull requests.

## Running the Application

An application executable is not available yet. The current build provides the Distribution library and its tests.
