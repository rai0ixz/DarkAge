# ❤️‍🩹 **UDAPlayerStatusWidget** (`DAPlayerStatusWidget.h`)

---

## 🎯 Purpose
Displays the player's core survival stats (health, stamina, hunger, thirst) in the HUD. Provides real-time feedback on player condition.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `HealthBar`             | Progress bar for health                     |
| `StaminaBar`            | Progress bar for stamina                    |
| `HungerBar`             | Progress bar for hunger                     |
| `ThirstBar`             | Progress bar for thirst                     |
| `HealthText`            | Numeric display for health                  |
| `StaminaText`           | Numeric display for stamina                 |
| `HungerText`            | Numeric display for hunger                  |
| `ThirstText`            | Numeric display for thirst                  |
| `UpdateStatus()`        | Updates all UI elements with current stats  |

---

## 💡 Example Usage
```cpp
// Update the status widget after stat change
UDAPlayerStatusWidget* Widget = ...;
if (Widget) {
    Widget->UpdateStatus();
}
```

---

## 🔗 Integration Points
- Receives data from **StatlineComponent** or player character
- Part of the main HUD (**ADAHUD**)
- Notifies UI and gameplay systems of stat changes

---

## 📝 Notes
- Extend to support additional stats or effects as needed.
