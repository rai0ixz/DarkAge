#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/FactionData.h"
#include "Data/SocialData.h"
#include "Data/PoliticalData.h"
#include "Core/PoliticalSystem.h"
#include "FactionManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReputationChanged, FName, FactionID, float, NewReputation);

UCLASS(Blueprintable)
class DARKAGE_API UFactionManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void CreateFaction(FName FactionID, const FText& DisplayName);

    UFUNCTION(BlueprintPure, Category = "Factions")
    bool GetFactionData(FName FactionID, FFaction& OutFaction) const;

    UFUNCTION(BlueprintPure, Category = "Factions")
    bool GetFactionInfo(EFactionID FactionID, FFactionInfoData& OutFactionInfo) const;

    UFUNCTION(BlueprintPure, Category = "Factions")
    FText GetFactionDisplayName(FName FactionID) const;

    UFUNCTION(BlueprintCallable, Category = "Factions|Reputation")
    void SetPlayerReputation(FName FactionID, float NewReputation);

    UFUNCTION(BlueprintCallable, Category = "Factions|Reputation")
    void ModifyPlayerReputation(FName FactionID, float Delta);

    UFUNCTION(BlueprintPure, Category = "Factions|Reputation")
    float GetPlayerReputation(FName FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Factions|Relationships")
    EFactionRelationshipType GetFactionRelationship(FName FactionA, FName FactionB) const;

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void SetFactionRelationship(FName FactionA, FName FactionB, EFactionRelationshipType NewRelationship);

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void ModifyFactionRelationship(FName Faction1ID, FName Faction2ID, int32 Delta, bool bMutual);

    UFUNCTION(BlueprintPure, Category = "Factions")
    TArray<FName> GetAllFactions() const;

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void SimulateFactionAI(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Factions")
    FName GetTerritoryController(const FName& RegionID) const;

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void SetTerritoryControl(const FName& RegionID, const FName& FactionID, float Influence);

    UFUNCTION(BlueprintPure, Category = "Factions")
    bool GetFactionState(FName FactionID, FFactionData& OutFactionState) const;

    UFUNCTION(BlueprintCallable, Category = "Factions")
    void UpdateFactionActions();

    // Political system methods
    UFUNCTION(BlueprintCallable, Category = "Factions|Politics")
    bool DeclareWar(FName Faction1ID, FName Faction2ID);

    UFUNCTION(BlueprintCallable, Category = "Factions|Politics")
    bool FormAlliance(FName Faction1ID, FName Faction2ID);

    UFUNCTION(BlueprintCallable, Category = "Factions|Politics")
    bool NegotiatePeace(FName Faction1ID, FName Faction2ID);

    UFUNCTION(BlueprintCallable, Category = "Factions|Politics")
    void ProcessPoliticalEvents();

    UFUNCTION(BlueprintPure, Category = "Factions|Politics")
    TArray<FPoliticalEvent> GetRecentPoliticalEvents(int32 MaxEvents = 10) const;

    UFUNCTION(BlueprintPure, Category = "Factions|Politics")
    float GetPoliticalStability() const;

    UPROPERTY(BlueprintAssignable, Category = "Factions|Reputation")
    FOnPlayerReputationChanged OnPlayerReputationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Factions|Politics")
    FOnPoliticalEventOccurred OnPoliticalEventOccurred;

private:
    void CreateDefaultFactions();
    void RegisterDebugCommands();
    void SaveFactions() const;
    void LoadFactions();
    FString DebugListFactions(const TArray<FString>& Args);
    FString DebugGetReputation(const TArray<FString>& Args);
    FString DebugSetReputation(const TArray<FString>& Args);
    FString DebugGetRelationship(const TArray<FString>& Args);
    FString DebugSetRelationship(const TArray<FString>& Args);
    FName GetFactionNameFromID(EFactionID FactionID) const;

    // Political system helper methods
    void ApplyWarEconomicEffects(FName Faction1ID, FName Faction2ID);
    void ApplyAllianceEconomicEffects(FName Faction1ID, FName Faction2ID);
    void ApplyPeaceEconomicEffects(FName Faction1ID, FName Faction2ID);
    void PropagateWarEffects(FName Faction1ID, FName Faction2ID);
    float CalculatePeaceNegotiationChance(FName Faction1ID, FName Faction2ID);
    void DisruptTradeBetweenFactions(FName Faction1ID, FName Faction2ID);
    void ImproveTradeBetweenFactions(FName Faction1ID, FName Faction2ID);
    void RestoreTradeBetweenFactions(FName Faction1ID, FName Faction2ID);
    void GenerateRandomPoliticalEvent();
    void GenerateTradeDisputeEvent(FName Faction1ID, FName Faction2ID);
    void GenerateBorderSkirmishEvent(FName Faction1ID, FName Faction2ID);
    void GenerateDiplomaticMarriageEvent(FName Faction1ID, FName Faction2ID);
    void GenerateResourceDiscoveryEvent(FName FactionID);
    void GenerateLeadershipChangeEvent(FName FactionID);
    void UpdatePoliticalTensions();

    UPROPERTY(VisibleAnywhere, Category = "Factions", SaveGame)
    TMap<FName, FFaction> Factions;

    UPROPERTY(VisibleAnywhere, Category = "Factions", SaveGame)
    TMap<FName, FName> TerritoryControl;

    UPROPERTY(VisibleAnywhere, Category = "Factions", SaveGame)
    TMap<FName, float> PlayerFactionReputations;

    UPROPERTY(VisibleAnywhere, Category = "Factions|Politics", SaveGame)
    TArray<FPoliticalEvent> PoliticalEvents;
};