# AIMemoryComponent

**Class:** `UAIMemoryComponent`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Components/AIMemoryComponent.h`  
**Implementation:** `Source/DarkAge/Private/Components/AIMemoryComponent.cpp`

## Overview

The `UAIMemoryComponent` provides NPCs with sophisticated memory systems, allowing them to remember player interactions, witness events, form opinions, and make decisions based on past experiences. This creates more believable and reactive AI characters that enhance immersion and emergent storytelling.

## Key Features

- **Event Memory**: NPCs remember witnessed crimes, kindnesses, and significant events
- **Personal Opinions**: Individual AI opinions separate from official faction standings
- **Relationship Tracking**: Dynamic relationship building based on interactions
- **Witness System**: NPCs can testify about events they witnessed
- **Memory Decay**: Realistic forgetting of older, less significant events
- **Emotional Context**: Memories tagged with emotional significance

## Core Functionality

### Memory Management
```cpp
// Add new memory entry
UFUNCTION(BlueprintCallable, Category = "AI Memory")
void AddMemory(const FAIMemoryEntry& MemoryEntry);

// Retrieve memories by type
UFUNCTION(BlueprintPure, Category = "AI Memory")
TArray<FAIMemoryEntry> GetMemoriesByType(EAIMemoryType MemoryType) const;

// Check if AI remembers specific event
UFUNCTION(BlueprintPure, Category = "AI Memory")
bool RemembersEvent(FGuid EventID) const;

// Get memory of specific actor
UFUNCTION(BlueprintPure, Category = "AI Memory")
FAIMemoryEntry GetMemoryOfActor(AActor* Actor) const;
```

### Opinion System
```cpp
// Get AI's personal opinion of actor (separate from faction reputation)
UFUNCTION(BlueprintPure, Category = "AI Memory")
float GetPersonalOpinion(AActor* Actor) const;

// Modify opinion based on witnessed behavior
UFUNCTION(BlueprintCallable, Category = "AI Memory")
void ModifyPersonalOpinion(AActor* Actor, float Delta, const FString& Reason);

// Get relationship tier based on opinion
UFUNCTION(BlueprintPure, Category = "AI Memory")
EPersonalRelationship GetPersonalRelationship(AActor* Actor) const;
```

### Witness System
```cpp
// Record witnessed event
UFUNCTION(BlueprintCallable, Category = "AI Memory")
void WitnessEvent(const FWitnessedEvent& Event);

// Check if AI would testify about event
UFUNCTION(BlueprintPure, Category = "AI Memory")
bool WouldTestifyAbout(FGuid EventID, AActor* Accused) const;

// Get witness testimony for crime
UFUNCTION(BlueprintPure, Category = "AI Memory")
FWitnessTestimony GetTestimony(FGuid EventID) const;
```

## Data Structures

### FAIMemoryEntry
```cpp
USTRUCT(BlueprintType)
struct FAIMemoryEntry
{
    GENERATED_BODY()

    // Memory identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid MemoryID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAIMemoryType MemoryType;

    // Memory content
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText MemoryDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> InvolvedActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector EventLocation;

    // Emotional context
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEmotionalValence EmotionalValence; // Positive, Negative, Neutral

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EmotionalIntensity = 1.0f;

    // Timing and decay
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime MemoryTimestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MemoryStrength = 1.0f; // Decays over time

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ImportanceLevel = 1.0f; // Affects decay rate

    // Opinion impact
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float OpinionModifier = 0.0f;

    // Verification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsWitnessedEvent = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WitnessReliability = 1.0f; // How clearly they witnessed it
};
```

### EAIMemoryType
```cpp
UENUM(BlueprintType)
enum class EAIMemoryType : uint8
{
    PersonalInteraction,    // Direct conversation or interaction
    WitnessedCrime,        // Saw player commit crime
    WitnessedKindness,     // Saw player help someone
    SharedExperience,      // Mutual participation in event
    HeardRumor,           // Second-hand information
    TradeExperience,      // Commercial interaction
    CombatEncounter,      // Fought with/against player
    QuestInteraction,     // Quest-related memory
    SocialEvent,          // Party, gathering, celebration
    EmotionalEvent        // Wedding, funeral, significant life event
};
```

### FWitnessedEvent
```cpp
USTRUCT(BlueprintType)
struct FWitnessedEvent
{
    GENERATED_BODY()

    // Event details
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid EventID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGlobalEventType EventType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> PrimaryActor; // Usually the player

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> SecondaryActor; // Victim, target, etc.

    // Witness details
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WitnessDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WitnessClarity; // How well they saw it (lighting, obstructions, etc.)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WitnessAttention; // How focused they were on the event

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDirectWitness; // Saw it directly vs heard commotion

    // Context
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector EventLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime EventTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> EventDetails; // Specific details they remember
};
```

## Memory Decay System

### Decay Mechanics
```cpp
// Process memory decay over time
void ProcessMemoryDecay(float DeltaTime);

// Calculate decay rate based on memory importance
float CalculateDecayRate(const FAIMemoryEntry& Memory) const;

// Reinforce memory when similar events occur
void ReinforceMemory(FGuid MemoryID, float ReinforcementStrength);
```

### Memory Consolidation
```cpp
// Merge similar memories to prevent memory bloat
void ConsolidateMemories();

// Check if two memories are similar enough to merge
bool AreMemoriesSimilar(const FAIMemoryEntry& MemoryA, const FAIMemoryEntry& MemoryB) const;

// Create consolidated memory from multiple related memories
FAIMemoryEntry CreateConsolidatedMemory(const TArray<FAIMemoryEntry>& Memories) const;
```

## Personality Integration

### Personality Influence on Memory
```cpp
// Personality traits affect what NPCs remember and how
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Personality")
float MemoryRetention = 1.0f; // How well they remember things

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Personality")
float EmotionalSensitivity = 1.0f; // How much emotions affect memory

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Personality")
bool bGrudgeHolder = false; // Remembers negative events longer

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Personality")
bool bForgiving = false; // Negative memories decay faster
```

## Crime Witness System

### Witness Reliability
```cpp
// Calculate how reliable this NPC is as a witness
float CalculateWitnessReliability(const FWitnessedEvent& Event) const;

// Factors affecting reliability:
// - Distance from event
// - Lighting conditions
// - NPC's attention at the time
// - NPC's personality traits (honest, observant, etc.)
// - Time elapsed since event
```

### Testimony Generation
```cpp
USTRUCT(BlueprintType)
struct FWitnessTestimony
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText TestimonyText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ConfidenceLevel; // How confident they are in their account

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> SpecificDetails; // Details they remember

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> UncertainDetails; // Things they're not sure about

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WitnessCredibility; // How trustworthy this witness is

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWillingToTestify; // Will they actually speak up?
};
```

## Integration with Other Systems

### Faction System Separation
```cpp
// Personal opinion modifiers that work with official faction reputation
float GetEffectiveReputation(AActor* Actor, FName FactionName) const
{
    // Get official faction reputation
    float OfficialRep = GetFactionReputation(Actor, FactionName);
    
    // Apply personal opinion modifier
    float PersonalMod = GetPersonalOpinion(Actor);
    
    // Combine based on NPC personality
    return CombineReputationAndOpinion(OfficialRep, PersonalMod);
}
```

### Global Event Bus Integration
```cpp
// Listen for events to create memories
virtual void OnGlobalEventReceived_Implementation(const FGlobalEvent& Event) override
{
    // Determine if this NPC would have witnessed/heard about the event
    if (ShouldRememberEvent(Event))
    {
        CreateMemoryFromEvent(Event);
    }
}
```

### Dialogue System Integration
```cpp
// Memories influence dialogue options and NPC responses
TArray<FDialogueOption> GetAvailableDialogueOptions(AActor* Player) const
{
    TArray<FDialogueOption> Options;
    
    // Check memories for conversation topics
    TArray<FAIMemoryEntry> RelevantMemories = GetMemoriesOfActor(Player);
    
    for (const FAIMemoryEntry& Memory : RelevantMemories)
    {
        if (Memory.MemoryStrength > 0.5f) // Strong enough to mention
        {
            Options.Add(CreateDialogueFromMemory(Memory));
        }
    }
    
    return Options;
}
```

## Performance Optimization

### Memory Limits
```cpp
// Maximum memories per category to prevent bloat
UPROPERTY(EditAnywhere, Category = "AI Memory Config")
int32 MaxMemoriesPerType = 50;

// Memory cleanup threshold
UPROPERTY(EditAnywhere, Category = "AI Memory Config")
float MinMemoryStrengthToKeep = 0.1f;

// Update frequency for memory processing
UPROPERTY(EditAnywhere, Category = "AI Memory Config")
float MemoryUpdateInterval = 5.0f;
```

### Smart Culling
```cpp
// Intelligent memory removal prioritizing importance
void CullWeakMemories();

// Keep memories that are:
// - Recent (< 1 day old)
// - High importance (> 0.8)
// - About the player
// - Crime-related
// - Emotionally significant
```

## Usage Examples

### Crime Witnessing
```cpp
// Player steals something, nearby NPC witnesses it
void ProcessCrimeWitness(AActor* Criminal, AActor* Victim, ECrimeType CrimeType)
{
    // Find nearby NPCs who could witness
    TArray<AActor*> NearbyNPCs = GetNearbyNPCs(Criminal->GetActorLocation(), 1000.0f);
    
    for (AActor* NPC : NearbyNPCs)
    {
        if (UAIMemoryComponent* Memory = NPC->FindComponentByClass<UAIMemoryComponent>())
        {
            // Create witnessed event
            FWitnessedEvent WitnessEvent;
            WitnessEvent.EventType = EGlobalEventType::PlayerCommittedCrime;
            WitnessEvent.PrimaryActor = Criminal;
            WitnessEvent.SecondaryActor = Victim;
            WitnessEvent.WitnessDistance = FVector::Dist(NPC->GetActorLocation(), Criminal->GetActorLocation());
            WitnessEvent.WitnessClarity = CalculateWitnessClarity(NPC, Criminal);
            
            Memory->WitnessEvent(WitnessEvent);
            
            // Also affects personal opinion
            Memory->ModifyPersonalOpinion(Criminal, -10.0f, TEXT("Witnessed Theft"));
        }
    }
}
```

### Dialogue Integration
```cpp
// NPC mentions past interaction
FText GetGreetingBasedOnMemory(AActor* Player)
{
    if (UAIMemoryComponent* Memory = GetOwner()->FindComponentByClass<UAIMemoryComponent>())
    {
        float Opinion = Memory->GetPersonalOpinion(Player);
        
        TArray<FAIMemoryEntry> RecentMemories = Memory->GetRecentMemoriesOfActor(Player, 7.0f); // Last week
        
        if (RecentMemories.Num() > 0)
        {
            FAIMemoryEntry& LastMemory = RecentMemories[0];
            
            if (LastMemory.MemoryType == EAIMemoryType::WitnessedKindness)
            {
                return FText::FromString("I saw what you did for that family last week. You're a good person.");
            }
            else if (LastMemory.MemoryType == EAIMemoryType::WitnessedCrime)
            {
                return FText::FromString("I know what you did. Stay away from me.");
            }
        }
    }
    
    return FText::FromString("Hello there.");
}
```

## See Also

- [FactionReputationComponent](FactionReputationComponent.md) - Official faction standings
- [DialogueComponent](DialogueComponent.md) - Conversation system integration
- [CrimeManagerSubsystem](CrimeManagerSubsystem.md) - Crime witness system
- [NPCEcosystemSubsystem](NPCEcosystemSubsystem.md) - NPC behavior coordination