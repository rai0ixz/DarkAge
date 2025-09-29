#include "Core/GlobalEventBus.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/DateTime.h"

UGlobalEventBus::UGlobalEventBus()
    : MaxHistorySize(1000)
    , bLogEvents(false)
    , bLogEventProcessing(false)
    , TotalProcessingTime(0.0f)
    , TotalEventsProcessed(0)
{
}

void UGlobalEventBus::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Initialized"));
    
    // Clear any existing data
    EventQueue.Empty();
    EventHistory.Empty();
    EventListeners.Empty();
    EventDelegates.Empty();
    EventCounts.Empty();
    
    // Initialize event counts for all event types
    for (int32 i = 0; i < static_cast<int32>(EGlobalEventType::Custom) + 1; ++i)
    {
        EventCounts.Add(static_cast<EGlobalEventType>(i), 0);
    }
    
    bLogEvents = true; // Enable logging by default in development
}

void UGlobalEventBus::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Deinitializing"));
    
    // Clear all data
    EventQueue.Empty();
    EventHistory.Empty();
    EventListeners.Empty();
    EventDelegates.Empty();
    EventCounts.Empty();
    
    Super::Deinitialize();
}

void UGlobalEventBus::Tick(float DeltaTime)
{
    if (EventQueue.Num() == 0)
    {
        return;
    }
    
    // Sort events by priority
    SortEventsByPriority(EventQueue);
    
    // Process all queued events
    TArray<FGlobalEvent> EventsToProcess = EventQueue;
    EventQueue.Empty();
    
    float StartTime = FPlatformTime::Seconds();
    
    for (const FGlobalEvent& Event : EventsToProcess)
    {
        ProcessGlobalEvent(Event);
    }
    
    float ProcessingTime = FPlatformTime::Seconds() - StartTime;
    TotalProcessingTime += ProcessingTime;
    TotalEventsProcessed += EventsToProcess.Num();
    
    if (bLogEventProcessing && EventsToProcess.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Processed %d events in %.4f seconds"), 
            EventsToProcess.Num(), ProcessingTime);
    }
}

void UGlobalEventBus::BroadcastEvent(const FGlobalEvent& Event)
{
    // Create a copy with timestamp
    FGlobalEvent TimestampedEvent = Event;
    TimestampedEvent.Timestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : FPlatformTime::Seconds();
    
    if (bLogEvents)
    {
        UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Broadcasting event %s from %s to %s"), 
            *EventTypeToString(TimestampedEvent.EventType),
            *TimestampedEvent.Source,
            TimestampedEvent.Target.IsEmpty() ? TEXT("ALL") : *TimestampedEvent.Target);
    }
    
    // Add to history
    AddToHistory(TimestampedEvent);
    
    // Update statistics
    EventCounts.FindOrAdd(TimestampedEvent.EventType)++;
    
    if (TimestampedEvent.bImmediate)
    {
        // Process immediately
        ProcessGlobalEvent(TimestampedEvent);
    }
    else
    {
        // Add to queue for next tick
        EventQueue.Add(TimestampedEvent);
    }
}

void UGlobalEventBus::BroadcastSimpleEvent(EGlobalEventType EventType, const FString& Source,
    const FString& Target, const TMap<FString, FString>& EventData,
    EEventPriority Priority, bool bImmediate)
{
    FGlobalEvent Event(EventType, Priority, bImmediate);
    Event.Source = Source;
    Event.Target = Target;
    Event.EventData = EventData;
    
    BroadcastEvent(Event);
}

void UGlobalEventBus::BroadcastSimpleEventNoData(EGlobalEventType EventType, const FString& Source,
    const FString& Target, EEventPriority Priority, bool bImmediate)
{
    TMap<FString, FString> EmptyEventData;
    BroadcastSimpleEvent(EventType, Source, Target, EmptyEventData, Priority, bImmediate);
}

void UGlobalEventBus::RegisterListener(TScriptInterface<IGlobalEventListener> Listener, const TArray<EGlobalEventType>& EventTypes)
{
    if (!Listener.GetInterface() || !Listener.GetObject())
    {
        UE_LOG(LogTemp, Warning, TEXT("GlobalEventBus: Attempted to register null listener"));
        return;
    }
    
    TArray<EGlobalEventType> ListenTypes = EventTypes;
    
    // If no specific types provided, get them from the listener
    if (ListenTypes.Num() == 0)
    {
        ListenTypes = Listener.GetInterface()->GetListenedEventTypes();
    }
    
    // If still no types, register for all events
    if (ListenTypes.Num() == 0)
    {
        for (int32 i = 0; i < static_cast<int32>(EGlobalEventType::Custom) + 1; ++i)
        {
            ListenTypes.Add(static_cast<EGlobalEventType>(i));
        }
    }
    
    // Register for each event type
    for (EGlobalEventType EventType : ListenTypes)
    {
        TArray<TWeakObjectPtr<UObject>>& Listeners = EventListeners.FindOrAdd(EventType);
        
        // Check if already registered
        bool bAlreadyRegistered = false;
        for (const TWeakObjectPtr<UObject>& ExistingListener : Listeners)
        {
            if (ExistingListener.Get() == Listener.GetObject())
            {
                bAlreadyRegistered = true;
                break;
            }
        }
        
        if (!bAlreadyRegistered)
        {
            Listeners.Add(Listener.GetObject());
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Registered listener %s for %d event types"), 
        *Listener.GetObject()->GetName(), ListenTypes.Num());
}

void UGlobalEventBus::UnregisterListener(TScriptInterface<IGlobalEventListener> Listener)
{
    if (!Listener.GetInterface() || !Listener.GetObject())
    {
        return;
    }
    
    int32 RemovedCount = 0;
    
    // Remove from all event type lists
    for (auto& EventListenerPair : EventListeners)
    {
        TArray<TWeakObjectPtr<UObject>>& Listeners = EventListenerPair.Value;
        
        for (int32 i = Listeners.Num() - 1; i >= 0; --i)
        {
            if (!Listeners[i].IsValid() || Listeners[i].Get() == Listener.GetObject())
            {
                Listeners.RemoveAt(i);
                RemovedCount++;
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Unregistered listener %s from %d event types"), 
        *Listener.GetObject()->GetName(), RemovedCount);
}

void UGlobalEventBus::RegisterDelegate(EGlobalEventType EventType, FOnGlobalEvent& Delegate)
{
    TArray<FOnGlobalEvent*>& Delegates = EventDelegates.FindOrAdd(EventType);
    Delegates.Add(&Delegate);
    
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Registered delegate for event type %s"), 
        *EventTypeToString(EventType));
}

void UGlobalEventBus::UnregisterDelegate(EGlobalEventType EventType, FOnGlobalEvent& Delegate)
{
    if (TArray<FOnGlobalEvent*>* Delegates = EventDelegates.Find(EventType))
    {
        Delegates->Remove(&Delegate);
        
        UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Unregistered delegate for event type %s"), 
            *EventTypeToString(EventType));
    }
}

TArray<FGlobalEvent> UGlobalEventBus::GetRecentEvents(EGlobalEventType EventType, int32 MaxCount) const
{
    TArray<FGlobalEvent> FilteredEvents;
    
    // Search from most recent to oldest
    for (int32 i = EventHistory.Num() - 1; i >= 0 && FilteredEvents.Num() < MaxCount; --i)
    {
        if (EventHistory[i].EventType == EventType)
        {
            FilteredEvents.Add(EventHistory[i]);
        }
    }
    
    return FilteredEvents;
}

TArray<FGlobalEvent> UGlobalEventBus::GetAllRecentEvents(int32 MaxCount) const
{
    TArray<FGlobalEvent> RecentEvents;
    
    int32 StartIndex = FMath::Max(0, EventHistory.Num() - MaxCount);
    for (int32 i = StartIndex; i < EventHistory.Num(); ++i)
    {
        RecentEvents.Add(EventHistory[i]);
    }
    
    return RecentEvents;
}

int32 UGlobalEventBus::GetEventCount(EGlobalEventType EventType) const
{
    if (const int32* Count = EventCounts.Find(EventType))
    {
        return *Count;
    }
    return 0;
}

void UGlobalEventBus::SetEventLogging(bool bEnabled)
{
    bLogEvents = bEnabled;
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Event logging %s"), 
        bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UGlobalEventBus::PrintRecentEvents(EGlobalEventType EventType, int32 Count)
{
    TArray<FGlobalEvent> Events;
    
    if (EventType == EGlobalEventType::Custom)
    {
        Events = GetAllRecentEvents(Count);
    }
    else
    {
        Events = GetRecentEvents(EventType, Count);
    }
    
    UE_LOG(LogTemp, Log, TEXT("=== Recent Events (%d) ==="), Events.Num());
    
    for (const FGlobalEvent& Event : Events)
    {
        UE_LOG(LogTemp, Log, TEXT("[%.2f] %s: %s -> %s (Priority: %s)"), 
            Event.Timestamp,
            *EventTypeToString(Event.EventType),
            *Event.Source,
            Event.Target.IsEmpty() ? TEXT("ALL") : *Event.Target,
            *PriorityToString(Event.Priority));
        
        // Print event data if any
        for (const auto& DataPair : Event.EventData)
        {
            UE_LOG(LogTemp, Log, TEXT("  %s: %s"), *DataPair.Key, *DataPair.Value);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("=== End Recent Events ==="));
}

void UGlobalEventBus::ClearEventHistory()
{
    EventHistory.Empty();
    EventCounts.Empty();
    
    // Reinitialize event counts
    for (int32 i = 0; i < static_cast<int32>(EGlobalEventType::Custom) + 1; ++i)
    {
        EventCounts.Add(static_cast<EGlobalEventType>(i), 0);
    }
    
    UE_LOG(LogTemp, Log, TEXT("GlobalEventBus: Event history cleared"));
}

FString UGlobalEventBus::GetEventSystemStats() const
{
    FString Stats;
    Stats += FString::Printf(TEXT("=== Global Event Bus Statistics ===\n"));
    Stats += FString::Printf(TEXT("Events in Queue: %d\n"), EventQueue.Num());
    Stats += FString::Printf(TEXT("Events in History: %d\n"), EventHistory.Num());
    Stats += FString::Printf(TEXT("Total Events Processed: %d\n"), TotalEventsProcessed);
    Stats += FString::Printf(TEXT("Total Processing Time: %.4f seconds\n"), TotalProcessingTime);
    
    if (TotalEventsProcessed > 0)
    {
        Stats += FString::Printf(TEXT("Average Processing Time: %.6f seconds per event\n"), 
            TotalProcessingTime / TotalEventsProcessed);
    }
    
    Stats += FString::Printf(TEXT("Registered Listeners: %d event types\n"), EventListeners.Num());
    Stats += FString::Printf(TEXT("Registered Delegates: %d event types\n"), EventDelegates.Num());
    
    Stats += TEXT("\n=== Event Counts by Type ===\n");
    for (const auto& CountPair : EventCounts)
    {
        if (CountPair.Value > 0)
        {
            Stats += FString::Printf(TEXT("%s: %d\n"), 
                *EventTypeToString(CountPair.Key), CountPair.Value);
        }
    }
    
    return Stats;
}

UGlobalEventBus* UGlobalEventBus::Get(const UObject* WorldContext)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UGlobalEventBus>();
        }
    }
    return nullptr;
}

void UGlobalEventBus::ProcessGlobalEvent(const FGlobalEvent& Event)
{
    // Process listeners for this specific event type
    if (TArray<TWeakObjectPtr<UObject>>* Listeners = EventListeners.Find(Event.EventType))
    {
        // Clean up invalid listeners while processing
        for (int32 i = Listeners->Num() - 1; i >= 0; --i)
        {
            TWeakObjectPtr<UObject>& ListenerPtr = (*Listeners)[i];
            
            if (!ListenerPtr.IsValid())
            {
                Listeners->RemoveAt(i);
                continue;
            }
            
            if (UObject* ListenerObject = ListenerPtr.Get())
            {
                if (IGlobalEventListener* Listener = Cast<IGlobalEventListener>(ListenerObject))
                {
                    // Call the Blueprint implementable event
                    Listener->Execute_OnGlobalEventReceived(ListenerObject, Event);
                }
            }
        }
    }
    
    // Process delegates for this event type
    if (TArray<FOnGlobalEvent*>* Delegates = EventDelegates.Find(Event.EventType))
    {
        for (FOnGlobalEvent* Delegate : *Delegates)
        {
            if (Delegate && Delegate->IsBound())
            {
                Delegate->Broadcast(Event);
            }
        }
    }
    
    if (bLogEventProcessing)
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("GlobalEventBus: Processed event %s"), 
            *EventTypeToString(Event.EventType));
    }
}

void UGlobalEventBus::SortEventsByPriority(TArray<FGlobalEvent>& Events)
{
    Events.Sort([](const FGlobalEvent& A, const FGlobalEvent& B)
    {
        // Higher priority values are processed first
        return static_cast<int32>(A.Priority) > static_cast<int32>(B.Priority);
    });
}

void UGlobalEventBus::AddToHistory(const FGlobalEvent& Event)
{
    EventHistory.Add(Event);
    TrimHistory();
}

void UGlobalEventBus::TrimHistory()
{
    if (EventHistory.Num() > MaxHistorySize)
    {
        int32 ElementsToRemove = EventHistory.Num() - MaxHistorySize;
        EventHistory.RemoveAt(0, ElementsToRemove);
    }
}

FString UGlobalEventBus::EventTypeToString(EGlobalEventType EventType) const
{
    switch (EventType)
    {
        case EGlobalEventType::PlayerCommittedCrime: return TEXT("PlayerCommittedCrime");
        case EGlobalEventType::PlayerCompletedQuest: return TEXT("PlayerCompletedQuest");
        case EGlobalEventType::PlayerEnteredRegion: return TEXT("PlayerEnteredRegion");
        case EGlobalEventType::PlayerLeftRegion: return TEXT("PlayerLeftRegion");
        case EGlobalEventType::PlayerDied: return TEXT("PlayerDied");
        case EGlobalEventType::PlayerLevelUp: return TEXT("PlayerLevelUp");
        case EGlobalEventType::FactionWarDeclared: return TEXT("FactionWarDeclared");
        case EGlobalEventType::FactionAllianceFormed: return TEXT("FactionAllianceFormed");
        case EGlobalEventType::FactionPeaceNegotiated: return TEXT("FactionPeaceNegotiated");
        case EGlobalEventType::FactionReputationChanged: return TEXT("FactionReputationChanged");
        case EGlobalEventType::FactionLeadershipChanged: return TEXT("FactionLeadershipChanged");
        case EGlobalEventType::WorldStateChanged: return TEXT("WorldStateChanged");
        case EGlobalEventType::WorldSaveRequested: return TEXT("WorldSaveRequested");
        case EGlobalEventType::WorldSaveCompleted: return TEXT("WorldSaveCompleted");
        case EGlobalEventType::WorldLoadRequested: return TEXT("WorldLoadRequested");
        case EGlobalEventType::WorldLoadCompleted: return TEXT("WorldLoadCompleted");
        case EGlobalEventType::SeasonChanged: return TEXT("SeasonChanged");
        case EGlobalEventType::WeatherChanged: return TEXT("WeatherChanged");
        case EGlobalEventType::ResourceDepleted: return TEXT("ResourceDepleted");
        case EGlobalEventType::ResourceDiscovered: return TEXT("ResourceDiscovered");
        case EGlobalEventType::AnimalPopulationChanged: return TEXT("AnimalPopulationChanged");
        case EGlobalEventType::EnvironmentalEventTriggered: return TEXT("EnvironmentalEventTriggered");
        case EGlobalEventType::NPCRoutineChanged: return TEXT("NPCRoutineChanged");
        case EGlobalEventType::NPCMoraleChanged: return TEXT("NPCMoraleChanged");
        case EGlobalEventType::NPCMemoryAdded: return TEXT("NPCMemoryAdded");
        case EGlobalEventType::NPCDied: return TEXT("NPCDied");
        case EGlobalEventType::NPCSpawned: return TEXT("NPCSpawned");
        case EGlobalEventType::CombatStarted: return TEXT("CombatStarted");
        case EGlobalEventType::CombatEnded: return TEXT("CombatEnded");
        case EGlobalEventType::DragonSighted: return TEXT("DragonSighted");
        case EGlobalEventType::TradeCompleted: return TEXT("TradeCompleted");
        case EGlobalEventType::MarketPriceChanged: return TEXT("MarketPriceChanged");
        case EGlobalEventType::QuestStarted: return TEXT("QuestStarted");
        case EGlobalEventType::QuestCompleted: return TEXT("QuestCompleted");
        case EGlobalEventType::QuestFailed: return TEXT("QuestFailed");
        case EGlobalEventType::Custom: return TEXT("Custom");
        default: return TEXT("Unknown");
    }
}

FString UGlobalEventBus::PriorityToString(EEventPriority Priority) const
{
    switch (Priority)
    {
        case EEventPriority::Low: return TEXT("Low");
        case EEventPriority::Normal: return TEXT("Normal");
        case EEventPriority::High: return TEXT("High");
        case EEventPriority::Critical: return TEXT("Critical");
        default: return TEXT("Unknown");
    }
}