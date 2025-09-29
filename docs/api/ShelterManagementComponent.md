# 🏕️ **ShelterManagementComponent**

---

## 🎯 Purpose
Manages player and NPC shelters, including construction, upgrades, and environmental protection. Handles shelter status, durability, and integration with survival systems.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `BuildShelter()`        | Constructs a new shelter                    |
| `UpgradeShelter()`      | Improves shelter durability or features     |
| `GetShelterStatus()`    | Returns current shelter status              |
| `OnShelterDamaged`      | Event/callback for shelter damage           |

---

## 💡 Example Usage
```cpp
// Build a new shelter
UShelterManagementComponent* Shelter = Player->FindComponentByClass<UShelterManagementComponent>();
if (Shelter) {
    Shelter->BuildShelter();
}
```

---

## 🔗 Integration Points
- Works with **AdvancedSurvivalSubsystem** for environmental effects
- Can interact with **InventoryComponent** for building materials
- Notifies UI and gameplay systems of shelter changes

---

## 📝 Notes
- Extend to support multi-person shelters, weatherproofing, and advanced upgrades.