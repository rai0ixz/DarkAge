# 🖥️ **ADAHUD** (`DAHUD.h`)

---

## 🎯 Purpose
Main HUD class for Dark Age. Manages and displays all in-game UI widgets, including player status, inventory, notifications, and interaction prompts.

---

## 🗝️ Key Methods & Properties
| Method/Property             | Description                                         |
|-----------------------------|-----------------------------------------------------|
| `BeginPlay()`               | Initializes and adds widgets to the viewport         |
| `ShowInventory()`           | Shows the inventory UI                              |
| `HideInventory()`           | Hides the inventory UI                              |
| `ShowNotification(msg)`     | Displays notifications to the player                |
| `Widgets`                   | References to all managed UI widgets                |

---

## 💡 Example Usage
```cpp
// Show the inventory UI
ADAHUD* HUD = Cast<ADAHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
if (HUD) {
    HUD->ShowInventory();
}
```

---

## 🔗 Integration Points
- Receives data from player character and components
- Manages **UDAPlayerStatusWidget**, **UDAInventoryWidget**, **UDAInteractionPromptWidget**, **UDANotificationWidget**, etc.
- Notifies UI and gameplay systems of HUD changes

---

## 📝 Notes
- Extend to support additional UI elements and custom HUD logic as needed.
