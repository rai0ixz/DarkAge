# ⛏️ **ADAResourceNode** (`DAResourceNode.h`)

---

## 🎯 Purpose
Represents a resource node in the world (e.g., tree, rock, ore vein). Handles harvesting logic, health, respawn, and integration with inventory and tool systems.

---

## 🗝️ Key Methods & Properties
| Method/Property                | Description                                         |
|--------------------------------|-----------------------------------------------------|
| `OnInteract_Implementation()`  | Handles player interaction with the node            |
| `TakeHarvestDamage()`          | Applies damage from harvesting tools                |
| `NodeData`                     | Data struct for node type, health, yields, etc.     |
| `CurrentHealth`                | Current health of the node                          |
| `ResourceNodeDataTable`        | DataTable reference for node configuration          |

---

## 💡 Example Usage
```cpp
// Interact with a resource node
ADAResourceNode* Node = ...;
Node->OnInteract_Implementation(PlayerCharacter);
```

---

## 🔗 Integration Points
- Used by player and AI for resource gathering
- Interacts with **InventoryComponent** and **DAToolItem**
- Notifies world and UI systems of resource changes

---

## 🏆 Best Practices
- Always check tool type and power before applying damage
- Use DataTables for easy balancing and content updates

---

## ⚠️ Common Pitfalls
- Forgetting to reset health on respawn
- Not validating tool requirements

---

## 🛠️ How-To Guides
- How to add a new resource node type: Add a row to the DataTable and create a new Blueprint subclass if needed.
- How to implement respawn: Use timers and NodeData.RespawnTime.

---

## 🖼️ Visuals
![Resource Node Example](../assets/resource-node-example.png)

---

## 📝 Notes
- Extend to support advanced respawn, rare drops, and environmental effects.
