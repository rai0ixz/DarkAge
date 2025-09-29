# 🎒 **UDAInventoryWidget** (`DAInventoryWidget.h`)

---

## 🎯 Purpose
Provides the user interface for managing the player's inventory, including item display, selection, and basic actions.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `InventoryList`         | UI element displaying items                 |
| `OnItemSelected`        | Event/callback for item selection           |
| `RefreshInventory()`    | Updates the UI with the current inventory   |

---

## 💡 Example Usage
```cpp
// Refresh the inventory UI after an item is added
UDAInventoryWidget* Widget = ...;
if (Widget) {
    Widget->RefreshInventory();
}
```

---

## 🔗 Integration Points
- Works with **InventoryComponent** for data
- Can trigger item use, equip, or drop actions
- Notifies UI and gameplay systems of inventory changes

---

## 📝 Notes
- Extend to support drag-and-drop, filtering, and advanced item management features.
