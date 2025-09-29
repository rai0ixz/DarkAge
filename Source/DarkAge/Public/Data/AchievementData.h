// AchievementData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AchievementData.generated.h"

/**
 * Achievement Types for categorization
 */
UENUM(BlueprintType)
enum class EAchievementType : uint8
{
    None            UMETA(DisplayName = "None"),
    Story           UMETA(DisplayName = "Story"),          // Main story achievements
    Exploration     UMETA(DisplayName = "Exploration"),    // Discovery and travel
    Combat          UMETA(DisplayName = "Combat"),         // Fighting and hunting
    Social          UMETA(DisplayName = "Social"),         // NPC relationships
    Economic        UMETA(DisplayName = "Economic"),       // Trade and wealth
    Survival        UMETA(DisplayName = "Survival"),       // Health and needs
    Crafting        UMETA(DisplayName = "Crafting"),       // Item creation
    Collection      UMETA(DisplayName = "Collection"),     // Gathering items
    Hidden          UMETA(DisplayName = "Hidden"),         // Secret achievements
    Milestone       UMETA(DisplayName = "Milestone")       // Major progression marks
};

/**
 * Achievement Rarity for UI display and rewards
 */
UENUM(BlueprintType)
enum class EAchievementRarity : uint8
{
    Common          UMETA(DisplayName = "Common"),
    Uncommon        UMETA(DisplayName = "Uncommon"),
    Rare            UMETA(DisplayName = "Rare"),
    Epic            UMETA(DisplayName = "Epic"),
    Legendary       UMETA(DisplayName = "Legendary")
};

/**
 * Achievement Reward Data Structure
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FAchievementReward
{
    GENERATED_BODY()

    FAchievementReward()
        : RewardType()
        , Amount(0)
        , ItemID()
        , Description()
    {
    }

    // Type of reward (Money, Item, Reputation, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Reward")
    FString RewardType;

    // Amount/quantity of reward
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Reward")
    int32 Amount;

    // Specific item identifier (if applicable)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Reward")
    FString ItemID;

    // Description for UI display
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Reward")
    FText Description;
};

/**
 * Achievement Progress Milestone
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FAchievementMilestone
{
    GENERATED_BODY()

    FAchievementMilestone()
        : ProgressValue(0)
        , Description()
        , bIsUnlocked(false)
    {
    }

    // Progress value required to unlock this milestone
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Milestone")
    int32 ProgressValue;

    // Description of this milestone
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Milestone")
    FText Description;

    // Whether this milestone has been unlocked
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Milestone")
    bool bIsUnlocked;
};

/**
 * Achievement Definition Data Structure for DataTable
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FAchievementData : public FTableRowBase
{
    GENERATED_BODY()

    FAchievementData()
        : AchievementID()
        , AchievementType(EAchievementType::None)
        , Rarity(EAchievementRarity::Common)
        , DisplayName()
        , Description()
        , FlavorText()
        , IconPath()
        , bIsHidden(false)
        , bIsProgressive(false)
        , MaxProgress(1)
        , Milestones()
        , Rewards()
        , PrerequisiteAchievements()
        , Category()
        , SortOrder(0)
    {
    }

    // Unique achievement identifier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    FName AchievementID;

    // Achievement type/category
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    EAchievementType AchievementType;

    // Achievement rarity level
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    EAchievementRarity Rarity;

    // Display name for UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    FText DisplayName;

    // Achievement description
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    FText Description;

    // Flavor text for immersion
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Basic")
    FText FlavorText;

    // Path to achievement icon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement UI")
    FString IconPath;

    // Hidden until unlocked or discovered
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Behavior")
    bool bIsHidden;

    // Has progress tracking (vs binary unlock)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Progress")
    bool bIsProgressive;

    // Maximum progress value for completion
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Progress")
    int32 MaxProgress;

    // Progress milestones for multi-stage achievements
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Progress")
    TArray<FAchievementMilestone> Milestones;

    // Rewards for completion
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Rewards")
    TArray<FAchievementReward> Rewards;

    // Achievements that must be unlocked first
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Prerequisites")
    TArray<FName> PrerequisiteAchievements;

    // Category for organization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Organization")
    FString Category;

    // Sort order within category
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Organization")
    int32 SortOrder;
};

/**
 * Player Achievement Progress - Runtime tracking data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FPlayerAchievementProgress
{
    GENERATED_BODY()

    FPlayerAchievementProgress()
        : AchievementID()
        , CurrentProgress(0)
        , bIsUnlocked(false)
        , UnlockTimestamp(0.0f)
        , bIsNotified(false)
    {
    }

    // Achievement identifier
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Progress")
    FName AchievementID;

    // Current progress value
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Progress")
    int32 CurrentProgress;

    // Whether achievement is unlocked
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Progress")
    bool bIsUnlocked;

    // World time when achievement was unlocked
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Progress")
    float UnlockTimestamp;

    // Whether player has been notified of unlock
    UPROPERTY(BlueprintReadOnly, Category = "Achievement Progress")
    bool bIsNotified;
};

/**
 * Achievement Event Delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAchievementUnlocked, FName, AchievementID, const FAchievementData&, AchievementData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAchievementProgressUpdated, FName, AchievementID, int32, NewProgress, int32, MaxProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAchievementMilestoneReached, FName, AchievementID, int32, MilestoneIndex);