# 🏹 **USurvivalSkillsComponent** (`SurvivalSkillsComponent.h`)

---

## 🎯 Purpose
Manages a character's survival skills, such as crafting, resource gathering, and shelter building. Tracks skill progression and provides functionalities related to survival actions.

---

## 🗝️ Key Methods & Properties
| Method/Property         | Description                                 |
|-------------------------|---------------------------------------------|
| `GetSkillLevel(Skill)`  | Returns the current level of a skill        |
| `AddXP(Skill, XP)`      | Adds experience points to a skill           |
| `UnlockSkill(Skill)`    | Unlocks a new survival skill                |
| `OnSkillChanged`        | Event/callback for skill changes            |

---

## 💡 Example Usage
```cpp
// Add XP to the crafting skill
USurvivalSkillsComponent* Skills = Player->FindComponentByClass<USurvivalSkillsComponent>();
if (Skills) {
    Skills->AddXP(ESurvivalSkill::Crafting, 50.0f);
}
```

---

## 🔗 Integration Points
- Used by player and NPC characters
- Interacts with **CraftingComponent** and **StatlineComponent**
- Notifies UI and gameplay systems of skill changes

---

## 📝 Notes
- Extend to support advanced skill trees, perks, and group survival actions.