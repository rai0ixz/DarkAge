#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/SocialData.h" // For FSocialRelationship, FSocialEvent, FFactionData, FFNameArrayWrapper
#include "Data/FactionData.h"
#include "SocialSimulationSubsystem.generated.h"

/**
 * Social Simulation Subsystem
 *
 * Manages complex NPC relationships, faction dynamics, and social event propagation.
 * Creates a living world where character relationships evolve based on interactions,
 * witnessing events, and factional allegiances.
 *
 * Key Features:
 * - Dynamic relationship tracking between all characters
 * - Faction reputation and political systems
 * - Social event propagation and consequences
 * - Personality-driven relationship changes
 * - Marriage, family, and romantic relationships
 * - Political intrigue and power dynamics
 *
 * Integration Points:
 * - PlayerStateComponent: Player social standing and reputation
 * - DAAIController: NPC behavior modification based on relationships
 * - WorldManagementSubsystem: Regional social dynamics
 * - Crime System: Social consequences of criminal actions
 */
UCLASS()
class DARKAGE_API USocialSimulationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    using Super = UGameInstanceSubsystem;
    USocialSimulationSubsystem();

    /** If false, the entire social simulation update will be skipped. Can be toggled with the 'ToggleSocialSimulation' console command. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Simulation")
    bool bSocialSimulationEnabled = true;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void OnSystemInitialized();
    virtual void OnSystemStarted();
    virtual void OnSystemTick(float DeltaTime);
    virtual void OnSystemEvent(class UAdvancedSystemBase* SourceSystem, FName EventType, const FString& EventData);
    void BroadcastSystemEvent(const FName& EventName, const FString& EventData);

    // Relationship Management
    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Relationships")
    void UpdateRelationship(const FString& Character1ID, const FString& Character2ID, ESocialRelationshipType NewType, float StrengthChange);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Relationships")
    FSocialRelationship GetRelationship(const FString& Character1ID, const FString& Character2ID) const;

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Relationships")
    void RecordSocialInteraction(const FString& Character1ID, const FString& Character2ID, FName InteractionType, float ImpactStrength);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Relationships")
    TArray<FString> GetCharacterFriends(const FString& CharacterID, float MinRelationshipStrength = 50.0f) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Relationships")
    TArray<FString> GetCharacterEnemies(const FString& CharacterID, float MinRelationshipStrength = -50.0f) const;

    // Faction Management
    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void CreateFaction(FName FactionID, const FFactionData& FactionData);

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void AddCharacterToFaction(const FString& CharacterID, FName FactionID);

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void RemoveCharacterFromFaction(const FString& CharacterID, FName FactionID);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Factions")
    FFactionData GetFactionData(FName FactionID) const;

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void UpdateFactionReputation(FName FactionID, float ReputationChange, const FString& Reason);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Factions")
    TArray<FName> GetCharacterFactions(const FString& CharacterID) const;

    // --- Next-level: Faction diplomacy and analytics ---
    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void SetFactionAlliance(FName FactionA, FName FactionB, bool bAllied);

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Factions")
    void SetFactionRivalry(FName FactionA, FName FactionB, bool bRivals);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Factions")
    bool AreFactionsAllied(FName FactionA, FName FactionB) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Factions")
    bool AreFactionsRivals(FName FactionA, FName FactionB) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Analytics")
    TArray<FName> GetFactionAllies(FName FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Analytics")
    TArray<FName> GetFactionRivals(FName FactionID) const;

    // Social Events
    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Events")
    void TriggerSocialEvent(const FSocialEvent& SocialEvent);

    UFUNCTION(BlueprintCallable, Category = "Social Simulation|Events")
    void ProcessSocialEventPropagation(const FSocialEvent& OriginalEvent, float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Events")
    TArray<FSocialEvent> GetRecentSocialEvents(float TimeRange = 3600.0f) const;

    // Social Analytics
    UFUNCTION(BlueprintPure, Category = "Social Simulation|Analytics")
    float GetAverageSocialStanding(const FString& CharacterID) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Analytics")
    TArray<FString> GetMostInfluentialCharacters(int32 Count = 10) const;

    UFUNCTION(BlueprintPure, Category = "Social Simulation|Analytics")
    TArray<FName> GetMostPowerfulFactions(int32 Count = 5) const;

    // Event delegates
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRelationshipChanged, FString, Character1ID, FString, Character2ID, FSocialRelationship, NewRelationship);
    UPROPERTY(BlueprintAssignable, Category = "Social Simulation Events")
    FOnRelationshipChanged OnRelationshipChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFactionReputationChanged, FName, FactionID, float, NewReputation);
    UPROPERTY(BlueprintAssignable, Category = "Social Simulation Events")
    FOnFactionReputationChanged OnFactionReputationChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSocialEventTriggered, FSocialEvent, SocialEvent);
    UPROPERTY(BlueprintAssignable, Category = "Social Simulation Events")
    FOnSocialEventTriggered OnSocialEventTriggered;

protected:
    // Relationship storage (Character1ID_Character2ID -> Relationship)
    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TMap<FString, FSocialRelationship> CharacterRelationships;

    // Faction storage
    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TMap<FName, FFactionData> Factions;

    // Character to faction mappings
    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TMap<FString, FFNameArrayWrapper> CharacterFactionMemberships;

    // Faction diplomacy: alliances and rivalries
    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TMap<FName, FFNameArrayWrapper> FactionAlliances;

    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TMap<FName, FFNameArrayWrapper> FactionRivalries;

    // Recent social events
    UPROPERTY(VisibleAnywhere, Category = "Social Data")
    TArray<FSocialEvent> RecentSocialEvents;

    // Configuration
    UPROPERTY(EditAnywhere, Category = "Social Simulation Config")
    float RelationshipDecayRate = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Social Simulation Config")
    float EventPropagationSpeed = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Social Simulation Config")
    float MaxEventAge = 86400.0f; // 24 hours

    UPROPERTY(EditAnywhere, Category = "Social Simulation Config")
    int32 MaxStoredEvents = 1000;

    // System info
    FString SystemName;
    FString SystemVersion;
    int32 SystemPriority;
    float TickInterval;

private:
    // Helper functions
    FString GetRelationshipKey(const FString& Character1ID, const FString& Character2ID) const;
    void ProcessRelationshipDecay(float DeltaTime);
    void CleanupOldEvents();
    void ApplyEventToRelationship(const FSocialEvent& Event, const FString& Character1ID, const FString& Character2ID);

    // Event processing timer
    float LastEventCleanupTime;
    float LastRelationshipDecayTime;

    // System event broadcast stub
    void BroadcastSystemEvent(FName EventType, const FString& EventData);
};