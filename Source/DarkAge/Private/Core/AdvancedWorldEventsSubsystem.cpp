#include "Core/AdvancedWorldEventsSubsystem.h"
#include "Core/NPCEcosystemSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/AdvancedMarketSimulationSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedWorldEventsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeEventTemplates();
    InitializeEventChains();
    InitializeProbabilityModifiers();
    
    EventCheckTimer = 0.0f;
    NextEventID = 1;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedWorldEventsSubsystem initialized with %d event templates"), EventTemplates.Num());
}

void UAdvancedWorldEventsSubsystem::Tick(float DeltaTime)
{
    
    EventCheckTimer += DeltaTime;
    
    // Check for new events periodically
    if (EventCheckTimer >= BaseEventCheckInterval)
    {
        ProcessEventGeneration();
        EventCheckTimer = 0.0f;
    }
    
    // Update active events
    UpdateActiveEvents(DeltaTime);
    
    // Update event chains
    UpdateEventChains(DeltaTime);
}

void UAdvancedWorldEventsSubsystem::InitializeEventTemplates()
{
    // Natural Disaster Events
    FWorldEvent Earthquake;
    Earthquake.EventID = TEXT("Earthquake");
    Earthquake.EventName = TEXT("Devastating Earthquake");
    Earthquake.Description = TEXT("A powerful earthquake shakes the land, damaging structures and disrupting trade routes");
    Earthquake.EventType = EWorldEventType::Natural;
    Earthquake.Severity = EEventSeverity::Major;
    Earthquake.Scope = EEventScope::Regional;
    Earthquake.Duration = 300.0f; // 5 minutes
    Earthquake.AffectedRadius = 5000.0f;
    Earthquake.bHasCascadingEffects = true;
    Earthquake.CascadeProbability = 0.4f;
    
    FWorldEventEffect EarthquakeEffect;
    EarthquakeEffect.EffectType = TEXT("StructuralDamage");
    EarthquakeEffect.Magnitude = 0.7f;
    EarthquakeEffect.Duration = 1800.0f; // 30 minutes
    EarthquakeEffect.AffectedSystems.Add(TEXT("Economy"));
    EarthquakeEffect.AffectedSystems.Add(TEXT("NPCEcosystem"));
    EarthquakeEffect.AffectedSystems.Add(TEXT("Market"));
    EarthquakeEffect.SystemModifiers.Add(TEXT("TradeEfficiency"), -0.5f);
    EarthquakeEffect.SystemModifiers.Add(TEXT("PopulationMorale"), -0.3f);
    EarthquakeEffect.SystemModifiers.Add(TEXT("BuildingIntegrity"), -0.6f);
    Earthquake.Effects.Add(EarthquakeEffect);
    
    Earthquake.TriggerEvents.Add(TEXT("Aftershock"));
    Earthquake.TriggerEvents.Add(TEXT("Refugee Crisis"));
    EventTemplates.Add(TEXT("Earthquake"), Earthquake);
    
    // Political Events
    FWorldEvent CivilUnrest;
    CivilUnrest.EventID = TEXT("CivilUnrest");
    CivilUnrest.EventName = TEXT("Civil Unrest");
    CivilUnrest.Description = TEXT("Citizens rise up against local authorities, causing widespread disruption");
    CivilUnrest.EventType = EWorldEventType::Political;
    CivilUnrest.Severity = EEventSeverity::Moderate;
    CivilUnrest.Scope = EEventScope::Local;
    CivilUnrest.Duration = 600.0f; // 10 minutes
    CivilUnrest.AffectedRadius = 2000.0f;
    CivilUnrest.bHasCascadingEffects = true;
    CivilUnrest.CascadeProbability = 0.6f;
    
    FWorldEventEffect UnrestEffect;
    UnrestEffect.EffectType = TEXT("SocialDisorder");
    UnrestEffect.Magnitude = 0.8f;
    UnrestEffect.Duration = 900.0f; // 15 minutes
    UnrestEffect.AffectedSystems.Add(TEXT("Faction"));
    UnrestEffect.AffectedSystems.Add(TEXT("NPCEcosystem"));
    UnrestEffect.AffectedSystems.Add(TEXT("Economy"));
    UnrestEffect.SystemModifiers.Add(TEXT("FactionStability"), -0.4f);
    UnrestEffect.SystemModifiers.Add(TEXT("PublicOrder"), -0.7f);
    UnrestEffect.SystemModifiers.Add(TEXT("TradeVolume"), -0.3f);
    CivilUnrest.Effects.Add(UnrestEffect);
    
    CivilUnrest.TriggerEvents.Add(TEXT("Martial Law"));
    CivilUnrest.TriggerEvents.Add(TEXT("Economic Sanctions"));
    EventTemplates.Add(TEXT("CivilUnrest"), CivilUnrest);
    
    // Economic Events
    FWorldEvent MarketCrash;
    MarketCrash.EventID = TEXT("MarketCrash");
    MarketCrash.EventName = TEXT("Economic Collapse");
    MarketCrash.Description = TEXT("A sudden market crash devastates the regional economy");
    MarketCrash.EventType = EWorldEventType::Economic;
    MarketCrash.Severity = EEventSeverity::Critical;
    MarketCrash.Scope = EEventScope::National;
    MarketCrash.Duration = 1200.0f; // 20 minutes
    MarketCrash.AffectedRadius = 10000.0f;
    MarketCrash.bHasCascadingEffects = true;
    MarketCrash.CascadeProbability = 0.8f;
    
    FWorldEventEffect CrashEffect;
    CrashEffect.EffectType = TEXT("EconomicCollapse");
    CrashEffect.Magnitude = 0.9f;
    CrashEffect.Duration = 3600.0f; // 1 hour
    CrashEffect.AffectedSystems.Add(TEXT("Market"));
    CrashEffect.AffectedSystems.Add(TEXT("Economy"));
    CrashEffect.AffectedSystems.Add(TEXT("NPCEcosystem"));
    CrashEffect.AffectedSystems.Add(TEXT("Faction"));
    CrashEffect.SystemModifiers.Add(TEXT("CommodityPrices"), -0.6f);
    CrashEffect.SystemModifiers.Add(TEXT("TradeVolume"), -0.8f);
    CrashEffect.SystemModifiers.Add(TEXT("Employment"), -0.5f);
    CrashEffect.SystemModifiers.Add(TEXT("PopulationWealth"), -0.4f);
    MarketCrash.Effects.Add(CrashEffect);
    
    MarketCrash.TriggerEvents.Add(TEXT("Mass Migration"));
    MarketCrash.TriggerEvents.Add(TEXT("Bandit Uprising"));
    MarketCrash.TriggerEvents.Add(TEXT("Political Revolution"));
    EventTemplates.Add(TEXT("MarketCrash"), MarketCrash);
    
    // Supernatural Events
    FWorldEvent AncientAwakening;
    AncientAwakening.EventID = TEXT("AncientAwakening");
    AncientAwakening.EventName = TEXT("Ancient Evil Awakens");
    AncientAwakening.Description = TEXT("An ancient evil stirs, spreading darkness and fear across the land");
    AncientAwakening.EventType = EWorldEventType::Supernatural;
    AncientAwakening.Severity = EEventSeverity::Catastrophic;
    AncientAwakening.Scope = EEventScope::Global;
    AncientAwakening.Duration = 2400.0f; // 40 minutes
    AncientAwakening.AffectedRadius = 15000.0f;
    AncientAwakening.bHasCascadingEffects = true;
    AncientAwakening.CascadeProbability = 0.9f;
    
    FWorldEventEffect SupernaturalEffect;
    SupernaturalEffect.EffectType = TEXT("SupernaturalCorruption");
    SupernaturalEffect.Magnitude = 1.0f;
    SupernaturalEffect.Duration = 7200.0f; // 2 hours
    SupernaturalEffect.AffectedSystems.Add(TEXT("NPCEcosystem"));
    SupernaturalEffect.AffectedSystems.Add(TEXT("Weather"));
    SupernaturalEffect.AffectedSystems.Add(TEXT("Magic"));
    SupernaturalEffect.AffectedSystems.Add(TEXT("Faction"));
    SupernaturalEffect.SystemModifiers.Add(TEXT("NPCMorale"), -0.8f);
    SupernaturalEffect.SystemModifiers.Add(TEXT("MagicStability"), -0.6f);
    SupernaturalEffect.SystemModifiers.Add(TEXT("WeatherSeverity"), 0.7f);
    SupernaturalEffect.SystemModifiers.Add(TEXT("UndeadActivity"), 2.0f);
    AncientAwakening.Effects.Add(SupernaturalEffect);
    
    AncientAwakening.TriggerEvents.Add(TEXT("Undead Plague"));
    AncientAwakening.TriggerEvents.Add(TEXT("Divine Intervention"));
    AncientAwakening.TriggerEvents.Add(TEXT("Hero's Call"));
    EventTemplates.Add(TEXT("AncientAwakening"), AncientAwakening);
    
    // Environmental Events
    FWorldEvent Plague;
    Plague.EventID = TEXT("Plague");
    Plague.EventName = TEXT("Deadly Plague");
    Plague.Description = TEXT("A virulent disease spreads rapidly through settlements");
    Plague.EventType = EWorldEventType::Environmental;
    Plague.Severity = EEventSeverity::Major;
    Plague.Scope = EEventScope::Regional;
    Plague.Duration = 1800.0f; // 30 minutes
    Plague.AffectedRadius = 8000.0f;
    Plague.bHasCascadingEffects = true;
    Plague.CascadeProbability = 0.7f;
    
    FWorldEventEffect PlagueEffect;
    PlagueEffect.EffectType = TEXT("DiseaseSpread");
    PlagueEffect.Magnitude = 0.8f;
    PlagueEffect.Duration = 3600.0f; // 1 hour
    PlagueEffect.AffectedSystems.Add(TEXT("NPCEcosystem"));
    PlagueEffect.AffectedSystems.Add(TEXT("Economy"));
    PlagueEffect.AffectedSystems.Add(TEXT("Market"));
    PlagueEffect.SystemModifiers.Add(TEXT("PopulationHealth"), -0.6f);
    PlagueEffect.SystemModifiers.Add(TEXT("PopulationGrowth"), -0.8f);
    PlagueEffect.SystemModifiers.Add(TEXT("TradeRestrictions"), 0.9f);
    PlagueEffect.SystemModifiers.Add(TEXT("MedicinePrice"), 3.0f);
    Plague.Effects.Add(PlagueEffect);
    
    Plague.TriggerEvents.Add(TEXT("Quarantine"));
    Plague.TriggerEvents.Add(TEXT("Medical Research"));
    EventTemplates.Add(TEXT("Plague"), Plague);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d world event templates"), EventTemplates.Num());
}

void UAdvancedWorldEventsSubsystem::InitializeEventChains()
{
    // Apocalypse Chain
    FEventChain ApocalypseChain;
    ApocalypseChain.ChainID = TEXT("Apocalypse");
    ApocalypseChain.EventSequence.Add(TEXT("Earthquake"));
    ApocalypseChain.EventSequence.Add(TEXT("CivilUnrest"));
    ApocalypseChain.EventSequence.Add(TEXT("MarketCrash"));
    ApocalypseChain.EventSequence.Add(TEXT("Plague"));
    ApocalypseChain.EventSequence.Add(TEXT("AncientAwakening"));
    EventChains.Add(TEXT("Apocalypse"), ApocalypseChain);
    
    // Political Crisis Chain
    FEventChain PoliticalCrisis;
    PoliticalCrisis.ChainID = TEXT("PoliticalCrisis");
    PoliticalCrisis.EventSequence.Add(TEXT("CivilUnrest"));
    PoliticalCrisis.EventSequence.Add(TEXT("MarketCrash"));
    PoliticalCrisis.EventSequence.Add(TEXT("CivilUnrest")); // Escalation
    EventChains.Add(TEXT("PoliticalCrisis"), PoliticalCrisis);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d event chains"), EventChains.Num());
}

void UAdvancedWorldEventsSubsystem::InitializeProbabilityModifiers()
{
    EventProbabilityModifiers.Add(EWorldEventType::Natural, 1.0f);
    EventProbabilityModifiers.Add(EWorldEventType::Political, 0.8f);
    EventProbabilityModifiers.Add(EWorldEventType::Economic, 0.6f);
    EventProbabilityModifiers.Add(EWorldEventType::Military, 0.4f);
    EventProbabilityModifiers.Add(EWorldEventType::Social, 0.7f);
    EventProbabilityModifiers.Add(EWorldEventType::Supernatural, 0.1f);
    EventProbabilityModifiers.Add(EWorldEventType::Technological, 0.3f);
    EventProbabilityModifiers.Add(EWorldEventType::Environmental, 0.5f);
}

void UAdvancedWorldEventsSubsystem::ProcessEventGeneration()
{
    // Don't generate new events if we're at the limit
    if (ActiveEvents.Num() >= MaxConcurrentEvents)
    {
        return;
    }
    
    // Analyze world stability to determine event probability
    float WorldStability = AnalyzeWorldStability();
    float EventChance = GlobalEventProbability * (2.0f - WorldStability); // Less stable = more events
    
    if (FMath::FRand() < EventChance)
    {
        // Determine event type based on current world state
        TArray<EWorldEventType> PossibleEventTypes = {
            EWorldEventType::Natural,
            EWorldEventType::Political,
            EWorldEventType::Economic,
            EWorldEventType::Environmental
        };
        
        // Add rare event types based on conditions
        if (WorldStability < 0.3f)
        {
            PossibleEventTypes.Add(EWorldEventType::Supernatural);
            PossibleEventTypes.Add(EWorldEventType::Military);
        }
        
        EWorldEventType SelectedType = PossibleEventTypes[FMath::RandRange(0, PossibleEventTypes.Num() - 1)];
        
        // Find suitable event template
        TArray<FString> SuitableEvents;
        for (const auto& EventPair : EventTemplates)
        {
            if (EventPair.Value.EventType == SelectedType && CheckEventPrerequisites(EventPair.Value))
            {
                SuitableEvents.Add(EventPair.Key);
            }
        }
        
        if (SuitableEvents.Num() > 0)
        {
            FString SelectedEventID = SuitableEvents[FMath::RandRange(0, SuitableEvents.Num() - 1)];
            TriggerEvent(SelectedEventID);
        }
    }
}

void UAdvancedWorldEventsSubsystem::UpdateActiveEvents(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    for (int32 i = ActiveEvents.Num() - 1; i >= 0; i--)
    {
        FWorldEvent& Event = ActiveEvents[i];
        
        // Check if event should end
        if (Event.Duration > 0.0f && (CurrentTime - Event.StartTime) >= Event.Duration)
        {
            UE_LOG(LogTemp, Log, TEXT("World event ended: %s"), *Event.EventName);
            
            // Remove event effects
            RemoveEventEffects(Event);
            
            // Broadcast event ended
            OnWorldEventEnded.Broadcast(Event);
            
            // Remove from active events
            ActiveEvents.RemoveAt(i);
        }
        else
        {
            // Process cascading effects
            if (bEnableCascadingEvents && Event.bHasCascadingEffects)
            {
                ProcessCascadingEffects(Event);
            }
        }
    }
}

void UAdvancedWorldEventsSubsystem::UpdateEventChains(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    for (int32 i = ActiveEventChains.Num() - 1; i >= 0; i--)
    {
        FEventChain& Chain = ActiveEventChains[i];
        
        // Check if it's time for the next event in the chain
        if (Chain.CurrentEventIndex < Chain.EventSequence.Num())
        {
            float TimeSinceChainStart = CurrentTime - Chain.ChainStartTime;
            float NextEventDelay = (Chain.CurrentEventIndex + 1) * 120.0f; // 2 minutes between events
            
            if (TimeSinceChainStart >= NextEventDelay)
            {
                FString NextEventInChain = Chain.EventSequence[Chain.CurrentEventIndex];
                TriggerEvent(NextEventInChain);
                Chain.CurrentEventIndex++;
                
                UE_LOG(LogTemp, Log, TEXT("Event chain %s triggered event: %s"), *Chain.ChainID, *NextEventInChain);
            }
        }
        else
        {
            // Chain completed
            UE_LOG(LogTemp, Log, TEXT("Event chain completed: %s"), *Chain.ChainID);
            ActiveEventChains.RemoveAt(i);
        }
    }
}

void UAdvancedWorldEventsSubsystem::ProcessCascadingEffects(const FWorldEvent& TriggerEvent)
{
    // Only process cascades occasionally to avoid spam
    if (FMath::FRand() > 0.1f) // 10% chance per update
    {
        return;
    }
    
    TArray<FString> PotentialCascades = DetermineCascadeEvents(TriggerEvent);
    
    for (const FString& CascadeEventID : PotentialCascades)
    {
        float CascadeProbability = CalculateCascadeProbability(TriggerEvent, CascadeEventID);
        
        if (FMath::FRand() < CascadeProbability)
        {
            TriggerCascadeEvent(TriggerEvent);
            break; // Only trigger one cascade per update
        }
    }
}

void UAdvancedWorldEventsSubsystem::TriggerEvent(const FString& EventID)
{
    if (FWorldEvent* Template = EventTemplates.Find(EventID))
    {
        FWorldEvent NewEvent = CreateEventFromTemplate(EventID);
        
        // Set random location if not specified
        if (NewEvent.EpicenterLocation == FVector::ZeroVector)
        {
            NewEvent.EpicenterLocation = FVector(
                FMath::FRandRange(-10000.0f, 10000.0f),
                FMath::FRandRange(-10000.0f, 10000.0f),
                0.0f
            );
        }
        
        if (UWorld* World = GetWorld())
        {
            NewEvent.StartTime = World->GetTimeSeconds();
        }
        NewEvent.bIsActive = true;
        
        ActiveEvents.Add(NewEvent);
        
        // Apply event effects
        ApplyEventEffects(NewEvent);
        
        // Notify affected systems
        NotifyAffectedSystems(NewEvent);
        
        UE_LOG(LogTemp, Warning, TEXT("WORLD EVENT TRIGGERED: %s at location %s"), 
            *NewEvent.EventName, *NewEvent.EpicenterLocation.ToString());
        
        OnWorldEventTriggered.Broadcast(NewEvent);
    }
}

void UAdvancedWorldEventsSubsystem::TriggerEventAtLocation(const FString& EventID, const FVector& Location)
{
    if (FWorldEvent* Template = EventTemplates.Find(EventID))
    {
        FWorldEvent NewEvent = CreateEventFromTemplate(EventID);
        NewEvent.EpicenterLocation = Location;
        if (UWorld* World = GetWorld())
        {
            NewEvent.StartTime = World->GetTimeSeconds();
        }
        NewEvent.bIsActive = true;
        
        ActiveEvents.Add(NewEvent);
        ApplyEventEffects(NewEvent);
        NotifyAffectedSystems(NewEvent);
        
        UE_LOG(LogTemp, Warning, TEXT("WORLD EVENT TRIGGERED: %s at %s"), 
            *NewEvent.EventName, *Location.ToString());
        
        OnWorldEventTriggered.Broadcast(NewEvent);
    }
}

void UAdvancedWorldEventsSubsystem::StartEventChain(const FString& ChainID)
{
    if (FEventChain* ChainTemplate = EventChains.Find(ChainID))
    {
        FEventChain NewChain = *ChainTemplate;
        if (UWorld* World = GetWorld())
        {
            NewChain.ChainStartTime = World->GetTimeSeconds();
        }
        NewChain.bIsActive = true;
        NewChain.CurrentEventIndex = 0;
        
        ActiveEventChains.Add(NewChain);
        
        // Trigger first event immediately
        if (NewChain.EventSequence.Num() > 0)
        {
            TriggerEvent(NewChain.EventSequence[0]);
            NewChain.CurrentEventIndex = 1;
        }
        
        UE_LOG(LogTemp, Warning, TEXT("EVENT CHAIN STARTED: %s"), *ChainID);
        OnEventChainStarted.Broadcast(ChainID, NewChain);
    }
}

FWorldEvent UAdvancedWorldEventsSubsystem::CreateEventFromTemplate(const FString& TemplateID)
{
    FWorldEvent NewEvent = EventTemplates[TemplateID];
    NewEvent.EventID = GenerateUniqueEventID();
    
    // Add some randomization to severity and duration
    float SeverityModifier = FMath::FRandRange(0.8f, 1.2f);
    NewEvent.Duration *= SeverityModifier;
    
    for (FWorldEventEffect& Effect : NewEvent.Effects)
    {
        Effect.Magnitude *= SeverityModifier;
        Effect.Duration *= SeverityModifier;
    }
    
    return NewEvent;
}

void UAdvancedWorldEventsSubsystem::ApplyEventEffects(const FWorldEvent& Event)
{
    for (const FWorldEventEffect& Effect : Event.Effects)
    {
        ApplySystemModifiers(Event);
        UE_LOG(LogTemp, Log, TEXT("Applied event effect: %s (Magnitude: %f)"), 
            *Effect.EffectType, Effect.Magnitude);
    }
}

void UAdvancedWorldEventsSubsystem::NotifyAffectedSystems(const FWorldEvent& Event)
{
    // This would integrate with other subsystems to notify them of the event
    // For now, we'll just log the notifications
    
    for (const FWorldEventEffect& Effect : Event.Effects)
    {
        for (const FString& SystemName : Effect.AffectedSystems)
        {
            UE_LOG(LogTemp, Log, TEXT("Notifying system %s of event %s"), *SystemName, *Event.EventName);
            
            // In a real implementation, this would call specific subsystem methods
            // Example: GetGameInstance()->GetSubsystem<UEconomySubsystem>()->HandleWorldEvent(Event);
        }
    }
}

float UAdvancedWorldEventsSubsystem::AnalyzeWorldStability()
{
    float Stability = 1.0f;
    
    // Factor in active events
    for (const FWorldEvent& Event : ActiveEvents)
    {
        float SeverityImpact = GetEventSeverityMultiplier(Event.Severity);
        Stability -= (SeverityImpact * 0.1f);
    }
    
    // Factor in active event chains
    Stability -= (ActiveEventChains.Num() * 0.15f);
    
    return FMath::Clamp(Stability, 0.0f, 1.0f);
}

float UAdvancedWorldEventsSubsystem::GetEventSeverityMultiplier(EEventSeverity Severity) const
{
    switch (Severity)
    {
    case EEventSeverity::Minor:
        return 0.5f;
    case EEventSeverity::Moderate:
        return 1.0f;
    case EEventSeverity::Major:
        return 1.5f;
    case EEventSeverity::Critical:
        return 2.0f;
    case EEventSeverity::Catastrophic:
        return 3.0f;
    default:
        return 1.0f;
    }
}

TArray<FString> UAdvancedWorldEventsSubsystem::DetermineCascadeEvents(const FWorldEvent& Event)
{
    return Event.TriggerEvents;
}

void UAdvancedWorldEventsSubsystem::TriggerCascadeEvent(const FWorldEvent& OriginalEvent)
{
    if (OriginalEvent.TriggerEvents.Num() > 0)
    {
        FString CascadeEventID = OriginalEvent.TriggerEvents[FMath::RandRange(0, OriginalEvent.TriggerEvents.Num() - 1)];
        
        // Trigger the cascade event near the original event
        FVector CascadeLocation = OriginalEvent.EpicenterLocation + FVector(
            FMath::FRandRange(-2000.0f, 2000.0f),
            FMath::FRandRange(-2000.0f, 2000.0f),
            0.0f
        );
        
        TriggerEventAtLocation(CascadeEventID, CascadeLocation);
        
        UE_LOG(LogTemp, Warning, TEXT("CASCADE EVENT: %s triggered %s"), 
            *OriginalEvent.EventName, *CascadeEventID);
        
        OnCascadeEventTriggered.Broadcast(OriginalEvent.EventID, CascadeEventID);
    }
}

FString UAdvancedWorldEventsSubsystem::GenerateUniqueEventID()
{
    return FString::Printf(TEXT("Event_%d"), NextEventID++);
}

bool UAdvancedWorldEventsSubsystem::CheckEventPrerequisites(const FWorldEvent& Event)
{
    // For now, all events can trigger
    // In a more complex system, this would check world state conditions
    return true;
}

// Public Interface Implementation
TArray<FWorldEvent> UAdvancedWorldEventsSubsystem::GetActiveEvents() const
{
    return ActiveEvents;
}

TArray<FEventChain> UAdvancedWorldEventsSubsystem::GetActiveEventChains() const
{
    return ActiveEventChains;
}

bool UAdvancedWorldEventsSubsystem::IsEventActive(const FString& EventID) const
{
    for (const FWorldEvent& Event : ActiveEvents)
    {
        if (Event.EventID == EventID)
        {
            return true;
        }
    }
    return false;
}

void UAdvancedWorldEventsSubsystem::SetEventProbabilityModifier(EWorldEventType EventType, float Modifier)
{
    EventProbabilityModifiers.Add(EventType, Modifier);
    UE_LOG(LogTemp, Log, TEXT("Set probability modifier for event type %d to %f"), (int32)EventType, Modifier);
}

void UAdvancedWorldEventsSubsystem::ForceEndEvent(const FString& EventID)
{
    for (int32 i = 0; i < ActiveEvents.Num(); i++)
    {
        if (ActiveEvents[i].EventID == EventID)
        {
            RemoveEventEffects(ActiveEvents[i]);
            OnWorldEventEnded.Broadcast(ActiveEvents[i]);
            ActiveEvents.RemoveAt(i);
            UE_LOG(LogTemp, Log, TEXT("Force ended event: %s"), *EventID);
            break;
        }
    }
}

void UAdvancedWorldEventsSubsystem::ApplySystemModifiers(const FWorldEvent& Event)
{
    // This would apply modifiers to various game systems
    // Implementation would depend on the specific systems available
    UE_LOG(LogTemp, Log, TEXT("Applying system modifiers for event: %s"), *Event.EventName);
}

void UAdvancedWorldEventsSubsystem::RemoveSystemModifiers(const FWorldEvent& Event)
{
    // This would remove modifiers from various game systems
    UE_LOG(LogTemp, Log, TEXT("Removing system modifiers for event: %s"), *Event.EventName);
}

void UAdvancedWorldEventsSubsystem::RemoveEventEffects(const FWorldEvent& Event)
{
    RemoveSystemModifiers(Event);
    UE_LOG(LogTemp, Log, TEXT("Removed effects for event: %s"), *Event.EventName);
}

float UAdvancedWorldEventsSubsystem::CalculateCascadeProbability(const FWorldEvent& OriginalEvent, const FString& PotentialCascadeID)
{
    return OriginalEvent.CascadeProbability * GetEventSeverityMultiplier(OriginalEvent.Severity) * 0.1f;
}

float UAdvancedWorldEventsSubsystem::CalculateEventProbability(EWorldEventType EventType)
{
    float BaseProbability = GlobalEventProbability;
    
    if (const float* Modifier = EventProbabilityModifiers.Find(EventType))
    {
        BaseProbability *= *Modifier;
    }
    
    return BaseProbability;
}

float UAdvancedWorldEventsSubsystem::GetDistanceFromEpicenter(const FVector& Location, const FWorldEvent& Event)
{
    return FVector::Dist(Location, Event.EpicenterLocation);
}

bool UAdvancedWorldEventsSubsystem::IsLocationAffectedByEvent(const FVector& Location, const FWorldEvent& Event)
{
    return GetDistanceFromEpicenter(Location, Event) <= Event.AffectedRadius;
}