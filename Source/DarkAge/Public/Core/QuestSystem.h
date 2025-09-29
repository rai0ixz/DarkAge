// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/QuestData.h"
#include "QuestSystem.generated.h"

class UEconomySubsystem;
class UFactionManagerSubsystem;

/**
 * Quest System for Dark Age
 *
 * Manages dynamic quest generation, tracking, and completion.
 * Integrates with faction, economy, and other game systems.
 */
UCLASS()
class DARKAGE_API UQuestSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UQuestSystem();
	
	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Deinitialize the subsystem
	virtual void Deinitialize() override;
	
	/**
	* Blueprint helper: Generate and accept a test quest in one call (for debugging)
	*/
	UFUNCTION(BlueprintCallable, Category = "Quest|Debug")
	FName GenerateAndAcceptTestQuest();
	
	// Update quest system
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void UpdateQuests(float DeltaTime);
	
	// Generate a new quest
	UFUNCTION(BlueprintCallable, Category = "Quest")
	FName GenerateQuest(EQuestType Type, FName Region, int32 Difficulty);
	
	// Accept a quest
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AcceptQuest(FName QuestID);
	
	// Abandon a quest
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AbandonQuest(FName QuestID);
	
	// Complete a quest
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteQuest(FName QuestID);
	
	// Fail a quest
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool FailQuest(FName QuestID);
	
	// Update objective progress
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress);
	
	// Get all available quests
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FQuestData> GetAvailableQuests() const;
	
	// Get all active quests
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FQuestData> GetActiveQuests() const;
	
	// Get quest data by ID
	UFUNCTION(BlueprintCallable, Category = "Quest")
	FQuestData GetQuestData(FName QuestID) const;
	
	// Check if all objectives are completed
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AreAllObjectivesCompleted(FName QuestID) const;
	
	// Delegate for quest status changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStatusChanged, FName, QuestID, EQuestState, NewStatus);
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestStatusChanged OnQuestStatusChanged;
	
	// Delegate for objective progress
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnObjectiveProgress, FName, QuestID, FName, ObjectiveID, int32, NewProgress);
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnObjectiveProgress OnObjectiveProgress;

public:
	// Get a mutable pointer to a quest data
	FQuestData* GetMutableQuestData(FName QuestID);
	
private:
	// All quests in the system
	UPROPERTY()
	TMap<FName, FQuestData> Quests;
	
	// Time since last quest generation check
	float TimeSinceLastGeneration;

	// Generate new dynamic quests
	void GenerateDynamicQuests();

	// Get the player's current region (placeholder)
	FName GetCurrentPlayerRegion() const;
	
	// Generate economic quest
	FQuestData GenerateEconomicQuest(FName Region, int32 Difficulty);
	
	// Generate political quest
	FQuestData GeneratePoliticalQuest(FName Region, int32 Difficulty);
	
	// Generate social quest
	FQuestData GenerateSocialQuest(FName Region, int32 Difficulty);
	
	// Generate environmental quest
	FQuestData GenerateEnvironmentalQuest(FName Region, int32 Difficulty);
	
	// Check for quest expiration
	void CheckQuestExpiration();
	
	// Generate a unique quest ID
	FName GenerateUniqueQuestID() const;

    UPROPERTY()
    UEconomySubsystem* EconomySubsystem;

    UPROPERTY()
    UFactionManagerSubsystem* FactionManagerSubsystem;
};