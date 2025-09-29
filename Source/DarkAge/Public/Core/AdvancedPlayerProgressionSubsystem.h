#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Data/AchievementData.h"
#include "AdvancedPlayerProgressionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EProgressionTreeType : uint8
{
    Combat      UMETA(DisplayName = "Combat"),
    Survival    UMETA(DisplayName = "Survival"),
    Social      UMETA(DisplayName = "Social"),
    Crafting    UMETA(DisplayName = "Crafting")
};

UENUM(BlueprintType)
enum class EAchievementCategory : uint8
{
    Combat      UMETA(DisplayName = "Combat"),
    Survival    UMETA(DisplayName = "Survival"),
    Social      UMETA(DisplayName = "Social"),
    Crafting    UMETA(DisplayName = "Crafting"),
    Exploration UMETA(DisplayName = "Exploration")
};

UENUM(BlueprintType)
enum class ELegacyEffectType : uint8
{
    Combat      UMETA(DisplayName = "Combat"),
    Survival    UMETA(DisplayName = "Survival"),
    Social      UMETA(DisplayName = "Social"),
    Crafting    UMETA(DisplayName = "Crafting")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FProgressionNode
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FString NodeID;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FString NodeName;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    int32 RequiredLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    int32 SkillPointCost = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    TArray<FString> Prerequisites;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    bool bIsUnlocked = false;

    FProgressionNode()
    {
        NodeID = TEXT("");
        NodeName = TEXT("");
        Description = TEXT("");
        RequiredLevel = 1;
        SkillPointCost = 1;
        bIsUnlocked = false;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FProgressionTree
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FString TreeName;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    EProgressionTreeType TreeType = EProgressionTreeType::Combat;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    TMap<FString, FProgressionNode> Nodes;

    FProgressionTree()
    {
        TreeName = TEXT("");
        Description = TEXT("");
        TreeType = EProgressionTreeType::Combat;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAchievement
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    FString AchievementID;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    EAchievementCategory Category = EAchievementCategory::Combat;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    int32 RequiredValue = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    int32 CurrentProgress = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    bool bIsUnlocked = false;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    float ExperienceReward = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Achievement")
    int32 SkillPointReward = 1;

    FAchievement()
    {
        AchievementID = TEXT("");
        Name = TEXT("");
        Description = TEXT("");
        Category = EAchievementCategory::Combat;
        RequiredValue = 1;
        CurrentProgress = 0;
        bIsUnlocked = false;
        ExperienceReward = 100.0f;
        SkillPointReward = 1;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FLegacyEffect
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    FString EffectID;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    ELegacyEffectType EffectType = ELegacyEffectType::Combat;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    TArray<FString> RequiredAchievements;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Legacy")
    float EffectMagnitude = 0.1f;

    FLegacyEffect()
    {
        EffectID = TEXT("");
        Name = TEXT("");
        Description = TEXT("");
        EffectType = ELegacyEffectType::Combat;
        bIsActive = false;
        EffectMagnitude = 0.1f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FActiveProgressionEffect
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Progression Effect")
    FString EffectName;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Effect")
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Effect")
    float RemainingTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Effect")
    float EffectMagnitude = 1.0f;

    FActiveProgressionEffect()
    {
        EffectName = TEXT("");
        Duration = 0.0f;
        RemainingTime = 0.0f;
        EffectMagnitude = 1.0f;
    }
};

// Progression Event Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceGained, float, Experience, float, TotalExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerLevelUp, int32, NewLevel, int32, AttributePoints, int32, SkillPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLegacyEffectActivated, const FLegacyEffect&, LegacyEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProgressionNodeUnlocked, const FString&, TreeName, const FString&, NodeID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAdvancedAchievementUnlocked, const FAchievement&, Achievement);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedPlayerProgressionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedPlayerProgressionSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedPlayerProgressionSubsystem, STATGROUP_Tickables); }

    // Progression Event Delegates
    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnExperienceGained OnExperienceGained;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnPlayerLevelUp OnPlayerLevelUp;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnLegacyEffectActivated OnLegacyEffectActivated;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnProgressionNodeUnlocked OnProgressionNodeUnlocked;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnAdvancedAchievementUnlocked OnAchievementUnlocked;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AwardExperience(float Experience);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool UnlockProgressionNode(const FString& TreeName, const FString& NodeID);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void UpdateAchievementProgress(const FString& AchievementID, int32 Progress);

    UFUNCTION(BlueprintPure, Category = "Progression")
    int32 GetPlayerLevel() const { return PlayerLevel; }

    UFUNCTION(BlueprintPure, Category = "Progression")
    float GetTotalExperience() const { return TotalExperience; }

    UFUNCTION(BlueprintPure, Category = "Progression")
    int32 GetAvailableAttributePoints() const { return AvailableAttributePoints; }

    UFUNCTION(BlueprintPure, Category = "Progression")
    int32 GetAvailableSkillPoints() const { return AvailableSkillPoints; }

    UFUNCTION(BlueprintPure, Category = "Progression")
    TArray<FAchievement> GetUnlockedAchievements() const;

    UFUNCTION(BlueprintPure, Category = "Progression")
    TArray<FLegacyEffect> GetActiveLegacyEffects() const;

    UFUNCTION(BlueprintPure, Category = "Progression")
    FProgressionTree GetProgressionTree(const FString& TreeName) const;

protected:
    // Player progression data
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 PlayerLevel = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    float TotalExperience = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 AvailableAttributePoints = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
    int32 AvailableSkillPoints = 0;

    // Progression systems
    UPROPERTY()
    TMap<FString, FProgressionTree> ProgressionTrees;

    UPROPERTY()
    TMap<FString, FAchievement> Achievements;

    UPROPERTY()
    TMap<FString, FLegacyEffect> LegacyEffects;

    UPROPERTY()
    TMap<FString, FActiveProgressionEffect> ActiveProgressionEffects;

    // Internal state
    float ProgressionUpdateTimer = 0.0f;

    // Initialization functions
    void InitializeProgressionTrees();
    void InitializeAchievementSystem();
    void InitializeLegacySystem();

    // Update functions
    void UpdateProgressionTracking();
    void UpdateCombatStats();
    void UpdateSurvivalStats();
    void UpdateSocialStats();
    void UpdateCraftingStats();
    void UpdateExplorationStats();
    void UpdateActiveEffects(float DeltaTime);

    // Achievement functions
    void CheckAchievements();
    void UnlockAchievement(const FString& AchievementID);

    // Legacy system functions
    void CheckLegacyEffectUnlocks();
    void UpdateLegacyEffects();
    void ApplyLegacyEffect(const FLegacyEffect& LegacyEffect);

    // Helper functions
    void LevelUp();
    float CalculateRequiredExperienceForLevel(int32 Level);
};