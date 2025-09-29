# 🚨 **UNotorietyComponent** (`NotorietyComponent.h`)

---

## 🎯 Purpose
Tracks individual character reputation and notoriety for crime and social systems. Used for AI reactions and law enforcement.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `GetNotoriety()`        | Returns current notoriety value             |
| `ModifyNotoriety(Amt)`  | Changes notoriety by given amount           |
| `OnNotorietyChanged`    | Event/callback for notoriety updates        |

---

## 💡 Example Usage
```cpp
// Increase notoriety by 5
UNotorietyComponent* Notoriety = Actor->FindComponentByClass<UNotorietyComponent>();
if (Notoriety) {
    Notoriety->ModifyNotoriety(5.0f);
}
```

---

## 🔗 Integration Points
- Used by **CrimeManagerSubsystem**, **FactionManagerSubsystem**, and AI
- Can trigger world events or law enforcement
- Notifies UI and gameplay systems of notoriety changes

---

## 📝 Notes
- Extend to support notoriety decay, fame, and advanced social effects.
- Consider adding notoriety thresholds for world responses.
