# 🖼️ **UDAUserInterface** (`DAUserInterface.h`)

---

## 🎯 Purpose
Base class for all Dark Age UI widgets. Provides common functionality and styling for derived widgets.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `Theme`                 | Common UI theme property                    |
| `Style`                 | Common UI style property                    |
| `OnUIEvent`             | Base event handler for UI interaction       |

---

## 💡 Example Usage
```cpp
// Inherit from UDAUserInterface for a custom widget
class UMyCustomWidget : public UDAUserInterface {
    // ...
};
```

---

## 🔗 Integration Points
- Used as the base for all in-game UI
- Managed by **ADAHUD**
- Notifies UI and gameplay systems of UI changes

---

## 📝 Notes
- Extend to add shared logic or styling for all widgets.
