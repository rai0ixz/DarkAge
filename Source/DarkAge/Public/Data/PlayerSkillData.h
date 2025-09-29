// PlayerSkillData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // If you plan to define base skill properties in a DataTable
#include "PlayerSkillData.generated.h"

// Optional: Enum for Skill IDs if you prefer strong typing over FNames
UENUM(BlueprintType)
enum class ESkillIdentifier : uint8
{
    SKILL_None UMETA(DisplayName = "None"),
    SKILL_Alchemy UMETA(DisplayName = "Alchemy"),
    SKILL_Lockpicking UMETA(DisplayName = "Lockpicking"),
    SKILL_Stealth UMETA(DisplayName = "Stealth"),
    SKILL_OneHanded UMETA(DisplayName = "One-Handed Weapons"),
    SKILL_Archery UMETA(DisplayName = "Archery")
    // Add other skills as needed from your GDD
};

// This struct will hold the dynamic data for a skill instance on a character
USTRUCT(BlueprintType)
struct FPlayerSkillInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Data")
    FName SkillID; // Using FName for flexibility, corresponds to a row in DT_SkillDefinitions or a unique key

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
    int32 CurrentLevel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
    float CurrentXP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
    float XPToNextLevel;

    // MODIFICATION: Added a list to track unlocked perks for this skill
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
    TArray<FName> UnlockedPerks;

    // Default constructor
    FPlayerSkillInstanceData()
        : SkillID(NAME_None)
        , CurrentLevel(1)
        , CurrentXP(0.0f)
        , XPToNextLevel(100.0f) // Example starting XP for level 2
    {
    }

    FPlayerSkillInstanceData(FName InSkillID, int32 InCurrentLevel, float InCurrentXP, float InXPToNextLevel)
        : SkillID(InSkillID)
        , CurrentLevel(InCurrentLevel)
        , CurrentXP(InCurrentXP)
        , XPToNextLevel(InXPToNextLevel)
    {
    }
};

USTRUCT(BlueprintType)
struct FPerkEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Effect")
    FName StatName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Effect")
    float ModifierValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Effect")
    bool bIsAdditive;

    FPerkEffect()
        : StatName(NAME_None)
        , ModifierValue(0.f)
        , bIsAdditive(true)
    {}
};

USTRUCT(BlueprintType)
struct FPerkDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    FName PerkID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    int32 RequiredSkillLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    TArray<FName> RequiredPerkIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk Definition")
    TArray<FPerkEffect> Effects;
    FORCEINLINE FPerkDefinition()
        : PerkID(NAME_None)
        , DisplayName(FText::GetEmpty())
        , Description(FText::GetEmpty())
        , RequiredSkillLevel(0)
    {
    }
};

// This struct could define the static properties of a skill, loadable from a DataTable
// Useful if you want to define names, descriptions, max levels, XP curves etc. in data.
USTRUCT(BlueprintType)
struct FSkillDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition", meta = (ClampMin = "1"))
    int32 MaxLevel;

    // Base XP required for the first level-up (from 1 to 2)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition", meta = (ClampMin = "1.0"))
    float BaseXPForLevel2;

    // Multiplier for how much more XP is needed for each subsequent level (e.g., 1.5 means 50% more XP per level)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition", meta = (ClampMin = "1.0"))
    float XPIncreaseFactorPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Definition")
    TArray<FName> PerkTree;

    FSkillDefinition()
        : DisplayName(FText::GetEmpty())
        , Description(FText::GetEmpty())
        , MaxLevel(100)
        , BaseXPForLevel2(100.0f)
        , XPIncreaseFactorPerLevel(1.2f) // Default: 20% more XP each level
    {
    }
};