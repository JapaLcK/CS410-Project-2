# Community Food Pantry Architecture

The Community Food Pantry Inventory and Distribution System tracks donated food, maintains available stock, and records food distributed to households.

## Class Responsibilities

| Class | Responsibility |
| --- | --- |
| `FoodItem` | Represents a food item and stores its name, available quantity, and expiration date. |
| `Donation` | Records a donation batch, including the food item, quantity donated, donation date, and responsible staff member. |
| `Household` | Stores a household's address, contact information, and history of food received. |
| `StaffMember` | Stores the staff member's name and the distributions they handled. |
| `Distribution` | Records the household, staff member, date, and quantities of the items distributed. |
| `InventoryService` | Adds donated quantities to inventory, checks that requested food is available and unexpired, and updates stock after a successful distribution. |

## Design Decisions

The design favors composition. A `Distribution` records item IDs and the quantities given to a household. Each `FoodItem` holds the quantity still available in inventory. `InventoryService` checks every requested item before reducing stock and recording the distribution. If any item fails validation, stock and distribution records stay as they were.

## Planned Repository Structure

```text
community-food-pantry/
├── README.md
├── docs/                  # Requirements, architecture, and UML diagram
│   └── architecture.md
├── include/               # C++ class declarations (.h)
├── src/                   # C++ implementations (.cpp)
├── tests/                 # Automated tests
└── data/                  # Persistent application data and sample files
```

## Implementation Status

The current C++ console application uses `FoodItem`, `Distribution`, and `InventoryService`. It identifies households and staff members by ID and keeps inventory and distribution records in memory during each run. The design includes dedicated `Donation`, `Household`, and `StaffMember` classes and plans to save application data between runs.
