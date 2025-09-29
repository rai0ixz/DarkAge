# 🗨️ **UDAInteractionPromptWidget** (`DAInteractionPromptWidget.h`)

---

## 🎯 Purpose
Displays context-sensitive prompts for player interactions (e.g., "Press E to open door").

---

## 🗝️ Key Methods & Properties
| Method/Property             | Description                                 |
|-----------------------------|---------------------------------------------|
| `SetPromptText(Text)`       | Sets the main prompt                        |
| `SetActionText(Text)`       | Sets the action description                  |
| `Show()`                    | Shows the prompt widget                     |
| `Hide()`                    | Hides the prompt widget                     |

---

## 💡 Example Usage
```cpp
// Show an interaction prompt
UDAInteractionPromptWidget* Widget = ...;
if (Widget) {
    Widget->SetPromptText(FText::FromString("Press E to open door"));
    Widget->Show();
}
```

---

## 🔗 Integration Points
- Receives data from player controller or interaction system
- Part of the main HUD
- Notifies UI and gameplay systems of interaction prompts

---

## 📝 Notes
- Extend to support multiple actions, icons, or advanced interaction logic.
