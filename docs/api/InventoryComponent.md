# UInventoryComponent (InventoryComponent.h)

## Purpose
This component manages an actor's inventory, including items and currency. It is designed with multiplayer support, replicating the inventory state from the server to clients. It provides a core set of functionalities for adding, removing, and using items.

## Key Structs & Delegates

### `FInventorySlot` (Struct)
This struct represents a single slot in the inventory.
- `Item (FItemData)`: A struct containing all the static data for the item in this slot (e.g., name, description, mesh, effects). This data comes from a `DT_Items` DataTable.
- `Quantity (int32)`: The number of items currently held in this slot.

### `FOnInventoryUpdated` (Delegate)
A multicast delegate that is broadcast whenever the `Items` array is updated and replicated. UI elements should bind to this delegate to refresh their display.

## Key Properties
- `Items (TArray<FInventorySlot>)`: The main array holding the inventory's contents. This property is replicated using an `OnRep` function (`OnRep_Items`).
- `Currency (int32)`: The amount of currency the actor possesses.
- `OnInventoryUpdated (FOnInventoryUpdated)`: The delegate that UI can bind to.

## Key Methods

### Item Management
- `AddItem(const FItemData& Item, int32 Quantity)`: Adds a specified quantity of an item to the inventory. Handles stacking if the item is stackable and a slot with that item already exists. Returns `true` on success.
- `RemoveItem(const FItemData& Item, int32 Quantity)`: Removes a specified quantity of an item from the inventory. Returns `true` on success.
- `HasItem(const FItemData& Item, int32 Quantity) const`: Checks if the inventory contains at least the specified quantity of an item.
- `UseItemBySlot(int32 SlotIndex)`: Intended to be called when a player uses an item from the UI. The logic for what "using" an item means is typically defined in the item's data itself or in another system.

### Currency Management
- `CanAfford(int32 Amount) const`: Checks if the actor has enough currency.
- `RemoveCurrency(int32 Amount)`: Deducts a specified amount of currency.

### Replication
- `OnRep_Items()`: The `OnRep` function for the `Items` array. It automatically broadcasts the `OnInventoryUpdated` delegate, notifying the client-side UI to refresh.

## Example Usage

To add an item to a character's inventory (e.g., after picking it up):
```cpp
// Assume we have a pointer to the ItemData from a Data Table
UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
if (Inventory)
{
    // This would typically be called on the server
    Inventory->AddItem(ItemDataFromTable, 1);
}
```

To bind the UI to inventory changes:
```cpp
// In a UDAInventoryWidget's Initialize method
UInventoryComponent* Inventory = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
if (Inventory)
{
    Inventory->OnInventoryUpdated.AddDynamic(this, &UDAInventoryWidget::RefreshInventory);
}

// The handler function in the widget
void UDAInventoryWidget::RefreshInventory()
{
    // Code to clear and repopulate the inventory UI based on the component's Items array
}
```

## Integration Points
- **UI Widgets (`UDAInventoryWidget`)**: The UI is the primary consumer of this component's data. It should bind to `OnInventoryUpdated` to stay in sync with the inventory's state.
- **`DT_Items` (DataTable)**: This component relies heavily on a `UDataTable` that uses the `FItemData` struct to define all possible items in the game.
- **Crafting & Vendor Systems**: Other systems will call `AddItem`, `RemoveItem`, and the currency functions to give/take items and money during crafting and trade.
