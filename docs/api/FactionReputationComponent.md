# FactionReputationComponent

**Class:** `UFactionReputationComponent`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Components/FactionReputationComponent.h`  
**Implementation:** `Source/DarkAge/Private/Components/FactionReputationComponent.cpp`

## Overview

The `UFactionReputationComponent` manages an actor's reputation and standing with various factions in the Dark Age world. It tracks reputation values, relationship modifiers, and handles reputation changes due to player actions and world events.

## Key Features

- **Multi-Faction Tracking**: Maintains reputation with all game factions simultaneously
- **Dynamic Reputation**: Real-time reputation changes based on actions and events
- **Relationship Tiers**: Converts numerical reputation to meaningful relationship levels
- **Event Integration**: Responds to Global Event Bus for reputation-affecting events
- **Save/Load Support**: Persistent reputation data across game sessions

## Core Functionality

### Reputation Management
```cpp
// Get current reputation with a faction
UFUNCTION(BlueprintPure, Category = "Faction Reputation")
float GetFactionReputation(FName FactionName) const;

// Modify reputation with a faction
UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
void ModifyFactionReputation(FName FactionName, float Delta, const FString& Reason = TEXT(""));

// Set absolute reputation value
UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
void SetFactionReputation(FName FactionName, float NewReputation);
```

### Relationship Queries
```cpp
// Get relationship tier (Enemy, Hostile, Neutral, Friendly, Allied)
UFUNCTION(BlueprintPure, Category = "Faction Reputation")
ERelationshipTier GetRelationshipTier(FName FactionName) const;

// Check if faction relationship meets minimum requirement
UFUNCTION(BlueprintPure, Category = "Faction Reputation")
bool HasMinimumReputation(FName FactionName, ERelationshipTier MinimumTier) const;

// Get all factions and their current reputation
UFUNCTION(BlueprintPure, Category = "Faction Reputation")
TMap<FName, float> GetAllReputations() const;
```

## Data Structures

### ERelationshipTier
```cpp
UENUM(BlueprintType)
enum class ERelationshipTier : uint8
{
    Enemy       = 0,    // -100 to -60
    Hostile     = 1,    // -59 to -20
    Neutral     = 2,    // -19 to +19
    Friendly    = 3,    // +20 to +59
    Allied      = 4     // +60 to +100
};
```

### FFactionReputationEntry
```cpp
USTRUCT(BlueprintType)
struct FFactionReputationEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FactionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Reputation = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastModified;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LastReason;
};
```

## Reputation System Mechanics

### Reputation Ranges
- **Enemy (-100 to -60)**: Faction will attack on sight, no trading, bounties placed
- **Hostile (-59 to -20)**: Aggressive dialogue, restricted access, higher prices
- **Neutral (-19 to +19)**: Standard interactions, normal prices
- **Friendly (+20 to +59)**: Positive dialogue, discounts, quest opportunities
- **Allied (+60 to +100)**: Full access, significant discounts, exclusive quests

### Reputation Decay
```cpp
// Configure reputation decay over time (optional)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reputation Config")
float ReputationDecayRate = 0.1f; // Per day

// Enable/disable reputation decay
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reputation Config")
bool bEnableReputationDecay = false;
```

## Integration Points

### Global Event Bus
- Listens for: `PlayerCommittedCrime`, `PlayerCompletedQuest`, `FactionWarDeclared`
- Broadcasts: `FactionReputationChanged`, `RelationshipTierChanged`

### Related Systems
- **FactionManagerSubsystem**: Central faction data and relationships
- **CrimeManagerSubsystem**: Criminal actions affecting reputation
- **QuestSystem**: Quest completion reputation rewards

## Event Handling

### Crime Impact
```cpp
// Automatic reputation loss for criminal actions
void OnPlayerCommittedCrime(const FGlobalEvent& Event)
{
    FName CrimeType = FName(*Event.EventData[TEXT("CrimeType")]);
    
    if (CrimeType == TEXT("Theft"))
    {
        ModifyFactionReputation(TEXT("Merchants"), -5.0f, TEXT("Theft"));
        ModifyFactionReputation(TEXT("Guards"), -10.0f, TEXT("Criminal Activity"));
    }
}
```

### Quest Rewards
```cpp
// Reputation bonuses for quest completion
void OnQuestCompleted(const FGlobalEvent& Event)
{
    FName QuestGiverFaction = FName(*Event.EventData[TEXT("GiverFaction")]);
    float ReputationReward = FCString::Atof(*Event.EventData[TEXT("ReputationReward")]);
    
    ModifyFactionReputation(QuestGiverFaction, ReputationReward, TEXT("Quest Completion"));
}
```

## Configuration

Faction reputation parameters are configured via Data Tables:
- `FactionReputationData` - Base reputation values and tier thresholds
- `ReputationModifierData` - Action-based reputation changes
- `FactionRelationshipData` - Inter-faction relationship effects

## Usage Examples

### Check Access Permission
```cpp
if (UFactionReputationComponent* RepComp = Actor->FindComponentByClass<UFactionReputationComponent>())
{
    ERelationshipTier Relationship = RepComp->GetRelationshipTier(TEXT("Merchants"));
    
    if (Relationship >= ERelationshipTier::Neutral)
    {
        // Allow trading
        OpenTradeInterface();
    }
    else
    {
        // Refuse service
        ShowHostileDialogue();
    }
}
```

### Apply Crime Penalty
```cpp
// Player caught stealing
RepComp->ModifyFactionReputation(TEXT("Guards"), -15.0f, TEXT("Caught Stealing"));
RepComp->ModifyFactionReputation(TEXT("Merchants"), -5.0f, TEXT("Criminal Activity"));

// Check if player is now hostile
if (RepComp->GetRelationshipTier(TEXT("Guards")) <= ERelationshipTier::Hostile)
{
    TriggerGuardPursuit();
}
```

## Replication

The component supports network replication for multiplayer:
```cpp
virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

UPROPERTY(Replicated)
TArray<FFactionReputationEntry> ReplicatedReputations;
```

## Performance Considerations

- Reputation calculations are cached and only updated when values change
- Event processing is batched to avoid frame rate spikes
- Large reputation changes (>10 points) are applied over multiple frames

## See Also

- [FactionManagerSubsystem](FactionManagerSubsystem.md) - Central faction management
- [CrimeManagerSubsystem](CrimeManagerSubsystem.md) - Criminal activity tracking
- [NotorietyComponent](NotorietyComponent.md) - Crime-specific reputation system