# GlobalEventBus

**Class:** `UGlobalEventBus`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Core/GlobalEventBus.h`  
**Implementation:** `Source/DarkAge/Private/Core/GlobalEventBus.cpp`

## Overview

The `UGlobalEventBus` is the central nervous system of Dark Age, providing a decoupled communication mechanism for all game subsystems. It enables emergent gameplay by allowing systems to react to events throughout the game world without tight coupling between components.

## Key Features

- **Decoupled Architecture**: Systems communicate without direct dependencies
- **Priority-Based Processing**: Critical events processed before lower-priority ones
- **Event History**: Complete audit trail for debugging and analytics
- **Performance Monitoring**: Built-in profiling for event system overhead
- **Network Support**: Events can be replicated across multiplayer connections
- **Flexible Filtering**: Listeners can subscribe to specific event types

## Core Architecture

### Event Types
The system supports over 30 different event types covering all major game domains:

#### Player Events
- `PlayerCommittedCrime` - Criminal actions affecting reputation
- `PlayerCompletedQuest` - Quest completion with faction rewards
- `PlayerLevelUp` - Character progression events
- `PlayerDied` - Death events triggering consequences

#### World Events
- `SeasonChanged` - Environmental shifts affecting gameplay
- `ResourceDepleted` - Economic scarcity events
- `EnvironmentalEventTriggered` - Natural disasters and weather

#### Political Events
- `FactionWarDeclared` - Major political conflicts
- `FactionAllianceFormed` - Diplomatic developments
- `FactionReputationChanged` - Relationship changes

#### Economic Events
- `MarketPriceChanged` - Economic fluctuations
- `TradeCompleted` - Commercial transactions

## Core Functionality

### Event Broadcasting
```cpp
// Broadcast an event to all listeners
UFUNCTION(BlueprintCallable, Category = "Global Events")
void BroadcastEvent(const FGlobalEvent& Event);

// Simplified event broadcasting
UFUNCTION(BlueprintCallable, Category = "Global Events")
void BroadcastSimpleEvent(EGlobalEventType EventType, const FString& Source,
    const FString& Target = TEXT(""), EEventPriority Priority = EEventPriority::Normal);
```

### Event Listening
```cpp
// Register a listener for specific event types
UFUNCTION(BlueprintCallable, Category = "Global Events")
void RegisterListener(TScriptInterface<IGlobalEventListener> Listener, 
    const TArray<EGlobalEventType>& EventTypes);

// Unregister a listener
UFUNCTION(BlueprintCallable, Category = "Global Events")
void UnregisterListener(TScriptInterface<IGlobalEventListener> Listener);

// Register delegate for specific event type
void RegisterDelegate(EGlobalEventType EventType, FOnGlobalEvent& Delegate);
```

### Event Analytics
```cpp
// Get recent events of specific type
UFUNCTION(BlueprintCallable, Category = "Global Events")
TArray<FGlobalEvent> GetRecentEvents(EGlobalEventType EventType, int32 MaxCount = 10) const;

// Get event count statistics
UFUNCTION(BlueprintCallable, Category = "Global Events")
int32 GetEventCount(EGlobalEventType EventType) const;

// Get comprehensive system statistics
UFUNCTION(BlueprintCallable, Category = "Global Events")
FString GetEventSystemStats() const;
```

## Data Structures

### FGlobalEvent
```cpp
USTRUCT(BlueprintType)
struct DARKAGE_API FGlobalEvent
{
    GENERATED_BODY()

    // Event identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGlobalEventType EventType = EGlobalEventType::Custom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEventPriority Priority = EEventPriority::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid EventID;

    // Event context
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Source = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Target = TEXT("");

    // Event data as key-value pairs
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> EventData;

    // Timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Timestamp = 0.0f;

    // Processing options
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bImmediate = false;
};
```

### Event Priority System
```cpp
UENUM(BlueprintType)
enum class EEventPriority : uint8
{
    Low         UMETA(DisplayName = "Low"),        // UI updates, cosmetic effects
    Normal      UMETA(DisplayName = "Normal"),     // Standard gameplay events
    High        UMETA(DisplayName = "High"),       // Player actions, quest updates
    Critical    UMETA(DisplayName = "Critical")    // Death, faction wars, major events
};
```

## Event Processing

### Tick-Based Processing
The system processes events during the game's tick cycle, with priority-based ordering:

1. **Critical Priority**: Death events, faction wars, emergency stops
2. **High Priority**: Player actions, quest updates, combat events
3. **Normal Priority**: Standard gameplay events, NPC behavior
4. **Low Priority**: UI updates, cosmetic effects, analytics

### Immediate vs Queued Events
```cpp
// Immediate events process synchronously
FGlobalEvent ImmediateEvent(EGlobalEventType::PlayerDied, EEventPriority::Critical, true);
EventBus->BroadcastEvent(ImmediateEvent);

// Queued events process during next tick
FGlobalEvent QueuedEvent(EGlobalEventType::NPCRoutineChanged, EEventPriority::Low, false);
EventBus->BroadcastEvent(QueuedEvent);
```

## Integration Patterns

### Subsystem Integration
```cpp
// Standard subsystem event handling pattern
class UAdvancedSurvivalSubsystem : public UGameInstanceSubsystem, public IGlobalEventListener
{
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);
        
        // Register for relevant events
        if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
        {
            TArray<EGlobalEventType> ListenEvents = {
                EGlobalEventType::SeasonChanged,
                EGlobalEventType::WeatherChanged,
                EGlobalEventType::PlayerDied
            };
            EventBus->RegisterListener(this, ListenEvents);
        }
    }

    // Implement event response
    virtual void OnGlobalEventReceived_Implementation(const FGlobalEvent& Event) override
    {
        switch (Event.EventType)
        {
            case EGlobalEventType::SeasonChanged:
                ProcessSeasonalSurvivalChanges(Event);
                break;
            case EGlobalEventType::WeatherChanged:
                ProcessWeatherEffects(Event);
                break;
        }
    }
};
```

### Component Integration
```cpp
// Components can also listen to events
class UStatlineComponent : public UActorComponent, public IGlobalEventListener
{
public:
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        
        if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
        {
            EventBus->RegisterListener(this, {EGlobalEventType::StatusEffectApplied});
        }
    }

    virtual void OnGlobalEventReceived_Implementation(const FGlobalEvent& Event) override
    {
        if (Event.EventType == EGlobalEventType::StatusEffectApplied)
        {
            // Handle status effect application
            ProcessStatusEffectEvent(Event);
        }
    }
};
```

## Event Examples

### Crime System Integration
```cpp
// Player commits theft
void UCrimeManagerSubsystem::OnPlayerStealsItem(AActor* Player, AActor* Victim)
{
    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
    {
        FGlobalEvent CrimeEvent(EGlobalEventType::PlayerCommittedCrime, EEventPriority::High);
        CrimeEvent.Source = Player->GetName();
        CrimeEvent.Target = Victim->GetName();
        CrimeEvent.EventData.Add(TEXT("CrimeType"), TEXT("Theft"));
        CrimeEvent.EventData.Add(TEXT("Location"), Player->GetActorLocation().ToString());
        
        EventBus->BroadcastEvent(CrimeEvent);
    }
}

// Faction system responds to crime
void UFactionManagerSubsystem::OnGlobalEventReceived_Implementation(const FGlobalEvent& Event)
{
    if (Event.EventType == EGlobalEventType::PlayerCommittedCrime)
    {
        FString CrimeType = Event.EventData[TEXT("CrimeType")];
        
        if (CrimeType == TEXT("Theft"))
        {
            // Reduce reputation with merchant factions
            ModifyPlayerReputation(TEXT("Merchants"), -5.0f, TEXT("Criminal Activity"));
            ModifyPlayerReputation(TEXT("Guards"), -10.0f, TEXT("Law Breaking"));
        }
    }
}
```

### Economic Ripple Effects
```cpp
// Economy system broadcasts price change
void UEconomySubsystem::UpdateItemPrice(EItemType ItemType, float NewPrice)
{
    SetItemPrice(ItemType, NewPrice);
    
    // Broadcast price change event
    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
    {
        FGlobalEvent PriceEvent(EGlobalEventType::MarketPriceChanged, EEventPriority::Normal);
        PriceEvent.EventData.Add(TEXT("ItemType"), UEnum::GetValueAsString(ItemType));
        PriceEvent.EventData.Add(TEXT("NewPrice"), FString::SanitizeFloat(NewPrice));
        
        EventBus->BroadcastEvent(PriceEvent);
    }
}

// NPC system responds to price changes
void UNPCEcosystemSubsystem::OnGlobalEventReceived_Implementation(const FGlobalEvent& Event)
{
    if (Event.EventType == EGlobalEventType::MarketPriceChanged)
    {
        // Update NPC merchant behaviors based on new prices
        UpdateMerchantStrategies(Event);
        
        // Generate quests if price changes create opportunities
        CheckForTradeOpportunityQuests(Event);
    }
}
```

## Performance Monitoring

### Built-in Profiling
```cpp
// Enable performance tracking
UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
void SetEventLogging(bool bEnabled);

// Get performance statistics
FString UGlobalEventBus::GetEventSystemStats() const
{
    return FString::Printf(TEXT(
        "Event System Statistics:\n"
        "Total Events Processed: %d\n"
        "Average Processing Time: %.3f ms\n"
        "Events in Queue: %d\n"
        "Active Listeners: %d\n"
        "Memory Usage: %.2f KB"
    ), TotalEventsProcessed, TotalProcessingTime, EventQueue.Num(), 
       GetTotalListenerCount(), GetMemoryUsage());
}
```

### Debug Commands
```cpp
// Console commands for debugging
UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
void PrintRecentEvents(EGlobalEventType EventType = EGlobalEventType::Custom, int32 Count = 10);

UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
void ClearEventHistory();
```

## Configuration

### Performance Settings
```cpp
// Maximum events in history (for memory management)
UPROPERTY(EditAnywhere, Category = "Event System")
int32 MaxHistorySize = 1000;

// Event processing batch size
UPROPERTY(EditAnywhere, Category = "Event System")
int32 EventProcessingBatchSize = 50;

// Enable/disable event logging
UPROPERTY(EditAnywhere, Category = "Event System")
bool bLogEvents = false;
```

## Static Access

```cpp
// Get global event bus instance from anywhere
static UGlobalEventBus* Get(const UObject* WorldContext);

// Usage example
if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
{
    EventBus->BroadcastSimpleEvent(EGlobalEventType::Custom, TEXT("MySystem"));
}
```

## Best Practices

1. **Use Specific Event Types**: Don't overuse `Custom` - create specific types for clarity
2. **Include Context**: Populate EventData with relevant information for listeners
3. **Mind Performance**: Use appropriate priority levels and avoid excessive immediate events
4. **Clean Registration**: Always unregister listeners in EndPlay/Deinitialize
5. **Event Data Validation**: Check EventData keys exist before accessing

## See Also

- [AdvancedSurvivalSubsystem](AdvancedSurvivalSubsystem.md) - Example event integration
- [FactionManagerSubsystem](FactionManagerSubsystem.md) - Political event handling
- [NPCEcosystemSubsystem](NPCEcosystemSubsystem.md) - NPC behavior events