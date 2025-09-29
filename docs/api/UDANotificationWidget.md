# 🔔 **UDANotificationWidget** (`DANotificationWidget.h`)

---

## 🎯 Purpose
Displays notifications and alerts to the player (e.g., item pickup, quest update).

---

## 🗝️ Key Methods & Properties
| Method/Property             | Description                                 |
|-----------------------------|---------------------------------------------|
| `SetNotificationText(Text)` | Sets the notification message               |
| `Show()`                    | Shows the notification widget               |
| `Hide()`                    | Hides the notification widget               |

---

## 💡 Example Usage
```cpp
// Show a notification
UDANotificationWidget* Widget = ...;
if (Widget) {
    Widget->SetNotificationText(FText::FromString("Quest Complete!"));
    Widget->Show();
}
```

---

## 🔗 Integration Points
- Receives data from game events, inventory, quest, or survival systems
- Part of the main HUD
- Notifies UI and gameplay systems of notifications

---

## 📝 Notes
- Extend to support notification history, categories, or advanced display options.
