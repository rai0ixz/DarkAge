# DAQuestLogWidget

**Class:** `UDAQuestLogWidget`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/UI/SubWidgets/DAQuestLogWidget.h`  
**Implementation:** `Source/DarkAge/Private/UI/SubWidgets/DAQuestLogWidget.cpp`

## Overview

The `UDAQuestLogWidget` is a comprehensive UI widget that displays the player's active, completed, and failed quests. It provides an organized view of quest objectives, progress tracking, and detailed quest information with an intuitive interface for quest management.

## Key Features

- **Quest Categorization**: Separate tabs for Active, Completed, and Failed quests
- **Real-time Updates**: Automatic refresh when quest status changes
- **Detailed Quest View**: Expandable quest entries with full descriptions
- **Progress Tracking**: Visual progress bars and objective completion status
- **Quest Filtering**: Search and filter functionality for large quest lists
- **Dynamic Sorting**: Configurable sorting by priority, date, or quest type

## Widget Structure

### Main Components
```cpp
// Primary quest list container
UPROPERTY(meta = (BindWidget))
class UScrollBox* QuestListScrollBox;

// Tab navigation for quest categories
UPROPERTY(meta = (BindWidget))
class UHorizontalBox* QuestTabsContainer;

// Selected quest details panel
UPROPERTY(meta = (BindWidget))
class UVerticalBox* QuestDetailsPanel;

// Quest search functionality
UPROPERTY(meta = (BindWidget))
class UEditableTextBox* QuestSearchBox;
```

### Quest Entry Widget
```cpp
// Individual quest entry widget class
UPROPERTY(EditDefaultsOnly, Category = "Quest UI")
TSubclassOf<class UDAQuestEntryWidget> QuestEntryWidgetClass;

// Currently selected quest for detailed view
UPROPERTY(BlueprintReadOnly, Category = "Quest UI")
class UQuestComponent* SelectedQuest;
```

## Core Functionality

### Quest Display Management
```cpp
// Refresh the entire quest log display
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void RefreshQuestLog();

// Update display for a specific quest
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void UpdateQuestDisplay(class UQuestComponent* Quest);

// Filter quests based on search criteria
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void FilterQuests(const FString& SearchText);
```

### Quest Interaction
```cpp
// Select a quest for detailed viewing
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void SelectQuest(class UQuestComponent* Quest);

// Track/untrack a quest in the HUD
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void ToggleQuestTracking(class UQuestComponent* Quest);

// Abandon an active quest (with confirmation)
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void AbandonQuest(class UQuestComponent* Quest);
```

### Navigation and Filtering
```cpp
// Switch between quest categories
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void SetActiveTab(EQuestCategory Category);

// Sort quests by specified criteria
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void SortQuests(EQuestSortType SortType);

// Clear search filters
UFUNCTION(BlueprintCallable, Category = "Quest UI")
void ClearFilters();
```

## Quest Categories

### EQuestCategory
```cpp
UENUM(BlueprintType)
enum class EQuestCategory : uint8
{
    Active      UMETA(DisplayName = "Active Quests"),
    Completed   UMETA(DisplayName = "Completed Quests"),
    Failed      UMETA(DisplayName = "Failed Quests"),
    All         UMETA(DisplayName = "All Quests")
};
```

### EQuestSortType
```cpp
UENUM(BlueprintType)
enum class EQuestSortType : uint8
{
    Priority    UMETA(DisplayName = "Sort by Priority"),
    Date        UMETA(DisplayName = "Sort by Date"),
    Name        UMETA(DisplayName = "Sort by Name"),
    Progress    UMETA(DisplayName = "Sort by Progress"),
    Reward      UMETA(DisplayName = "Sort by Reward")
};
```

## Quest Entry Display

### Quest Information Layout
```cpp
// Quest basic information
struct FQuestDisplayInfo
{
    FText QuestTitle;
    FText QuestGiver;
    FText ShortDescription;
    EQuestPriority Priority;
    float CompletionPercentage;
    FDateTime DateReceived;
    bool bIsTracked;
    TArray<FText> ObjectiveTexts;
    TArray<bool> ObjectiveCompletionStates;
};
```

### Visual Elements
- **Priority Indicators**: Color-coded borders and icons
- **Progress Bars**: Visual completion percentage for multi-objective quests
- **Tracking Icons**: Show which quests are actively tracked in HUD
- **Reward Preview**: Icons and text for quest rewards
- **Time Stamps**: Quest received/completed dates

## Integration Points

### Quest System
```cpp
// Connect to quest system for updates
void ConnectToQuestSystem();

// Handle quest status change events
UFUNCTION()
void OnQuestStatusChanged(class UQuestComponent* Quest, EQuestStatus NewStatus);

// Handle objective completion events
UFUNCTION()
void OnObjectiveCompleted(class UQuestComponent* Quest, int32 ObjectiveIndex);
```

### Global Event Bus
- Listens for: `QuestStarted`, `QuestCompleted`, `QuestFailed`, `ObjectiveUpdated`
- Broadcasts: `QuestSelected`, `QuestAbandoned`, `QuestTrackingToggled`

## User Interaction

### Keyboard Shortcuts
- **Tab**: Cycle through quest categories
- **Enter**: Select highlighted quest
- **Delete**: Abandon selected quest (with confirmation)
- **F**: Toggle quest tracking
- **Ctrl+F**: Focus search box

### Mouse Interaction
- **Left Click**: Select quest
- **Right Click**: Context menu (Track/Abandon/Details)
- **Double Click**: Toggle tracking
- **Mouse Wheel**: Scroll through quest list

## Configuration

### UI Layout Settings
```cpp
// Maximum quests displayed per page
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest UI Config")
int32 MaxQuestsPerPage = 10;

// Auto-refresh interval for quest updates
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest UI Config")
float AutoRefreshInterval = 1.0f;

// Show completed quests in main list
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest UI Config")
bool bShowCompletedQuests = true;
```

### Visual Customization
```cpp
// Quest priority color schemes
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest UI Appearance")
TMap<EQuestPriority, FLinearColor> PriorityColors;

// Quest entry widget styling
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest UI Appearance")
FQuestEntryStyle DefaultQuestEntryStyle;
```

## Performance Optimization

- **Lazy Loading**: Quest details loaded only when selected
- **Virtual Scrolling**: Only visible quest entries are rendered
- **Update Batching**: Multiple quest changes processed in single frame
- **Caching**: Quest display data cached to reduce calculations

## Usage Examples

### Opening Quest Log
```cpp
// Show quest log widget
if (UDAQuestLogWidget* QuestLog = CreateWidget<UDAQuestLogWidget>(this, QuestLogWidgetClass))
{
    QuestLog->AddToViewport();
    QuestLog->RefreshQuestLog();
    QuestLog->SetActiveTab(EQuestCategory::Active);
}
```

### Tracking a Quest
```cpp
// Enable quest tracking from quest log
void UDAQuestLogWidget::OnTrackButtonClicked()
{
    if (SelectedQuest)
    {
        ToggleQuestTracking(SelectedQuest);
        
        // Update HUD to show quest objectives
        if (ADAHUD* GameHUD = Cast<ADAHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
        {
            GameHUD->UpdateTrackedQuest(SelectedQuest);
        }
    }
}
```

## Accessibility Features

- **Screen Reader Support**: Full text-to-speech compatibility
- **High Contrast Mode**: Alternative color schemes for visibility
- **Font Scaling**: Adjustable text size for readability
- **Keyboard Navigation**: Complete functionality without mouse

## See Also

- [QuestSystem](QuestSystem.md) - Core quest management
- [UDAInventoryWidget](UDAInventoryWidget.md) - Related UI patterns
- [ADAHUD](ADAHUD.md) - HUD quest tracking integration