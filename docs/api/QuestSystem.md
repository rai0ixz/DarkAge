# UQuestSystem (QuestSystem.h)

## Purpose
Manages dynamic quest generation, tracking, and completion for the Dark Age project. It integrates with various game subsystems like economy and factions to create emergent and context-aware quests.

## Key Methods & Properties

### Quest Management
- `GenerateQuest(EQuestType Type, FName Region, int32 Difficulty)`: Generates a new quest of a specific type.
- `AcceptQuest(FName QuestID)`: Marks a quest as accepted by the player.
- `AbandonQuest(FName QuestID)`: Abandons an active quest.
- `CompleteQuest(FName QuestID)`: Marks a quest as successfully completed and triggers rewards.
- `FailQuest(FName QuestID)`: Marks a quest as failed.
- `UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress)`: Updates the progress of a specific quest objective.

### Data Retrieval
- `GetAvailableQuests() const`: Returns a list of all quests currently available to be accepted.
- `GetActiveQuests() const`: Returns a list of all quests the player has currently accepted.
- `GetQuestData(FName QuestID) const`: Retrieves the full data structure for a specific quest.
- `AreAllObjectivesCompleted(FName QuestID) const`: Checks if all objectives for a given quest are complete.

### Delegates
- `OnQuestStatusChanged`: A multicast delegate that broadcasts when a quest's state changes (e.g., from `Active` to `Completed`).
  - **Parameters**: `FName QuestID`, `EQuestState NewStatus`
- `OnObjectiveProgress`: A multicast delegate that broadcasts when an objective's progress is updated.
  - **Parameters**: `FName QuestID`, `FName ObjectiveID`, `int32 NewProgress`

## Example Usage
To start a new quest, first generate it and then have the player accept it.

```cpp
// In some gameplay event or dialogue trigger
UQuestSystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSystem>();
if (QuestSystem)
{
    // Generate a new quest
    FName NewQuestID = QuestSystem->GenerateQuest(EQuestType::Economic, FName("PlayerRegion"), 1);
    
    // Accept the quest
    if (NewQuestID != NAME_None)
    {
        QuestSystem->AcceptQuest(NewQuestID);
    }
}
```

To listen for quest completion in the UI:
```cpp
// In a UI widget's Initialize method
UQuestSystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSystem>();
if (QuestSystem)
{
    QuestSystem->OnQuestStatusChanged.AddDynamic(this, &UMyQuestWidget::HandleQuestStatusChanged);
}

// The handler function
void UMyQuestWidget::HandleQuestStatusChanged(FName QuestID, EQuestState NewStatus)
{
    if (NewStatus == EQuestState::Completed)
    {
        // Show completion message, update UI, etc.
        ShowCompletionNotification(QuestID);
    }
}
```

## Integration Points
- **UEconomySubsystem**: Used to generate economic-based quests (e.g., resource gathering, delivery).
- **UFactionManagerSubsystem**: Used to generate quests related to faction standing and political intrigue.
- **UI Widgets**: The system's delegates (`OnQuestStatusChanged`, `OnObjectiveProgress`) are crucial for updating the Quest Log, notifications, and other HUD elements.

## Best Practices
- Always check the return values of functions like `AcceptQuest` or `CompleteQuest` to handle cases where the operation might fail (e.g., quest ID not found).
- Bind to the delegates (`OnQuestStatusChanged`, `OnObjectiveProgress`) in UI or other gameplay systems to create reactive and decoupled code. Avoid polling the quest status every frame.

## Common Pitfalls
- Forgetting to call `UpdateObjectiveProgress` when a player performs an action that should count towards a quest.
- Trying to operate on a `QuestID` that no longer exists or has not been generated yet. Always validate the ID if possible.
