#include "../Public/Core/SocialSimulationSubsystem.h"
#include "Components/DAPlayerStateComponent.h"
#include "AI/DAAIBaseCharacter.h"
#include "Engine/World.h"


USocialSimulationSubsystem::USocialSimulationSubsystem()
{
    SystemName = TEXT("SocialSimulationSubsystem");
    SystemVersion = TEXT("1.0.0");
    SystemPriority = 90;
    TickInterval = 1.0f;
    LastEventCleanupTime = 0.0f;
    LastRelationshipDecayTime = 0.0f;
}

void USocialSimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("SocialSimulationSubsystem Initialized."));
}

void USocialSimulationSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("SocialSimulationSubsystem Stopped."));
    Super::Deinitialize();
}

void USocialSimulationSubsystem::OnSystemInitialized() {}

void USocialSimulationSubsystem::OnSystemStarted() {}

void USocialSimulationSubsystem::BroadcastSystemEvent(const FName& EventName, const FString& EventData)
{
    // Stub implementation for linker
    UE_LOG(LogTemp, Log, TEXT("BroadcastSystemEvent called: %s, Data: %s"), *EventName.ToString(), *EventData);
}

void USocialSimulationSubsystem::OnSystemTick(float DeltaTime)
{
    if (!bSocialSimulationEnabled)
    {
        return;
    }

    // Super::OnSystemTick(DeltaTime); // Removed: not present in base class
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    // Process relationship decay
    if (CurrentTime - LastRelationshipDecayTime >= 60.0f) // Every minute
    {
        ProcessRelationshipDecay(DeltaTime);
        LastRelationshipDecayTime = CurrentTime;
    }
    
    // Clean up old events
    if (CurrentTime - LastEventCleanupTime >= 300.0f) // Every 5 minutes
    {
        CleanupOldEvents();
        LastEventCleanupTime = CurrentTime;
    }
}

void USocialSimulationSubsystem::OnSystemEvent(UAdvancedSystemBase* SourceSystem, FName EventType, const FString& EventData)
{
    // Super::OnSystemEvent(SourceSystem, EventType, EventData); // Removed: not present in base class
    
    // Handle events from other systems that affect social simulation
    if (EventType == FName("CrimeCommitted"))
    {
        // Parse crime data and create social event
        FSocialEvent CrimeEvent;
        CrimeEvent.EventType = FName("CrimeWitnessed");
        CrimeEvent.EventDescription = FString::Printf(TEXT("Crime event: %s"), *EventData);
        CrimeEvent.EventSeverity = 3.0f;
        if (UWorld* World = GetWorld())
        {
            CrimeEvent.EventTime = World->GetTimeSeconds();
        }
        CrimeEvent.bShouldPropagate = true;
        
        TriggerSocialEvent(CrimeEvent);
    }
    else if (EventType == FName("NPCDeath"))
    {
        // Handle NPC death affecting relationships
        FSocialEvent DeathEvent;
        DeathEvent.EventType = FName("CharacterDeath");
        DeathEvent.EventDescription = FString::Printf(TEXT("Character death: %s"), *EventData);
        DeathEvent.EventSeverity = 5.0f;
        if (UWorld* World = GetWorld())
        {
            DeathEvent.EventTime = World->GetTimeSeconds();
        }
        DeathEvent.bShouldPropagate = true;
        
        TriggerSocialEvent(DeathEvent);
    }
}

void USocialSimulationSubsystem::UpdateRelationship(const FString& Character1ID, const FString& Character2ID, ESocialRelationshipType NewType, float StrengthChange)
{
    if (Character1ID.IsEmpty() || Character2ID.IsEmpty() || Character1ID == Character2ID)
    {
        return;
    }
    
    FString RelationshipKey = GetRelationshipKey(Character1ID, Character2ID);
    FSocialRelationship* Relationship = CharacterRelationships.Find(RelationshipKey);
    
    if (!Relationship)
    {
        // Create new relationship
        FSocialRelationship NewRelationship;
        NewRelationship.RelationshipType = NewType;
        NewRelationship.RelationshipStrength = StrengthChange;
        if (UWorld* World = GetWorld())
        {
            NewRelationship.LastInteractionTime = World->GetTimeSeconds();
        }
        NewRelationship.InteractionCount = 1;
        
        CharacterRelationships.Add(RelationshipKey, NewRelationship);
        Relationship = &CharacterRelationships[RelationshipKey];
    }
    else
    {
        // Update existing relationship
        Relationship->RelationshipType = NewType;
        Relationship->RelationshipStrength = FMath::Clamp(Relationship->RelationshipStrength + StrengthChange, -100.0f, 100.0f);
        if (UWorld* World = GetWorld())
        {
            Relationship->LastInteractionTime = World->GetTimeSeconds();
        }
        Relationship->InteractionCount++;
    }
    
    // Broadcast relationship change
    OnRelationshipChanged.Broadcast(Character1ID, Character2ID, *Relationship);
    
    UE_LOG(LogTemp, Verbose, TEXT("Updated relationship between %s and %s: Strength %.2f"), 
           *Character1ID, *Character2ID, Relationship->RelationshipStrength);
}

FSocialRelationship USocialSimulationSubsystem::GetRelationship(const FString& Character1ID, const FString& Character2ID) const
{
    FString RelationshipKey = GetRelationshipKey(Character1ID, Character2ID);
    const FSocialRelationship* Relationship = CharacterRelationships.Find(RelationshipKey);
    
    if (Relationship)
    {
        return *Relationship;
    }
    
    // Return default relationship
    return FSocialRelationship();
}

void USocialSimulationSubsystem::RecordSocialInteraction(const FString& Character1ID, const FString& Character2ID, FName InteractionType, float ImpactStrength)
{
    // Determine relationship change based on interaction type
    ESocialRelationshipType NewType = ESocialRelationshipType::Acquaintance;
    float StrengthChange = ImpactStrength;
    
    if (InteractionType == FName("PositiveInteraction"))
    {
        StrengthChange = FMath::Abs(ImpactStrength);
    }
    else if (InteractionType == FName("NegativeInteraction"))
    {
        StrengthChange = -FMath::Abs(ImpactStrength);
    }
    else if (InteractionType == FName("Combat"))
    {
        NewType = ESocialRelationshipType::Enemy;
        StrengthChange = -FMath::Abs(ImpactStrength) * 2.0f;
    }
    else if (InteractionType == FName("Trade"))
    {
        NewType = ESocialRelationshipType::Business_Partner;
        StrengthChange = FMath::Abs(ImpactStrength) * 0.5f;
    }
    
    UpdateRelationship(Character1ID, Character2ID, NewType, StrengthChange);
    
    // Create social event for this interaction
    FSocialEvent InteractionEvent;
    InteractionEvent.EventType = InteractionType;
    InteractionEvent.EventDescription = FString::Printf(TEXT("%s interaction between %s and %s"), 
                                                         *InteractionType.ToString(), *Character1ID, *Character2ID);
    InteractionEvent.EventSeverity = FMath::Abs(ImpactStrength) / 10.0f;
    if (UWorld* World = GetWorld())
    {
        InteractionEvent.EventTime = World->GetTimeSeconds();
    }
    InteractionEvent.InvolvedCharacters.Add(Character1ID);
    InteractionEvent.InvolvedCharacters.Add(Character2ID);
    
    TriggerSocialEvent(InteractionEvent);
}

TArray<FString> USocialSimulationSubsystem::GetCharacterFriends(const FString& CharacterID, float MinRelationshipStrength) const
{
    TArray<FString> Friends;
    
    for (const auto& RelationshipPair : CharacterRelationships)
    {
        const FString& Key = RelationshipPair.Key;
        const FSocialRelationship& Relationship = RelationshipPair.Value;
        
        if (Relationship.RelationshipStrength >= MinRelationshipStrength)
        {
            // Extract character IDs from the relationship key
            FString Character1, Character2;
            if (Key.Split(TEXT("_"), &Character1, &Character2))
            {
                if (Character1 == CharacterID)
                {
                    Friends.AddUnique(Character2);
                }
                else if (Character2 == CharacterID)
                {
                    Friends.AddUnique(Character1);
                }
            }
        }
    }
    
    return Friends;
}

TArray<FString> USocialSimulationSubsystem::GetCharacterEnemies(const FString& CharacterID, float MinRelationshipStrength) const
{
    TArray<FString> Enemies;
    
    for (const auto& RelationshipPair : CharacterRelationships)
    {
        const FString& Key = RelationshipPair.Key;
        const FSocialRelationship& Relationship = RelationshipPair.Value;
        
        if (Relationship.RelationshipStrength <= MinRelationshipStrength)
        {
            // Extract character IDs from the relationship key
            FString Character1, Character2;
            if (Key.Split(TEXT("_"), &Character1, &Character2))
            {
                if (Character1 == CharacterID)
                {
                    Enemies.AddUnique(Character2);
                }
                else if (Character2 == CharacterID)
                {
                    Enemies.AddUnique(Character1);
                }
            }
        }
    }
    
    return Enemies;
}

void USocialSimulationSubsystem::CreateFaction(FName FactionID, const FFactionData& FactionData)
{
    Factions.Add(FactionID, FactionData);
    
    UE_LOG(LogTemp, Log, TEXT("Created faction '%s': %s"), 
           *FactionID.ToString(), *FactionData.FactionName);
}

void USocialSimulationSubsystem::AddCharacterToFaction(const FString& CharacterID, FName FactionID)
{
    if (!Factions.Contains(FactionID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add character %s to non-existent faction %s"), 
               *CharacterID, *FactionID.ToString());
        return;
    }
    
    // Add to character's faction list
    FFNameArrayWrapper& CharacterFactions = CharacterFactionMemberships.FindOrAdd(CharacterID);
    CharacterFactions.FactionIDs.AddUnique(FactionID);
    
    // Add to faction's member list
    FFactionData& FactionData = Factions[FactionID];
    FactionData.MemberCharacters.AddUnique(CharacterID);
    
    UE_LOG(LogTemp, Verbose, TEXT("Added character %s to faction %s"), 
           *CharacterID, *FactionID.ToString());
}

void USocialSimulationSubsystem::RemoveCharacterFromFaction(const FString& CharacterID, FName FactionID)
{
    // Remove from character's faction list
    if (FFNameArrayWrapper* CharacterFactions = CharacterFactionMemberships.Find(CharacterID))
    {
        CharacterFactions->FactionIDs.Remove(FactionID);
    }
    
    // Remove from faction's member list
    if (FFactionData* FactionData = Factions.Find(FactionID))
    {
        FactionData->MemberCharacters.Remove(CharacterID);
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Removed character %s from faction %s"), 
           *CharacterID, *FactionID.ToString());
}

FFactionData USocialSimulationSubsystem::GetFactionData(FName FactionID) const
{
    const FFactionData* FactionData = Factions.Find(FactionID);
    return FactionData ? *FactionData : FFactionData();
}

void USocialSimulationSubsystem::UpdateFactionReputation(FName FactionID, float ReputationChange, const FString& Reason)
{
    FFactionData* FactionData = Factions.Find(FactionID);
    if (!FactionData)
    {
        return;
    }
    
    float OldReputation = FactionData->PublicReputation;
    FactionData->PublicReputation = FMath::Clamp(FactionData->PublicReputation + ReputationChange, -100.0f, 100.0f);
    
    // Broadcast reputation change
    OnFactionReputationChanged.Broadcast(FactionID, FactionData->PublicReputation);
    
    UE_LOG(LogTemp, Log, TEXT("Faction %s reputation changed from %.2f to %.2f (Reason: %s)"), 
           *FactionID.ToString(), OldReputation, FactionData->PublicReputation, *Reason);
}

TArray<FName> USocialSimulationSubsystem::GetCharacterFactions(const FString& CharacterID) const
{
    const FFNameArrayWrapper* CharacterFactions = CharacterFactionMemberships.Find(CharacterID);
    return CharacterFactions ? CharacterFactions->FactionIDs : TArray<FName>();
}

void USocialSimulationSubsystem::TriggerSocialEvent(const FSocialEvent& SocialEvent)
{
    // Add to recent events
    RecentSocialEvents.Add(SocialEvent);
    
    // Broadcast the event
    OnSocialEventTriggered.Broadcast(SocialEvent);
    
    // Process immediate effects of the event
    for (int32 i = 0; i < SocialEvent.InvolvedCharacters.Num() - 1; ++i)
    {
        for (int32 j = i + 1; j < SocialEvent.InvolvedCharacters.Num(); ++j)
        {
            ApplyEventToRelationship(SocialEvent, SocialEvent.InvolvedCharacters[i], SocialEvent.InvolvedCharacters[j]);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Triggered social event: %s (%s)"), 
           *SocialEvent.EventType.ToString(), *SocialEvent.EventDescription);
}

void USocialSimulationSubsystem::ProcessSocialEventPropagation(const FSocialEvent& OriginalEvent, float DeltaTime)
{
    if (!OriginalEvent.bShouldPropagate)
    {
        return;
    }

    TSet<FString> PropagatedTo;
    for (const FString& CharacterID : OriginalEvent.InvolvedCharacters)
    {
        PropagatedTo.Add(CharacterID);
    }

    for (const FString& CharacterID : OriginalEvent.InvolvedCharacters)
    {
        // Propagate to friends
        TArray<FString> Friends = GetCharacterFriends(CharacterID, 30.0f); // Friends with relationship > 30
        for (const FString& FriendID : Friends)
        {
            if (!PropagatedTo.Contains(FriendID))
            {
                FSocialEvent RumorEvent;
                RumorEvent.EventType = FName("Rumor");
                RumorEvent.EventDescription = FString::Printf(TEXT("Heard a rumor about: %s"), *OriginalEvent.EventDescription);
                RumorEvent.EventSeverity = OriginalEvent.EventSeverity * 0.5f; // Rumors are less severe
                if (UWorld* World = GetWorld())
                {
                    RumorEvent.EventTime = World->GetTimeSeconds();
                }
                RumorEvent.InvolvedCharacters.Add(FriendID);
                RumorEvent.bShouldPropagate = false; // Rumors don't propagate further in this simple model

                TriggerSocialEvent(RumorEvent);
                PropagatedTo.Add(FriendID);
            }
        }
    }
}

TArray<FSocialEvent> USocialSimulationSubsystem::GetRecentSocialEvents(float TimeRange) const
{
    TArray<FSocialEvent> FilteredEvents;
    UWorld* World = GetWorld();
    if (!World)
    {
        return FilteredEvents;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    for (const FSocialEvent& Event : RecentSocialEvents)
    {
        if (CurrentTime - Event.EventTime <= TimeRange)
        {
            FilteredEvents.Add(Event);
        }
    }
    
    return FilteredEvents;
}

float USocialSimulationSubsystem::GetAverageSocialStanding(const FString& CharacterID) const
{
    float TotalRelationshipStrength = 0.0f;
    int32 RelationshipCount = 0;
    
    for (const auto& RelationshipPair : CharacterRelationships)
    {
        const FString& Key = RelationshipPair.Key;
        const FSocialRelationship& Relationship = RelationshipPair.Value;
        
        FString Character1, Character2;
        if (Key.Split(TEXT("_"), &Character1, &Character2))
        {
            if (Character1 == CharacterID || Character2 == CharacterID)
            {
                TotalRelationshipStrength += Relationship.RelationshipStrength;
                RelationshipCount++;
            }
        }
    }
    
    return RelationshipCount > 0 ? TotalRelationshipStrength / RelationshipCount : 0.0f;
}

TArray<FString> USocialSimulationSubsystem::GetMostInfluentialCharacters(int32 Count) const
{
    TMap<FString, float> CharacterInfluence;

    // Get all unique characters
    TSet<FString> AllCharacters;
    for (const auto& RelationshipPair : CharacterRelationships)
    {
        FString C1, C2;
        RelationshipPair.Key.Split(TEXT("_"), &C1, &C2);
        AllCharacters.Add(C1);
        AllCharacters.Add(C2);
    }

    // Calculate influence score for each character
    for (const FString& CharacterID : AllCharacters)
    {
        float Influence = 0.0f;
        // Add influence from relationships
        TArray<FString> Friends = GetCharacterFriends(CharacterID, 0.0f);
        Influence += Friends.Num() * 1.0f; // 1 point per friend
        for(const FString& FriendID : Friends)
        {
            Influence += GetRelationship(CharacterID, FriendID).RelationshipStrength / 100.0f; // Add normalized relationship strength
        }

        // Add influence from faction power
        TArray<FName> FactionsOfChar = GetCharacterFactions(CharacterID);
        for (const FName& FactionID : FactionsOfChar)
        {
            Influence += GetFactionData(FactionID).PowerLevel * 0.1f; // Add 10% of faction power
        }
        
        CharacterInfluence.Add(CharacterID, Influence);
    }

    // Sort characters by influence
    CharacterInfluence.ValueSort([](const float A, const float B) {
        return A > B;
    });

    TArray<FString> Result;
    for (const auto& InfluencePair : CharacterInfluence)
    {
        if (Result.Num() >= Count)
        {
            break;
        }
        Result.Add(InfluencePair.Key);
    }

    return Result;
}

TArray<FName> USocialSimulationSubsystem::GetMostPowerfulFactions(int32 Count) const
{
    TArray<TPair<FName, float>> FactionPowers;
    
    for (const auto& FactionPair : Factions)
    {
        FactionPowers.Add(TPair<FName, float>(FactionPair.Key, FactionPair.Value.PowerLevel));
    }
    
    // Sort by power level
    FactionPowers.Sort([](const TPair<FName, float>& A, const TPair<FName, float>& B) {
        return A.Value > B.Value;
    });
    
    TArray<FName> Result;
    for (int32 i = 0; i < FMath::Min(Count, FactionPowers.Num()); ++i)
    {
        Result.Add(FactionPowers[i].Key);
    }
    
    return Result;
}

FString USocialSimulationSubsystem::GetRelationshipKey(const FString& Character1ID, const FString& Character2ID) const
{
    // Ensure consistent key ordering
    if (Character1ID.Compare(Character2ID) < 0)
    {
        return FString::Printf(TEXT("%s_%s"), *Character1ID, *Character2ID);
    }
    else
    {
        return FString::Printf(TEXT("%s_%s"), *Character2ID, *Character1ID);
    }
}

void USocialSimulationSubsystem::ProcessRelationshipDecay(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    for (auto& RelationshipPair : CharacterRelationships)
    {
        FSocialRelationship& Relationship = RelationshipPair.Value;
        
        // Decay relationships that haven't been updated recently
        float TimeSinceLastInteraction = CurrentTime - Relationship.LastInteractionTime;
        if (TimeSinceLastInteraction > 86400.0f) // 24 hours
        {
            float DecayAmount = RelationshipDecayRate * (TimeSinceLastInteraction / 86400.0f);
            
            if (Relationship.RelationshipStrength > 0.0f)
            {
                Relationship.RelationshipStrength = FMath::Max(0.0f, Relationship.RelationshipStrength - DecayAmount);
            }
            else if (Relationship.RelationshipStrength < 0.0f)
            {
                Relationship.RelationshipStrength = FMath::Min(0.0f, Relationship.RelationshipStrength + DecayAmount);
            }
        }
    }
}

void USocialSimulationSubsystem::CleanupOldEvents()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    // Remove events older than MaxEventAge
    RecentSocialEvents.RemoveAll([CurrentTime, this](const FSocialEvent& Event) {
        return CurrentTime - Event.EventTime > MaxEventAge;
    });
    
    // Limit number of stored events
    if (RecentSocialEvents.Num() > MaxStoredEvents)
    {
        RecentSocialEvents.Sort([](const FSocialEvent& A, const FSocialEvent& B) {
            return A.EventTime > B.EventTime; // Sort by time, newest first
        });
        
        RecentSocialEvents.SetNum(MaxStoredEvents);
    }
}

void USocialSimulationSubsystem::ApplyEventToRelationship(const FSocialEvent& Event, const FString& Character1ID, const FString& Character2ID)
{
    float ImpactStrength = Event.EventSeverity;
    
    // Determine impact based on event type
    if (Event.EventType == FName("CrimeWitnessed"))
    {
        // Negative impact on relationship with criminal
        UpdateRelationship(Character1ID, Character2ID, ESocialRelationshipType::Enemy, -ImpactStrength * 5.0f);
    }
    else if (Event.EventType == FName("PositiveInteraction"))
    {
        // Positive impact
        UpdateRelationship(Character1ID, Character2ID, ESocialRelationshipType::Friend, ImpactStrength * 2.0f);
    }
    else if (Event.EventType == FName("CharacterDeath"))
    {
        // Profound negative impact if characters were friends
        FSocialRelationship ExistingRelationship = GetRelationship(Character1ID, Character2ID);
        if (ExistingRelationship.RelationshipStrength > 0.0f)
        {
            UpdateRelationship(Character1ID, Character2ID, ESocialRelationshipType::Stranger, -ImpactStrength * 10.0f);
        }
    }
}

void USocialSimulationSubsystem::SetFactionAlliance(FName FactionA, FName FactionB, bool bAllied)
{
    if (bAllied)
    {
        FactionAlliances.FindOrAdd(FactionA).FactionIDs.AddUnique(FactionB);
        FactionAlliances.FindOrAdd(FactionB).FactionIDs.AddUnique(FactionA);
    }
    else
    {
        FactionAlliances.FindOrAdd(FactionA).FactionIDs.Remove(FactionB);
        FactionAlliances.FindOrAdd(FactionB).FactionIDs.Remove(FactionA);
    }
}

void USocialSimulationSubsystem::SetFactionRivalry(FName FactionA, FName FactionB, bool bRivals)
{
    if (bRivals)
    {
        FactionRivalries.FindOrAdd(FactionA).FactionIDs.AddUnique(FactionB);
        FactionRivalries.FindOrAdd(FactionB).FactionIDs.AddUnique(FactionA);
    }
    else
    {
        FactionRivalries.FindOrAdd(FactionA).FactionIDs.Remove(FactionB);
        FactionRivalries.FindOrAdd(FactionB).FactionIDs.Remove(FactionA);
    }
}

bool USocialSimulationSubsystem::AreFactionsAllied(FName FactionA, FName FactionB) const
{
    const FFNameArrayWrapper* Allies = FactionAlliances.Find(FactionA);
    return Allies && Allies->FactionIDs.Contains(FactionB);
}

bool USocialSimulationSubsystem::AreFactionsRivals(FName FactionA, FName FactionB) const
{
    const FFNameArrayWrapper* Rivals = FactionRivalries.Find(FactionA);
    return Rivals && Rivals->FactionIDs.Contains(FactionB);
}

TArray<FName> USocialSimulationSubsystem::GetFactionAllies(FName FactionID) const
{
    const FFNameArrayWrapper* Allies = FactionAlliances.Find(FactionID);
    return Allies ? Allies->FactionIDs : TArray<FName>();
}

TArray<FName> USocialSimulationSubsystem::GetFactionRivals(FName FactionID) const
{
    const FFNameArrayWrapper* Rivals = FactionRivalries.Find(FactionID);
    return Rivals ? Rivals->FactionIDs : TArray<FName>();
}