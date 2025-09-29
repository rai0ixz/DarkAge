#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/QuestData.h"
#include "QuestManagementSubsystem.generated.h"

// Forward declarations
class UWorldManagementSubsystem;
class UDAPlayerStateComponent;
class UDataTable;

/**
 * Quest Management Subsystem
 *
 * Handles the quest/mission system as outlined in the DarkAge GDD.
 * Manages quest discovery, availability, and dynamic generation.
 * Player-specific quest state is managed by DAQuestLogComponent.
 */
UCLASS(Config=Game, DefaultConfig)
class DARKAGE_API UQuestManagementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // UGameInstanceSubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Event Dispatchers
     */
    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestStateChanged OnQuestStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestObjectiveUpdated OnQuestObjectiveUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestStarted OnQuestStarted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestFailed OnQuestFailed;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestAbandoned OnQuestAbandoned;

    /**
     * Core Quest Management
     */

     // Start a quest by ID for the current player
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Core")
    bool StartQuest(FName QuestID, const FString& QuestGiverID = "");

    // Check if quest is available for the current player to start
    UFUNCTION(BlueprintPure, Category = "Quest Management|Core")
    bool IsQuestAvailable(FName QuestID) const;

    // Check if quest is currently active for the current player
    UFUNCTION(BlueprintPure, Category = "Quest Management|Core")
    bool IsQuestActive(FName QuestID) const;

    // Get quest state for the current player
    UFUNCTION(BlueprintPure, Category = "Quest Management|Core")
    EQuestState GetQuestState(FName QuestID) const;

    /**
     * Quest Discovery and Availability
     */

     // Get all available quests for current player state
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Discovery")
    TArray<FName> GetAvailableQuests() const;

    // Get available quests in current region
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Discovery")
    TArray<FName> GetAvailableQuestsInRegion(const FString& RegionID = "") const;

    // Get available quests from specific quest giver
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Discovery")
    TArray<FName> GetAvailableQuestsFromGiver(const FString& QuestGiverID) const;

    // Update quest availability (called when player state changes)
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Discovery")
    void UpdateQuestAvailability();

    /**
     * Objective Management
     */

     // Update objective progress for the current player
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Objectives")
    bool UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Amount = 1);

    /**
     * Quest Stage Management
     */

    // Get the current stage for a quest
    UFUNCTION(BlueprintPure, Category = "Quest Management|Stages")
    bool GetQuestStage(FName QuestID, FQuestStage& OutStage) const;

    // Activate a new stage for a quest
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Stages")
    bool ActivateQuestStage(FName QuestID, FName StageID);

    // Complete the current stage and transition to the next, based on evaluating branch conditions
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Stages")
    bool CompleteQuestStage(FName QuestID);

    /**
     * Quest Data Access
     */

     // Get quest data by ID
    UFUNCTION(BlueprintPure, Category = "Quest Management|Data")
    bool GetQuestData(FName QuestID, FQuestData& OutData) const;

    // Check if quest exists in data
    UFUNCTION(BlueprintPure, Category = "Quest Management|Data")
    bool DoesQuestExist(FName QuestID) const;

    // Get all quest IDs of specific type
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Data")
    TArray<FName> GetQuestsByType(EQuestType QuestType) const;

    // Get all quest IDs with specific tag
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Data")
    TArray<FName> GetQuestsByTag(const FString& Tag) const;

    /**
     * Dynamic Quest Generation
     */

     // Generate a dynamic quest based on current world state
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Generation")
    FName GenerateDynamicQuest(EQuestType QuestType, const FString& RegionID = "");

    /**
     * World Event Integration
     */

     // Handle world time progression (check time-limited quests)
    UFUNCTION(BlueprintCallable, Category = "Quest Management|World Events")
    void OnWorldTimeUpdate(float CurrentWorldTime);

    // Handle region change events
    UFUNCTION(BlueprintCallable, Category = "Quest Management|World Events")
    void OnPlayerRegionChanged(const FString& NewRegionID, const FString& PreviousRegionID);

protected:
    /**
     * Data Tables and Configuration
     */

     // Quest data table reference
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Data")
    UDataTable* QuestDataTable;

    // Dynamic quest templates data table
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Data")
    UDataTable* QuestTemplatesDataTable;

    /**
     * System References
     */

     // Cached world management subsystem
    UPROPERTY()
    UWorldManagementSubsystem* WorldManagementSubsystem;

private:
    /**
     * Internal Helper Functions
     */

     // Check if player meets quest prerequisites
    bool CheckQuestPrerequisites(const FQuestData& QuestData) const;

    // Evaluate the conditions for a quest branch
    bool AreBranchConditionsMet(const FQuestBranch& Branch) const;

    // Check a single quest condition
    bool CheckQuestCondition(const FQuestCondition& Condition) const;

    // Find player's quest log component
    class UDAQuestLogComponent* GetPlayerQuestLog() const;
};