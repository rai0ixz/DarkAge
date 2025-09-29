# 🌍 **UWorldEcosystemSubsystem** (`WorldEcosystemSubsystem.h`)

---

## 🎯 Purpose
Simulates the world’s ecosystem, including weather, seasons, resource management, and animal populations. Coordinates environmental effects across regions.

---

## 🗝️ Key Methods & Properties
| Method/Property                   | Description                                         |
|-----------------------------------|-----------------------------------------------------|
| `UpdateEcosystem()`               | Advances simulation for all regions                  |
| `GetResourceAvailability()`       | Returns resource data for a region/resource type     |
| `TriggerEnvironmentalEvent()`     | Simulates events (storms, droughts, etc.)            |
| `OnEcosystemChanged`              | Event/callback for major ecosystem changes           |

---

## 💡 Example Usage
```cpp
// Advance the ecosystem simulation
UWorldEcosystemSubsystem* Eco = World->GetSubsystem<UWorldEcosystemSubsystem>();
if (Eco) {
    Eco->UpdateEcosystem();
}
```

---

## 🔗 Integration Points
- Works with **EnvironmentalFactorsComponent**, **WeatherSystem**, and **WorldManagementSubsystem**
- Data used by survival and AI systems
- Notifies UI and gameplay systems of ecosystem changes

---

## 📝 Notes
- Extend to support migration, advanced events, and integration with economy systems.
