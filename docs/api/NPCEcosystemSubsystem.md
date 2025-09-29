# 🧑‍🌾 **NPCEcosystemSubsystem**

---

## 🎯 Purpose
Simulates the ecosystem for all NPCs, including population dynamics, migration, and environmental adaptation. Coordinates NPC behavior with world ecosystem events.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `UpdateNPCs()`          | Advances simulation for all NPCs            |
| `GetPopulationStats()`  | Returns current NPC population data         |
| `TriggerMigration()`    | Simulates migration events                  |
| `OnNPCEvent`            | Event/callback for major NPC ecosystem events|

---

## 💡 Example Usage
```cpp
// Update all NPCs for the current tick
UNPCEcosystemSubsystem* NPCSystem = World->GetSubsystem<UNPCEcosystemSubsystem>();
if (NPCSystem) {
    NPCSystem->UpdateNPCs();
}
```

---

## 🔗 Integration Points
- Works with **WorldEcosystemSubsystem** for environmental data
- Can interact with **AICombatBehaviorComponent** for NPC behavior
- Notifies UI and gameplay systems of major NPC events

---

## 📝 Notes
- Extend to support advanced migration, breeding, and NPC-environment interactions.