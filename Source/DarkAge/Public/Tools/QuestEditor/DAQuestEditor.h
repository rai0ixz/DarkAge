#pragma once

#include "CoreMinimal.h"
#include "Tools/DAEditorToolBase.h"
#include "Engine/DataTable.h"
#include "DAQuestEditor.generated.h"


/**
 * Quest Types
 */
UENUM(BlueprintType)
enum class EDAQuestType : uint8
{
    Main            UMETA(DisplayName = "Main Quest"),
    Side            UMETA(DisplayName = "Side Quest"),
    Daily           UMETA(DisplayName = "Daily Quest"),
    Repeatable      UMETA(DisplayName = "Repeatable Quest"),
    Hidden          UMETA(DisplayName = "Hidden Quest"),
    Tutorial        UMETA(DisplayName = "Tutorial Quest")
};

/**
 * Quest Status
 */
UENUM(BlueprintType)
enum class EDAQuestStatus : uint8
{
    NotStarted      UMETA(DisplayName = "Not Started"),
    Available       UMETA(DisplayName = "Available"),
    Active          UMETA(DisplayName = "Active"),
    Completed       UMETA(DisplayName = "Completed"),
    Failed          UMETA(DisplayName = "Failed"),
    Abandoned       UMETA(DisplayName = "Abandoned"),
    Locked          UMETA(DisplayName = "Locked")
};

/**
 * Quest Objective Types
 */
UENUM(BlueprintType)
enum class EDAQuestObjectiveType : uint8
{
    KillTarget      UMETA(DisplayName = "Kill Target"),
    CollectItem     UMETA(DisplayName = "Collect Item"),
    TalkToNPC       UMETA(DisplayName = "Talk to NPC"),
    ReachLocation   UMETA(DisplayName = "Reach Location"),
    UseItem         UMETA(DisplayName = "Use Item"),
    Custom          UMETA(DisplayName = "Custom Objective")
};

/**
 * Quest Objective Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    FString ObjectiveID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    FText ObjectiveDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    EDAQuestObjectiveType ObjectiveType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    FString TargetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    int32 RequiredCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    int32 CurrentCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    bool bIsOptional;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    bool bIsHidden;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    TArray<FString> PrerequisiteObjectives;

    FDAQuestObjective()
    {
        ObjectiveID = TEXT("");
        ObjectiveDescription = FText::GetEmpty();
        ObjectiveType = EDAQuestObjectiveType::Custom;
        TargetID = TEXT("");
        RequiredCount = 1;
        CurrentCount = 0;
        bIsOptional = false;
        bIsHidden = false;
    }
};

/**
 * Quest Reward Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAQuestReward
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    FString RewardID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    FText RewardDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    int32 ExperiencePoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    int32 GoldAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    TArray<FString> ItemRewards;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    TMap<FString, int32> FactionReputationChanges;

    FDAQuestReward()
    {
        RewardID = TEXT("");
        RewardDescription = FText::GetEmpty();
        ExperiencePoints = 0;
        GoldAmount = 0;
    }
};

/**
 * Quest Data Structure
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAQuestData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestSummary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EDAQuestType QuestType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 QuestLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FDAQuestObjective> Objectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FDAQuestReward Rewards;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> PrerequisiteQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> ExclusiveQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestGiver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsRepeatable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    float TimeLimit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TMap<FString, FString> CustomProperties;

    FDAQuestData()
    {
        QuestID = TEXT("");
        QuestName = FText::GetEmpty();
        QuestDescription = FText::GetEmpty();
        QuestSummary = FText::GetEmpty();
        QuestType = EDAQuestType::Side;
        QuestLevel = 1;
        QuestGiver = TEXT("");
        QuestLocation = TEXT("");
        bIsRepeatable = false;
        TimeLimit = 0.0f;
    }
};

/**
 * DarkAge Quest Editor Tool
 *
 * This tool provides a comprehensive interface for creating, editing,
 * and managing quests within the DarkAge game.
 */

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UDAQuestEditor : public UDAEditorToolBase
{
	GENERATED_BODY()

public:
	UDAQuestEditor();

	// UDAEditorToolBase interface
	virtual bool Initialize(UDAEditorToolManager* InManager) override;
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void Tick(float DeltaTime) override;
	virtual FString GetToolDataAsJSON() override;
	virtual bool SetToolDataFromJSON(const FString& JSONData) override;

	// --- Quest Management ---

	/**
	 * Create a new quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	FDAQuestData CreateNewQuest(const FString& QuestID);

	/**
	 * Load an existing quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	bool LoadQuest(const FString& QuestID);

	/**
	 * Save the current quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	bool SaveCurrentQuest();

	/**
	 * Delete a quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	bool DeleteQuest(const FString& QuestID);

	/**
	 * Duplicate a quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	FDAQuestData DuplicateQuest(const FString& SourceQuestID, const FString& NewQuestID);

	/**
	 * Get all available quests
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor")
	TArray<FString> GetAllQuestIDs() const;

	/**
	 * Get quests by type
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor")
	TArray<FString> GetQuestsByType(EDAQuestType QuestType) const;

	/**
	 * Get the currently edited quest
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor")
	FDAQuestData GetCurrentQuest() const { return CurrentQuest; }

	/**
	 * Set the current quest data
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	void SetCurrentQuest(const FDAQuestData& QuestData);

	// --- Quest Validation ---

	/**
	 * Validate the current quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor")
	bool ValidateCurrentQuest(TArray<FString>& OutErrors) const;

	/**
	 * Check if a quest ID is unique
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor")
	bool IsQuestIDUnique(const FString& QuestID) const;

	/**
	 * Validate quest prerequisites
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor")
	bool ValidatePrerequisites(const FDAQuestData& QuestData, TArray<FString>& OutErrors) const;

	// --- Objective Management ---

	/**
	 * Add an objective to the current quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Objectives")
	void AddObjective(const FDAQuestObjective& Objective);

	/**
	 * Remove an objective from the current quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Objectives")
	bool RemoveObjective(const FString& ObjectiveID);

	/**
	 * Update an objective in the current quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Objectives")
	bool UpdateObjective(const FString& ObjectiveID, const FDAQuestObjective& UpdatedObjective);

	/**
	 * Get an objective by ID
	 */
	UFUNCTION(BlueprintPure, Category = "Quest Editor|Objectives")
	FDAQuestObjective GetObjective(const FString& ObjectiveID) const;

	/**
	 * Reorder objectives
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Objectives")
	void ReorderObjectives(const TArray<FString>& NewOrder);

	// --- Import/Export ---

	/**
	 * Export quest to JSON file
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Import Export")
	bool ExportQuestToFile(const FString& QuestID, const FString& FilePath);

	/**
	 * Import quest from JSON file
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Import Export")
	bool ImportQuestFromFile(const FString& FilePath);

	/**
	 * Export all quests to data table
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Import Export")
	bool ExportToDataTable(const FString& DataTablePath);

	/**
	 * Import quests from data table
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Import Export")
	bool ImportFromDataTable(UDataTable* DataTable);

	// --- Preview and Testing ---

	/**
	 * Preview quest in game
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Preview")
	void PreviewQuest(const FString& QuestID);

	/**
	 * Test quest flow
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Editor|Preview")
	bool TestQuestFlow(const FString& QuestID, TArray<FString>& OutIssues);

	// --- Events ---

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestLoaded, const FDAQuestData&, QuestData);
	UPROPERTY(BlueprintAssignable, Category = "Quest Editor|Events")
	FOnQuestLoaded OnQuestLoaded;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestSaved, const FString&, QuestID);
	UPROPERTY(BlueprintAssignable, Category = "Quest Editor|Events")
	FOnQuestSaved OnQuestSaved;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestValidated, bool, bIsValid);
	UPROPERTY(BlueprintAssignable, Category = "Quest Editor|Events")
	FOnQuestValidated OnQuestValidated;

protected:
	/**
	 * Current quest being edited
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Editor")
	FDAQuestData CurrentQuest;

	/**
	 * Quest data table reference
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Editor")
	UDataTable* QuestDataTable;

	/**
	 * Auto-save settings
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Editor")
	bool bAutoValidate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Editor")
	float ValidationInterval;

	/**
	 * Template quests for quick creation
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Editor")
	TMap<FString, FDAQuestData> QuestTemplates;

private:
	// Validation timer
	float ValidationTimer;

	// Helper methods
	bool LoadQuestDataTable();
	bool SaveQuestDataTable();
	FString GenerateUniqueQuestID(const FString& BaseName) const;
	void ValidateQuestPeriodically(float DeltaTime);
	bool ValidateObjectiveReferences(const FDAQuestData& QuestData, TArray<FString>& OutErrors) const;
	bool ValidateRewardData(const FDAQuestReward& Reward, TArray<FString>& OutErrors) const;
};