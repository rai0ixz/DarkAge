#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/FactionData.h"
#include "DynamicQuestSubsystem.generated.h"

// Forward declaration for FQuestObjective
UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
    Collect         UMETA(DisplayName = "Collect"),
    Kill            UMETA(DisplayName = "Kill"),
    Deliver         UMETA(DisplayName = "Deliver"),
    TalkTo          UMETA(DisplayName = "Talk To"),
    Investigate     UMETA(DisplayName = "Investigate"),
    Craft           UMETA(DisplayName = "Craft")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    EObjectiveType ObjectiveType = EObjectiveType::Collect;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString TargetItem;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString TargetEnemy;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString TargetNPC;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString TargetLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    int32 RequiredCount = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    int32 CurrentCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    bool bIsCompleted = false;

    FQuestObjective()
    {
        ObjectiveType = EObjectiveType::Collect;
        TargetItem = TEXT("");
        TargetEnemy = TEXT("");
        TargetNPC = TEXT("");
        TargetLocation = TEXT("");
        RequiredCount = 1;
        CurrentCount = 0;
        bIsCompleted = false;
    }
};

// Forward declarations
class UWorldManagementSubsystem;
class UEconomySubsystem;

/**
 * Enumeration for quest types
 */
UENUM(BlueprintType)
enum class EDynamicQuestType : uint8
{
	Delivery       UMETA(DisplayName = "Delivery"),
	Elimination    UMETA(DisplayName = "Elimination"),
	Gathering      UMETA(DisplayName = "Gathering"),
	Exploration    UMETA(DisplayName = "Exploration"),
	Social         UMETA(DisplayName = "Social Interaction"),
	Protection     UMETA(DisplayName = "Protection"),
	Investigation  UMETA(DisplayName = "Investigation")
};

/**
 * Quest difficulty levels
 */
UENUM(BlueprintType)
enum class EDynamicQuestDifficulty : uint8
{
	Easy      UMETA(DisplayName = "Easy"),
	Medium    UMETA(DisplayName = "Medium"),
	Hard      UMETA(DisplayName = "Hard"),
	Extreme   UMETA(DisplayName = "Extreme")
};

/**
 * Quest status tracking
 */
UENUM(BlueprintType)
enum class EDynamicQuestStatus : uint8
{
	Generated     UMETA(DisplayName = "Generated"),
	Available     UMETA(DisplayName = "Available"),
	Active        UMETA(DisplayName = "Active"),
	Completed     UMETA(DisplayName = "Completed"),
	Failed        UMETA(DisplayName = "Failed"),
	Expired       UMETA(DisplayName = "Expired")
};

/**
 * Enumeration for quest event types
 */
UENUM(BlueprintType)
enum class EQuestEventType : uint8
{
	Accepted     UMETA(DisplayName = "Accepted"),
	Completed    UMETA(DisplayName = "Completed"),
	Failed       UMETA(DisplayName = "Failed"),
	Expired      UMETA(DisplayName = "Expired"),
	ChoiceMade   UMETA(DisplayName = "Choice Made")
};

/**
 * Structure representing a quest event log entry
 */
USTRUCT(BlueprintType)
struct FQuestEventLogEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestEventType EventType = EQuestEventType::Accepted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ChoiceOrDetail = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.0f;
};

/**
 * Structure representing a dynamically generated quest
 */
USTRUCT(BlueprintType)
struct FDynamicQuest
{
	GENERATED_BODY()

	// Unique identifier for this quest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestID = TEXT("");

	// Display name of the quest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestName = TEXT("");

	// Title of the quest (alias for QuestName for compatibility)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title = TEXT("");

	// Detailed description
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description = TEXT("");

	// Type of quest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDynamicQuestType QuestType = EDynamicQuestType::Delivery;

	// Difficulty level
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDynamicQuestDifficulty Difficulty = EDynamicQuestDifficulty::Medium;

	// Current status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDynamicQuestStatus Status = EDynamicQuestStatus::Generated;

	// Region where this quest takes place
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RegionID = TEXT("");

	// Faction associated with this quest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FactionID = NAME_None;

	// Quest objectives
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuestObjective> Objectives;

	// Rewards for completion
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> Rewards;

	// Gold reward (for compatibility)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GoldReward = 0.0f;

	// Experience reward (for compatibility)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExperienceReward = 0.0f;

	// Time limit for completion (0 = no time limit)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLimit = 0.0f;

	// Remaining time (updated during quest)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTime = 0.0f;

	// Target location (if applicable)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation = FVector::ZeroVector;

	// Required items or conditions
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> Requirements;

	// Progress tracking
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> Progress;

	// NPC quest giver (if any)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> QuestGiver;

	// Quest completion status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompleted = false;

	// Quest expiration status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsExpired = false;

	// Time when quest was created
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CreationTime = 0.0f;

	FORCEINLINE FDynamicQuest()
		: QuestType(EDynamicQuestType::Delivery)
		, Difficulty(EDynamicQuestDifficulty::Medium)
		, Status(EDynamicQuestStatus::Generated)
		, FactionID(NAME_None)
		, GoldReward(0.0f)
		, ExperienceReward(0.0f)
		, TimeLimit(0.0f)
		, RemainingTime(0.0f)
		, TargetLocation(FVector::ZeroVector)
		, QuestGiver(nullptr)
		, bIsCompleted(false)
		, bIsExpired(false)
		, CreationTime(0.0f)
	{
		QuestID = TEXT("");
	}
};

/**
 * Quest generation parameters for customizing quest creation
 */
USTRUCT(BlueprintType)
struct FQuestGenerationParams
{
	GENERATED_BODY()

	// Preferred quest types
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EDynamicQuestType> PreferredTypes;

	// Preferred difficulty range
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDynamicQuestDifficulty MinDifficulty = EDynamicQuestDifficulty::Easy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDynamicQuestDifficulty MaxDifficulty = EDynamicQuestDifficulty::Hard;

	// Target region for quest generation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TargetRegion;

	// Target faction for quest generation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetFaction;

	// Player level or progression factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerLevel = 1.0f;

	// Whether to include time-limited quests
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTimeLimited = true;

	// Maximum number of active quests
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxActiveQuests = 5;

	FQuestGenerationParams()
	{
		PreferredTypes = { EDynamicQuestType::Delivery, EDynamicQuestType::Gathering };
		MinDifficulty = EDynamicQuestDifficulty::Easy;
		MaxDifficulty = EDynamicQuestDifficulty::Hard;
		TargetRegion = "";
		TargetFaction = NAME_None;
		PlayerLevel = 1.0f;
		bAllowTimeLimited = true;
		MaxActiveQuests = 5;
	}
};

/**
 * Dynamic Quest Subsystem
 * 
 * Provides intelligent quest generation based on world state, player progress,
 * and dynamic events. Integrates with the WorldManagementSubsystem and AI
 * systems to create contextually appropriate quests.
 */
UCLASS()
class DARKAGE_API UDynamicQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UDynamicQuestSubsystem();

	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Quest Generation
	 */

	// Generate a new quest based on current world state
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	FDynamicQuest GenerateQuest(const FQuestGenerationParams& Params = FQuestGenerationParams());

	// Generate multiple quests for a quest board or NPC
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	TArray<FDynamicQuest> GenerateQuests(int32 Count, const FQuestGenerationParams& Params = FQuestGenerationParams());

	// Generate a quest of a specific type
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	FDynamicQuest GenerateQuestOfType(EDynamicQuestType QuestType, const FQuestGenerationParams& Params = FQuestGenerationParams());

	/**
	 * Quest Management
	 */

	// Add a quest to the active quest list
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	bool AddQuest(const FDynamicQuest& Quest);

	// Remove a quest by ID
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	bool RemoveQuest(const FString& QuestID);

	// Get quest by ID
	UFUNCTION(BlueprintPure, Category = "Dynamic Quest System")
	FDynamicQuest GetQuest(const FString& QuestID) const;

	// Get all active quests
	UFUNCTION(BlueprintPure, Category = "Dynamic Quest System")
	TArray<FDynamicQuest> GetActiveQuests() const;

	// Get quests by status
	UFUNCTION(BlueprintPure, Category = "Dynamic Quest System")
	TArray<FDynamicQuest> GetQuestsByStatus(EDynamicQuestStatus Status) const;

	// Get quests in a specific region
	UFUNCTION(BlueprintPure, Category = "Dynamic Quest System")
	TArray<FDynamicQuest> GetQuestsInRegion(const FString& RegionID) const;

	/**
	 * Quest Progress and Completion
	 */

	// Update quest progress
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	bool UpdateQuestProgress(const FString& QuestID, const FString& ObjectiveKey, float ProgressAmount);

	// Complete a quest
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	bool CompleteQuest(const FString& QuestID);

	// Fail a quest
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System")
	bool FailQuest(const FString& QuestID);

	// Check if quest objectives are completed
	UFUNCTION(BlueprintPure, Category = "Dynamic Quest System")
	bool AreObjectivesCompleted(const FString& QuestID) const;

	/**
	 * Dynamic Events Integration
	 */

	// Generate quests based on world events
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Events")
	TArray<FDynamicQuest> GenerateEventQuests(const FString& EventType, const FVector& EventLocation);

	// React to world state changes and generate appropriate quests
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Events")
	void ReactToWorldStateChange(const FString& StateChange, const FString& RegionID);

	/**
	 * Quest Templates and Customization
	 */

	// Add a custom quest template
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Templates")
	void AddQuestTemplate(const FString& TemplateName, const FDynamicQuest& Template);

	// Generate quest from template
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Templates")
	FDynamicQuest GenerateFromTemplate(const FString& TemplateName, const FQuestGenerationParams& Params);

	/**
	 * Quest Analytics and Event Log
	 */

	// Query quest events
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Analytics")
	TArray<FQuestEventLogEntry> QueryQuestEvents(const FString& QuestID, EQuestEventType EventType, int32 MaxResults = 50) const;

	// Get count of specific quest events
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Analytics")
	int32 GetQuestEventCount(const FString& QuestID, EQuestEventType EventType) const;

	// Get count of completed quests by type
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Analytics")
	int32 GetQuestTypeCompletionCount(EDynamicQuestType QuestType) const;

	// Log a player choice for a quest (for analytics)
	UFUNCTION(BlueprintCallable, Category = "Dynamic Quest System|Analytics")
	void LogPlayerQuestChoice(const FString& QuestID, const FString& PlayerID, const FString& ChoiceDetail);

protected:
	// Active quests
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dynamic Quest System")
	TMap<FString, FDynamicQuest> ActiveQuests;

	// Quest templates for generation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dynamic Quest System")
	TMap<FString, FDynamicQuest> QuestTemplates;

	// Quest event log
	UPROPERTY(VisibleAnywhere, Category = "Dynamic Quest System|Analytics")
	TArray<FQuestEventLogEntry> QuestEventLog;

	// Reference to world management subsystem
	UPROPERTY()
	TWeakObjectPtr<UWorldManagementSubsystem> WorldManagementSubsystem;

	// Reference to economy subsystem
	UPROPERTY()
	TWeakObjectPtr<UEconomySubsystem> EconomySubsystem;

	// Quest generation settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Quest System Settings")
	float QuestUpdateInterval = 60.0f; // 1 minute

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Quest System Settings")
	int32 MaxQuestsPerRegion = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Quest System Settings")
	float DefaultQuestReward = 100.0f;

	// --- Next-level: Quest analytics and event log ---
	void LogQuestEvent(const FString& QuestID, EQuestEventType EventType, const FString& PlayerID, const FString& ChoiceOrDetail);
	virtual UWorld* GetWorld() const override { return GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr; }

private:
	// Timer for quest updates
	FTimerHandle QuestUpdateTimer;

	// Possible quest types for generation
	TArray<EDynamicQuestType> PossibleQuestTypes;

	// Initialize default quest templates
	void InitializeQuestTemplates();

	// Update active quests (handle time limits, etc.)
	void UpdateActiveQuests();

	// Generate specific quest types
	FDynamicQuest GenerateDeliveryQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateEliminationQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateGatheringQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateExplorationQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateSocialQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateProtectionQuest(const FQuestGenerationParams& Params);
	FDynamicQuest GenerateInvestigationQuest(const FQuestGenerationParams& Params);

	// Generate quests based on economic conditions
	void GenerateResourceShortageQuests();

	// Helper functions
	FString GenerateQuestName(EDynamicQuestType QuestType, EDynamicQuestDifficulty Difficulty);
	FString GenerateQuestDescription(const FDynamicQuest& Quest);
	float CalculateQuestReward(const FDynamicQuest& Quest);
	FVector FindQuestLocation(const FString& RegionID, FName PointOfInterestID = NAME_None);
};