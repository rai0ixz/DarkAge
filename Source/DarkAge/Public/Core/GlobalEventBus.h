#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "GlobalEventBus.generated.h"

// Forward declarations
class UFactionManagerSubsystem;
class UWorldPersistenceSystem;
class UWorldEcosystemSubsystem;

/**
 * Event Types for the Global Event Bus
 */
UENUM(BlueprintType)
enum class EGlobalEventType : uint8
{
    // Player Events
    PlayerCommittedCrime        UMETA(DisplayName = "Player Committed Crime"),
    PlayerCompletedQuest        UMETA(DisplayName = "Player Completed Quest"),
    PlayerEnteredRegion         UMETA(DisplayName = "Player Entered Region"),
    PlayerLeftRegion            UMETA(DisplayName = "Player Left Region"),
    PlayerDied                  UMETA(DisplayName = "Player Died"),
    PlayerLevelUp               UMETA(DisplayName = "Player Level Up"),
    
    // Faction Events
    FactionWarDeclared          UMETA(DisplayName = "Faction War Declared"),
    FactionAllianceFormed       UMETA(DisplayName = "Faction Alliance Formed"),
    FactionPeaceNegotiated      UMETA(DisplayName = "Faction Peace Negotiated"),
    FactionReputationChanged    UMETA(DisplayName = "Faction Reputation Changed"),
    FactionLeadershipChanged    UMETA(DisplayName = "Faction Leadership Changed"),
    
    // World Events
    WorldStateChanged           UMETA(DisplayName = "World State Changed"),
    WorldSaveRequested          UMETA(DisplayName = "World Save Requested"),
    WorldSaveCompleted          UMETA(DisplayName = "World Save Completed"),
    WorldLoadRequested          UMETA(DisplayName = "World Load Requested"),
    WorldLoadCompleted          UMETA(DisplayName = "World Load Completed"),
    
    // Ecosystem Events
    SeasonChanged               UMETA(DisplayName = "Season Changed"),
    WeatherChanged              UMETA(DisplayName = "Weather Changed"),
    ResourceDepleted            UMETA(DisplayName = "Resource Depleted"),
    ResourceDiscovered          UMETA(DisplayName = "Resource Discovered"),
    AnimalPopulationChanged     UMETA(DisplayName = "Animal Population Changed"),
    EnvironmentalEventTriggered UMETA(DisplayName = "Environmental Event Triggered"),
    
    // NPC Events
    NPCRoutineChanged           UMETA(DisplayName = "NPC Routine Changed"),
    NPCMoraleChanged            UMETA(DisplayName = "NPC Morale Changed"),
    NPCMemoryAdded              UMETA(DisplayName = "NPC Memory Added"),
    NPCDied                     UMETA(DisplayName = "NPC Died"),
    NPCSpawned                  UMETA(DisplayName = "NPC Spawned"),
    
    // Combat Events
    CombatStarted               UMETA(DisplayName = "Combat Started"),
    CombatEnded                 UMETA(DisplayName = "Combat Ended"),
    DragonSighted               UMETA(DisplayName = "Dragon Sighted"),
    
    // Economy Events
    TradeCompleted              UMETA(DisplayName = "Trade Completed"),
    MarketPriceChanged          UMETA(DisplayName = "Market Price Changed"),
    
    // Quest Events
    QuestStarted                UMETA(DisplayName = "Quest Started"),
    QuestCompleted              UMETA(DisplayName = "Quest Completed"),
    QuestFailed                 UMETA(DisplayName = "Quest Failed"),
    
    // Custom Events
    Custom                      UMETA(DisplayName = "Custom Event")
};

/**
 * Priority levels for event processing
 */
UENUM(BlueprintType)
enum class EEventPriority : uint8
{
    Low         UMETA(DisplayName = "Low"),
    Normal      UMETA(DisplayName = "Normal"),
    High        UMETA(DisplayName = "High"),
    Critical    UMETA(DisplayName = "Critical")
};

/**
 * Base structure for all global events
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FGlobalEvent
{
    GENERATED_BODY()

    // Event type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    EGlobalEventType EventType = EGlobalEventType::Custom;

    // Event priority
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    EEventPriority Priority = EEventPriority::Normal;

    // Timestamp when event was created
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    float Timestamp = 0.0f;

    // Source of the event (optional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FString Source = TEXT("");

    // Target of the event (optional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FString Target = TEXT("");

    // Event data as key-value pairs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    TMap<FString, FString> EventData;

    // Unique event ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FGuid EventID;

    // Whether this event should be processed immediately
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    bool bImmediate = false;

    FORCEINLINE FGlobalEvent()
        : EventType(EGlobalEventType::Custom)
        , Priority(EEventPriority::Normal)
        , Timestamp(0.0f)
        , Source(TEXT(""))
        , Target(TEXT(""))
        , EventID()
        , bImmediate(false)
    {
    }

    FORCEINLINE FGlobalEvent(EGlobalEventType InEventType, EEventPriority InPriority = EEventPriority::Normal, bool bInImmediate = false)
        : EventType(InEventType)
        , Priority(InPriority)
        , Timestamp(0.0f)
        , Source(TEXT(""))
        , Target(TEXT(""))
        , EventID(FGuid::NewGuid())
        , bImmediate(bInImmediate)
    {
    }
};

/**
 * Delegate for event listeners
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalEvent, const FGlobalEvent&, Event);

/**
 * Interface for objects that can listen to global events
 */
UINTERFACE(BlueprintType)
class DARKAGE_API UGlobalEventListener : public UInterface
{
    GENERATED_BODY()
};

class DARKAGE_API IGlobalEventListener
{
    GENERATED_BODY()

public:
    // Called when a global event is received
    UFUNCTION(BlueprintImplementableEvent, Category = "Global Events")
    void OnGlobalEventReceived(const FGlobalEvent& Event);

    // Get the event types this listener is interested in
    virtual TArray<EGlobalEventType> GetListenedEventTypes() const { return TArray<EGlobalEventType>(); }

    // Get the priority of this listener (higher priority listeners are called first)
    virtual int32 GetListenerPriority() const { return 0; }
};

/**
 * Global Event Bus Subsystem
 * 
 * This system provides a centralized event messaging system that allows
 * different subsystems to communicate without direct dependencies.
 * 
 * Benefits:
 * - Decouples systems from each other
 * - Makes emergent gameplay easier to implement
 * - Simplifies debugging and testing
 * - Allows for easy addition/removal of systems
 */
UCLASS()
class DARKAGE_API UGlobalEventBus : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UGlobalEventBus();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    // Tick function to process queued events
    void Tick(float DeltaTime);

    // --- Event Broadcasting ---
    
    /**
     * Broadcast an event to all registered listeners
     * @param Event The event to broadcast
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    void BroadcastEvent(const FGlobalEvent& Event);

    /**
     * Broadcast an event with simple parameters
     * @param EventType Type of event
     * @param Source Source of the event
     * @param Target Target of the event (optional)
     * @param EventData Additional data
     * @param Priority Event priority
     * @param bImmediate Whether to process immediately
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    void BroadcastSimpleEvent(EGlobalEventType EventType, const FString& Source,
        const FString& Target, const TMap<FString, FString>& EventData,
        EEventPriority Priority = EEventPriority::Normal, bool bImmediate = false);

    /**
     * Broadcast an event with simple parameters (no event data)
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    void BroadcastSimpleEventNoData(EGlobalEventType EventType, const FString& Source,
        const FString& Target = TEXT(""), EEventPriority Priority = EEventPriority::Normal, bool bImmediate = false);

    // --- Event Listening ---

    /**
     * Register a listener for specific event types
     * @param Listener The object implementing IGlobalEventListener
     * @param EventTypes Array of event types to listen for (empty = all events)
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    void RegisterListener(TScriptInterface<IGlobalEventListener> Listener, const TArray<EGlobalEventType>& EventTypes);

    /**
     * Unregister a listener
     * @param Listener The listener to unregister
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    void UnregisterListener(TScriptInterface<IGlobalEventListener> Listener);

    /**
     * Register a delegate for a specific event type
     * @param EventType The event type to listen for
     * @param Delegate The delegate to call when the event occurs
     */
    void RegisterDelegate(EGlobalEventType EventType, FOnGlobalEvent& Delegate);

    /**
     * Unregister a delegate
     * @param EventType The event type
     * @param Delegate The delegate to unregister
     */
    void UnregisterDelegate(EGlobalEventType EventType, FOnGlobalEvent& Delegate);

    // --- Event History and Analytics ---

    /**
     * Get recent events of a specific type
     * @param EventType The type of events to retrieve
     * @param MaxCount Maximum number of events to return
     * @return Array of recent events
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    TArray<FGlobalEvent> GetRecentEvents(EGlobalEventType EventType, int32 MaxCount = 10) const;

    /**
     * Get all recent events
     * @param MaxCount Maximum number of events to return
     * @return Array of recent events
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    TArray<FGlobalEvent> GetAllRecentEvents(int32 MaxCount = 50) const;

    /**
     * Get event count for a specific type
     * @param EventType The event type to count
     * @return Number of events of this type
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events")
    int32 GetEventCount(EGlobalEventType EventType) const;

    // --- Debug and Admin Functions ---

    /**
     * Enable/disable event logging
     * @param bEnabled Whether to enable logging
     */
    UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
    void SetEventLogging(bool bEnabled);

    /**
     * Print recent events to log
     * @param EventType Specific event type (Custom = all types)
     * @param Count Number of events to print
     */
    UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
    void PrintRecentEvents(EGlobalEventType EventType = EGlobalEventType::Custom, int32 Count = 10);

    /**
     * Clear event history
     */
    UFUNCTION(BlueprintCallable, Exec, Category = "Global Events|Debug")
    void ClearEventHistory();

    /**
     * Get statistics about the event system
     */
    UFUNCTION(BlueprintCallable, Category = "Global Events|Debug")
    FString GetEventSystemStats() const;

    // Static helper to get the global event bus instance
    static UGlobalEventBus* Get(const UObject* WorldContext);

protected:
    // Process a single event
    void ProcessGlobalEvent(const FGlobalEvent& Event);

    // Sort events by priority
    void SortEventsByPriority(TArray<FGlobalEvent>& Events);

private:
    // Event queue for non-immediate events
    TArray<FGlobalEvent> EventQueue;

    // Event history for analytics
    TArray<FGlobalEvent> EventHistory;

    // Maximum number of events to keep in history
    int32 MaxHistorySize;

    // Registered listeners by event type
    TMap<EGlobalEventType, TArray<TWeakObjectPtr<UObject>>> EventListeners;

    // Registered delegates by event type
    TMap<EGlobalEventType, TArray<FOnGlobalEvent*>> EventDelegates;

    // Event statistics
    TMap<EGlobalEventType, int32> EventCounts;

    // Debug settings
    bool bLogEvents;
    bool bLogEventProcessing;

    // Performance tracking
    float TotalProcessingTime;
    int32 TotalEventsProcessed;

    // Helper functions
    void AddToHistory(const FGlobalEvent& Event);
    void TrimHistory();
    FString EventTypeToString(EGlobalEventType EventType) const;
    FString PriorityToString(EEventPriority Priority) const;
};