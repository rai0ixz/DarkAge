# DialogueComponent

**Class:** `UDialogueComponent`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Components/DialogueComponent.h`  
**Implementation:** `Source/DarkAge/Private/Components/DialogueComponent.cpp`

## Overview

The `UDialogueComponent` manages NPC dialogue interactions, providing a sophisticated conversation system that integrates with memory, faction reputation, and player choices. It supports branching dialogue trees, conditional responses, and dynamic content generation based on game state.

## Key Features

- **Branching Dialogue Trees**: Complex conversation flows with multiple paths
- **Dynamic Responses**: Dialogue adapts to faction reputation, memories, and world state
- **Memory Integration**: NPCs reference past interactions and witnessed events
- **Emotional Context**: Dialogue reflects NPC mood and relationship with player
- **Quest Integration**: Conversations can start, advance, or complete quests
- **Localization Support**: Full text localization for multiple languages

## Core Functionality

### Dialogue Management
```cpp
// Start conversation with NPC
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void StartDialogue(AActor* Speaker, AActor* Listener);

// Process player dialogue choice
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void SelectDialogueOption(int32 OptionIndex);

// End current conversation
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void EndDialogue();

// Check if NPC is currently in dialogue
UFUNCTION(BlueprintPure, Category = "Dialogue")
bool IsInDialogue() const;
```

### Dialogue Tree Navigation
```cpp
// Get current dialogue node
UFUNCTION(BlueprintPure, Category = "Dialogue")
FDialogueNode GetCurrentNode() const;

// Get available response options
UFUNCTION(BlueprintPure, Category = "Dialogue")
TArray<FDialogueOption> GetAvailableOptions() const;

// Navigate to specific dialogue node
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void GotoDialogueNode(FName NodeID);
```

### Dynamic Content
```cpp
// Generate contextual dialogue based on game state
UFUNCTION(BlueprintCallable, Category = "Dialogue")
TArray<FDialogueOption> GenerateContextualDialogue(AActor* Player);

// Update dialogue based on recent events
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void RefreshDialogueContext();

// Check if dialogue option should be available
UFUNCTION(BlueprintPure, Category = "Dialogue")
bool IsDialogueOptionAvailable(const FDialogueCondition& Condition) const;
```

## Data Structures

### FDialogueNode
```cpp
USTRUCT(BlueprintType)
struct FDialogueNode
{
    GENERATED_BODY()

    // Node identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NodeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText NPCText; // What the NPC says

    // Dialogue flow
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueOption> PlayerOptions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NextNodeID; // Auto-advance to this node if no player options

    // Conditions and requirements
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueCondition> DisplayConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bOnlyShowOnce = false;

    // Audio and presentation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* VoiceClip;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueMood NPCMood = EDialogueMood::Neutral;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DisplayDuration = 0.0f; // 0 = wait for player input

    // Actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueAction> OnEnterActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueAction> OnExitActions;
};
```

### FDialogueOption
```cpp
USTRUCT(BlueprintType)
struct FDialogueOption
{
    GENERATED_BODY()

    // Option content
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText OptionText; // What the player says

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText HoverText; // Additional info shown on hover

    // Navigation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetNodeID; // Where this option leads

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEndsDialogue = false;

    // Conditions
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueCondition> Requirements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bOnlyShowOnce = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasBeenUsed = false;

    // Visual presentation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueOptionType OptionType = EDialogueOptionType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor TextColor = FLinearColor::White;

    // Actions and consequences
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueAction> OnSelectActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ReputationModifier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AffectedFaction;
};
```

### FDialogueCondition
```cpp
USTRUCT(BlueprintType)
struct FDialogueCondition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueConditionType ConditionType;

    // Target for condition check
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TargetValue;

    // Comparison operation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EComparisonOperator Operator = EComparisonOperator::Equal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ComparisonValue = 0.0f;

    // Logic
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bInvertCondition = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EConditionLogic LogicOperator = EConditionLogic::And;
};
```

## Dialogue Condition Types

### EDialogueConditionType
```cpp
UENUM(BlueprintType)
enum class EDialogueConditionType : uint8
{
    FactionReputation,      // Check reputation with specific faction
    PersonalOpinion,        // Check AI's personal opinion of player
    HasItem,               // Player has specific item
    CompletedQuest,        // Quest completion status
    HasMemory,             // NPC remembers specific event
    PlayerLevel,           // Player character level
    TimeOfDay,             // Current game time
    Season,                // Current season
    Location,              // Current location/region
    PlayerClass,           // Player's character class
    NPCMood,               // NPC's current mood
    WorldState,            // Global world state variable
    CustomFlag             // Custom boolean flag
};
```

### EDialogueOptionType
```cpp
UENUM(BlueprintType)
enum class EDialogueOptionType : uint8
{
    Normal,                // Standard dialogue option
    Aggressive,            // Threatening or hostile response
    Persuasive,           // Charisma-based option
    Informative,          // Asks for information
    Trade,                // Opens trade interface
    Quest,                // Quest-related option
    Goodbye,              // Ends conversation politely
    Memory,               // References shared memory
    Faction,              // Faction-specific option
    Special               // Unique/rare option
};
```

## Memory-Driven Dialogue

### Integration with AIMemoryComponent
```cpp
// Generate dialogue options based on NPC's memories
TArray<FDialogueOption> GenerateMemoryBasedOptions(AActor* Player)
{
    TArray<FDialogueOption> Options;
    
    if (UAIMemoryComponent* Memory = GetOwner()->FindComponentByClass<UAIMemoryComponent>())
    {
        // Get recent memories of the player
        TArray<FAIMemoryEntry> PlayerMemories = Memory->GetMemoriesOfActor(Player);
        
        for (const FAIMemoryEntry& MemoryEntry : PlayerMemories)
        {
            if (MemoryEntry.MemoryStrength > 0.5f && ShouldMentionMemory(MemoryEntry))
            {
                FDialogueOption MemoryOption = CreateDialogueFromMemory(MemoryEntry);
                MemoryOption.OptionType = EDialogueOptionType::Memory;
                Options.Add(MemoryOption);
            }
        }
    }
    
    return Options;
}

// Create dialogue option from specific memory
FDialogueOption CreateDialogueFromMemory(const FAIMemoryEntry& Memory)
{
    FDialogueOption Option;
    
    switch (Memory.MemoryType)
    {
        case EAIMemoryType::WitnessedCrime:
            Option.OptionText = FText::FromString("I know what you did...");
            Option.HoverText = FText::FromString("Confront them about the crime you witnessed");
            break;
            
        case EAIMemoryType::WitnessedKindness:
            Option.OptionText = FText::FromString("Thank you for your kindness.");
            Option.HoverText = FText::FromString("Express gratitude for their past help");
            break;
            
        case EAIMemoryType::TradeExperience:
            Option.OptionText = FText::FromString("I remember our last trade...");
            Option.HoverText = FText::FromString("Reference past business dealings");
            break;
    }
    
    return Option;
}
```

## Dynamic Mood System

### EDialogueMood
```cpp
UENUM(BlueprintType)
enum class EDialogueMood : uint8
{
    Hostile,              // Angry, aggressive dialogue
    Suspicious,           // Wary, distrustful responses
    Neutral,              // Standard, polite conversation
    Friendly,             // Warm, welcoming dialogue
    Enthusiastic,         // Excited, energetic responses
    Sad,                  // Melancholy, subdued dialogue
    Fearful,              // Nervous, anxious responses
    Drunk,                // Altered dialogue patterns
    Sick,                 // Weakened, tired responses
    Romantic              // Flirtatious or affectionate
};
```

### Mood-Based Dialogue Modification
```cpp
// Modify dialogue text based on NPC's current mood
FText ModifyDialogueForMood(FText BaseText, EDialogueMood Mood)
{
    FString DialogueString = BaseText.ToString();
    
    switch (Mood)
    {
        case EDialogueMood::Drunk:
            // Add slurred speech patterns
            DialogueString = DialogueString.Replace(TEXT("s"), TEXT("sh"));
            DialogueString += TEXT(" *hiccup*");
            break;
            
        case EDialogueMood::Fearful:
            // Add nervous speech patterns
            DialogueString = TEXT("I... ") + DialogueString + TEXT("... if that's okay...");
            break;
            
        case EDialogueMood::Hostile:
            // Make dialogue more aggressive
            DialogueString = DialogueString.ToUpper();
            DialogueString += TEXT("!");
            break;
    }
    
    return FText::FromString(DialogueString);
}
```

## Quest Integration

### Quest-Related Dialogue
```cpp
// Generate quest-related dialogue options
TArray<FDialogueOption> GenerateQuestOptions(AActor* Player)
{
    TArray<FDialogueOption> QuestOptions;
    
    if (UQuestSystem* QuestSys = GetGameInstance()->GetSubsystem<UQuestSystem>())
    {
        // Check for available quests
        TArray<UQuestComponent*> AvailableQuests = QuestSys->GetAvailableQuestsForNPC(GetOwner());
        
        for (UQuestComponent* Quest : AvailableQuests)
        {
            FDialogueOption QuestOption;
            QuestOption.OptionText = Quest->GetQuestTitle();
            QuestOption.OptionType = EDialogueOptionType::Quest;
            QuestOption.OnSelectActions.Add(CreateStartQuestAction(Quest));
            
            QuestOptions.Add(QuestOption);
        }
        
        // Check for quest turn-ins
        TArray<UQuestComponent*> CompletableQuests = QuestSys->GetCompletableQuestsForNPC(GetOwner());
        
        for (UQuestComponent* Quest : CompletableQuests)
        {
            FDialogueOption TurnInOption;
            TurnInOption.OptionText = FText::Format(
                FText::FromString("I've completed '{0}'"),
                Quest->GetQuestTitle()
            );
            TurnInOption.OptionType = EDialogueOptionType::Quest;
            TurnInOption.OnSelectActions.Add(CreateCompleteQuestAction(Quest));
            
            QuestOptions.Add(TurnInOption);
        }
    }
    
    return QuestOptions;
}
```

## Dialogue Actions

### FDialogueAction
```cpp
USTRUCT(BlueprintType)
struct FDialogueAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueActionType ActionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ActionTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ActionValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> ActionParameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bExecuteOnServer = true;
};

UENUM(BlueprintType)
enum class EDialogueActionType : uint8
{
    ModifyReputation,       // Change faction reputation
    GiveItem,              // Add item to player inventory
    TakeItem,              // Remove item from player inventory
    StartQuest,            // Begin new quest
    CompleteQuest,         // Complete existing quest
    PlaySound,             // Play audio clip
    SpawnActor,            // Create actor in world
    TeleportPlayer,        // Move player to location
    ModifyWorldState,      // Change global variable
    TriggerEvent,          // Send global event
    OpenTrade,             // Start trading interface
    OpenCrafting,          // Start crafting interface
    AddMemory,             // Create AI memory
    SetFlag,               // Set boolean flag
    Custom                 // Execute custom Blueprint event
};
```

## Localization Support

### Text Management
```cpp
// Localized text handling
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
FText LocalizedDialogueText;

// Dynamic text formatting with variables
FText FormatDialogueText(FText BaseText, const TMap<FString, FString>& Variables)
{
    FString FormattedText = BaseText.ToString();
    
    for (const auto& Variable : Variables)
    {
        FString SearchString = FString::Printf(TEXT("{%s}"), *Variable.Key);
        FormattedText = FormattedText.Replace(*SearchString, *Variable.Value);
    }
    
    return FText::FromString(FormattedText);
}
```

## Performance Optimization

### Dialogue Caching
```cpp
// Cache frequently used dialogue trees
UPROPERTY(EditAnywhere, Category = "Performance")
TMap<FName, FDialogueTree> CachedDialogueTrees;

// Preload common dialogue responses
UPROPERTY(EditAnywhere, Category = "Performance")
bool bPreloadCommonDialogue = true;

// Limit number of dynamic options generated
UPROPERTY(EditAnywhere, Category = "Performance")
int32 MaxDynamicOptions = 10;
```

## Usage Examples

### Basic Dialogue Interaction
```cpp
// Start conversation when player interacts with NPC
void ANPC::OnPlayerInteract(AActor* Player)
{
    if (UDialogueComponent* Dialogue = FindComponentByClass<UDialogueComponent>())
    {
        // Load appropriate dialogue tree based on NPC state
        FName DialogueTreeID = DetermineDialogueTree(Player);
        
        Dialogue->LoadDialogueTree(DialogueTreeID);
        Dialogue->StartDialogue(this, Player);
    }
}

// Determine which dialogue tree to use
FName ANPC::DetermineDialogueTree(AActor* Player)
{
    // Check if this is first meeting
    if (UAIMemoryComponent* Memory = FindComponentByClass<UAIMemoryComponent>())
    {
        if (!Memory->HasMemoryOfActor(Player))
        {
            return TEXT("FirstMeeting");
        }
    }
    
    // Check faction relationships
    if (UFactionReputationComponent* FactionRep = Player->FindComponentByClass<UFactionReputationComponent>())
    {
        float Reputation = FactionRep->GetFactionReputation(MyFactionName);
        
        if (Reputation < -50.0f)
        {
            return TEXT("HostileDialogue");
        }
        else if (Reputation > 50.0f)
        {
            return TEXT("FriendlyDialogue");
        }
    }
    
    return TEXT("StandardDialogue");
}
```

## See Also

- [AIMemoryComponent](AIMemoryComponent.md) - Memory-driven dialogue content
- [FactionReputationComponent](FactionReputationComponent.md) - Reputation-based responses
- [QuestSystem](QuestSystem.md) - Quest integration
- [NPCEcosystemSubsystem](NPCEcosystemSubsystem.md) - NPC coordination