#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Data/QuestData.h"
#include "Core/DynamicQuestSubsystem.h"
#include "AdvancedQuestGenerationSubsystem.generated.h"

UENUM(BlueprintType)
enum class EQuestCategory : uint8
{
    Survival        UMETA(DisplayName = "Survival"),
    Combat          UMETA(DisplayName = "Combat"),
    Trade           UMETA(DisplayName = "Trade"),
    Social          UMETA(DisplayName = "Social"),
    Investigation   UMETA(DisplayName = "Investigation"),
    Crafting        UMETA(DisplayName = "Crafting")
};

// FQuestObjective and EObjectiveType are now defined in DynamicQuestSubsystem.h

USTRUCT(BlueprintType)
struct DARKAGE_API FQuestTemplate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    EQuestCategory QuestType = EQuestCategory::Survival;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString Title;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    float BaseReward = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    float BaseDifficulty = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    int32 RequiredLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    float TimeLimit = 1800.0f;

    FQuestTemplate()
    {
        QuestType = EQuestCategory::Survival;
        Title = TEXT("");
        Description = TEXT("");
        BaseReward = 50.0f;
        BaseDifficulty = 0.5f;
        RequiredLevel = 1;
        TimeLimit = 1800.0f;
    }
};

// FDynamicQuest is now defined in DynamicQuestSubsystem.h to avoid conflicts

USTRUCT(BlueprintType)
struct DARKAGE_API FGameStateContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    bool PlayerNeedsFood = false;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    bool PlayerNeedsMedicine = false;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    float EnemyThreatLevel = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    FString PrimaryThreatType;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    FString ThreatLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    float EconomicInstability = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    FString NeededResource;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    float PoliticalTension = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    TArray<FString> ConflictingFactions;

    FGameStateContext()
    {
        PlayerNeedsFood = false;
        PlayerNeedsMedicine = false;
        EnemyThreatLevel = 0.0f;
        PrimaryThreatType = TEXT("");
        ThreatLocation = TEXT("");
        EconomicInstability = 0.0f;
        NeededResource = TEXT("");
        PoliticalTension = 0.0f;
    }
};

// Quest Event Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestGenerated, const FDynamicQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccepted, const FDynamicQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestExpired, const FDynamicQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveCompleted, const FDynamicQuest&, Quest, const FQuestObjective&, Objective);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedQuestGenerationSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedQuestGenerationSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedQuestGenerationSubsystem, STATGROUP_Tickables); }

    // Quest Event Delegates
    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestGenerated OnQuestGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestAccepted OnQuestAccepted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestExpired OnQuestExpired;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestObjectiveCompleted OnQuestObjectiveCompleted;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<FDynamicQuest> GetAvailableQuests() const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<FDynamicQuest> GetActiveQuests() const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AcceptQuest(int32 QuestID);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AbandonQuest(int32 QuestID);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void ForceGenerateQuest(EQuestCategory QuestType);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    int32 GetAvailableQuestCount() const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    int32 GetActiveQuestCount() const;

protected:
    // Quest data
    UPROPERTY()
    TMap<FString, FQuestTemplate> QuestTemplates;

    UPROPERTY()
    TArray<FDynamicQuest> AvailableQuests;

    UPROPERTY()
    TArray<FDynamicQuest> ActiveQuests;

    // Quest parameters
    UPROPERTY()
    TArray<FString> ResourceTypes;

    UPROPERTY()
    TArray<FString> EnemyTypes;

    UPROPERTY()
    TArray<FString> ItemTypes;

    UPROPERTY()
    TArray<FString> Locations;

    UPROPERTY()
    TArray<FString> Mysteries;

    UPROPERTY()
    TArray<FString> QualityLevels;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Configuration")
    int32 MaxAvailableQuests = 10;

    // Internal state
    float QuestGenerationTimer = 0.0f;
    int32 NextQuestID = 1;

    // Initialization functions
    void InitializeQuestTemplates();
    void InitializeQuestParameters();

    // Quest generation functions
    void GenerateContextualQuests();
    FGameStateContext AnalyzeGameState();
    void GenerateSurvivalQuest(const FString& ResourceType);
    void GenerateCombatQuest(const FString& EnemyType, const FString& Location);
    void GenerateTradeQuest(const FString& NeededResource);
    void GenerateDiplomaticQuest(const FString& Faction1, const FString& Faction2);
    void GenerateRandomQuest();

    // Quest management functions
    FDynamicQuest CreateQuestFromTemplate(const FQuestTemplate& Template);
    void AddQuestToPool(const FDynamicQuest& Quest);
    void UpdateActiveQuests(float DeltaTime);
    void UpdateQuestObjectives(FDynamicQuest& Quest);
    void CompleteQuest(FDynamicQuest& Quest);
    void CleanupExpiredQuests();

    // Missing delegate declarations
    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestAbandoned OnQuestAbandoned;
};