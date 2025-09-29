// PlayerSkillsComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/PlayerSkillData.h" // Include the skill data definitions
#include "Engine/DataTable.h"    // For UDataTable
#include "PlayerSkillsComponent.generated.h"

// Delegate for when a skill levels up
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillLeveledUp, FName, SkillID, int32, NewLevel, int32, OldLevel);
// Delegate for when XP is added to a skill
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillXPAdded, FName, SkillID, float, XPAmount, float, NewTotalXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerkUnlocked, FName, SkillID, FName, PerkID);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UPlayerSkillsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerSkillsComponent();

protected:
    virtual void BeginPlay() override;

    // Map holding the player's current skill instances (SkillID -> SkillInstanceData)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills", SaveGame) // SaveGame if you want to persist skills
        TMap<FName, FPlayerSkillInstanceData> PlayerSkills;

    // DataTable containing definitions for all available skills (DisplayName, MaxLevel, XP Curve etc.)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills")
    TSoftObjectPtr<UDataTable> SkillDefinitionsDataTablePtr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills")
    TSoftObjectPtr<UDataTable> PerkDefinitionsDataTablePtr;

    UPROPERTY()
    TObjectPtr<UDataTable> LoadedSkillDefinitionsDataTable;

    UPROPERTY()
    TObjectPtr<UDataTable> LoadedPerkDefinitionsDataTable;

    // Helper function to calculate XP needed for a given level based on skill definition
    float CalculateXPForNextLevel(FName SkillID, int32 CurrentLevel) const;

    // Helper function to get skill definition
    bool GetSkillDefinition(FName SkillID, FSkillDefinition& OutSkillDef) const;

public:
    // Initializes skills, potentially loading from DataTable definitions
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void InitializeSkill(FName SkillID, bool bForceReinitialize = false);

    UFUNCTION(BlueprintCallable, Category = "Skills")
    void InitializeAllDefinedSkills();

    UFUNCTION(BlueprintPure, Category = "Skills")
    int32 GetSkillLevel(FName SkillID) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    float GetSkillXP(FName SkillID) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    float GetXPToNextLevel(FName SkillID) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    bool GetSkillProgress(FName SkillID, float& OutCurrentXP, float& OutXPForNext, int32& OutCurrentLevel) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    int32 GetCharacterLevel() const;

    UFUNCTION(BlueprintCallable, Category = "Skills")
    void AddXP(FName SkillID, float XPAmount);

    // --- MODIFICATION START: Perk Management Functions ---
    UFUNCTION(BlueprintCallable, Category = "Skills|Perks")
    bool UnlockPerk(FName SkillID, FName PerkID);

    UFUNCTION(BlueprintPure, Category = "Skills|Perks")
    bool HasPerk(FName SkillID, FName PerkID) const;
    // --- MODIFICATION END ---

    UFUNCTION(BlueprintPure, Category = "Skills")
    FText GetSkillDisplayName(FName SkillID) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    FText GetSkillDescription(FName SkillID) const;

    UFUNCTION(BlueprintPure, Category = "Skills")
    bool GetPlayerSkillInstanceData(FName SkillID, FPlayerSkillInstanceData& OutSkillData) const;



    UFUNCTION(BlueprintPure, Category = "Skills")
    const TMap<FName, FPlayerSkillInstanceData>& GetAllSkillsData() const;

    void SetPlayerSkills(const TMap<FName, FPlayerSkillInstanceData>& InPlayerSkills);

    UFUNCTION(BlueprintCallable, Category = "Skills")
    void PrintAllSkills();

private:
    TMap<FName, FPlayerSkillInstanceData>& GetMutablePlayerSkills() { return PlayerSkills; }

public:
    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Skills")
    FOnSkillLeveledUp OnSkillLeveledUp_Event;

    UPROPERTY(BlueprintAssignable, Category = "Skills")
    FOnSkillXPAdded OnSkillXPAdded_Event;

    UPROPERTY(BlueprintAssignable, Category = "Skills")
    FOnPerkUnlocked OnPerkUnlocked_Event;
};