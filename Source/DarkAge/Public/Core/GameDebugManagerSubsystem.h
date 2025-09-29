#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameDebugManagerSubsystem.generated.h"

UCLASS()
class DARKAGE_API UGameDebugManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- System Toggles ---
    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleEcosystem(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleMarket(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleSurvival(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleNPCEcosystem(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleSocialSimulation(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Systems")
    void ToggleWorldPopulation(bool bIsEnabled);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void SetSeason(const FString& RegionName, const FString& SeasonName);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void SetWeather(const FString& RegionName, const FString& WeatherName, float Intensity = 1.0f);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void SetResource(const FString& RegionName, const FString& ResourceName, float Value);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void SetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void TriggerEvent(const FString& RegionName, const FString& EventName, float Severity = 0.5f, float Duration = 1.0f);

    UFUNCTION(Exec, Category = "DebugManager|Ecosystem")
    void ClearEvent(const FString& RegionName, const FString& EventName);


    // --- Player Character Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|Player")
    void AddInventoryItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(Exec, Category = "DebugManager|Player")
    void PrintInventory();

    UFUNCTION(Exec, Category = "DebugManager|Player")
    void UseItemInSlot(int32 SlotIndex);

    UFUNCTION(Exec, Category = "DebugManager|Player")
    void PrintPlayerStats();

    UFUNCTION(Exec, Category = "DebugManager|Player")
    void CraftItem(FName RecipeID);

    // --- Skills Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|Skills")
    void AddSkillXP(FName SkillID, float XPAmount);

    UFUNCTION(Exec, Category = "DebugManager|Skills")
    void PrintSkillLevel(FName SkillID);

    UFUNCTION(Exec, Category = "DebugManager|Skills")
    void PrintAllSkills();

    UFUNCTION(Exec, Category = "DebugManager|Skills")
    void UnlockPerk(FName SkillID, FName PerkID);

    // --- Crime Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|Crime")
    void SimulateCrime(FString CrimeTypeString = "Theft", float NotorietyValue = 10.f);

    // --- Faction Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|Faction")
    void PrintFactionReputation(FString FactionIDString = "");

    UFUNCTION(Exec, Category = "DebugManager|Faction")
    void ModifyFactionReputation(FString FactionIDString, float Delta);

    UFUNCTION(Exec, Category = "DebugManager|Faction")
    void PrintFactionRelationships(FString FactionIDString = "");

    UFUNCTION(Exec, Category = "DebugManager|Faction")
    void SimulateFactionAI();
   
    // --- AI Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|AI")
       void SpawnAIPopulation(int32 Amount, const FString& AIBlueprintPath);
   
    // --- Political System Debug Commands ---
    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void PrintActiveTreaties(FString FactionIDString = "");

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void CreateTreaty(FString TreatyTypeString, FString Faction1String, FString Faction2String, int32 DurationDays = 365);

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void TerminateTreaty(FString TreatyIDString);

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void PrintActivePoliticalEvents(FString RegionIDString = "");

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void CreatePoliticalEvent(FString EventTypeString, FString PrimaryFactionString, FString SecondaryFactionString = "", FString RegionIDString = "", float Intensity = 0.7f);

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void ResolvePoliticalEvent(FString EventIDString, bool bSuccessful = true);

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void SimulateWar(FString AttackerFactionString, FString DefenderFactionString);

    UFUNCTION(Exec, Category = "DebugManager|Politics")
    void NegotiatePeace(FString Faction1String, FString Faction2String);

private:
    class ADAPlayerCharacter* GetPlayerCharacter() const;
};
