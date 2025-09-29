#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/QuestData.h"
#include "DAQuestLogComponent.generated.h"

USTRUCT(BlueprintType)
struct FObjectiveProgressEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    FName ObjectiveID = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    int32 Progress = 0;
};

USTRUCT(BlueprintType)
struct FQuestLogEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    FName QuestID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    EQuestState QuestState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    FName CurrentStageID;

    // Maps Objective ID to its current progress
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log")
    TArray<FObjectiveProgressEntry> ObjectiveProgress;

    FQuestLogEntry()
        : QuestID(NAME_None)
        , QuestState(EQuestState::QS_NotStarted)
        , CurrentStageID(NAME_None)
    {}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UDAQuestLogComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDAQuestLogComponent();

    UPROPERTY(BlueprintAssignable, Category = "Quest Log")
    FOnQuestStateChanged OnQuestStateChanged;

    // Adds a new quest to the log or updates its state if it already exists.
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void AddOrUpdateQuest(FName QuestID, const FQuestData& QuestData);

    // Updates the progress of a specific objective.
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    bool UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress);

    // Sets the current stage for a quest.
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void SetQuestStage(FName QuestID, FName StageID);

    // Completes a quest.
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void CompleteQuest(FName QuestID);

    // Gets the state of a specific quest.
    UFUNCTION(BlueprintPure, Category = "Quest Log")
    EQuestState GetQuestState(FName QuestID) const;

    // Gets the full log entry for a quest.
    UFUNCTION(BlueprintPure, Category = "Quest Log")
    bool GetQuestLogEntry(FName QuestID, FQuestLogEntry& OutEntry) const;

    // Gets all active quests.
    UFUNCTION(BlueprintPure, Category = "Quest Log")
    TArray<FQuestLogEntry> GetActiveQuests() const;

    // Sets the entire quest log, used for loading from a save game.
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void SetQuestLog(const TArray<FQuestLogEntry>& InQuestLog);

    // Gets the entire quest log, used for saving to a save game.
    UFUNCTION(BlueprintPure, Category = "Quest Log")
    const TArray<FQuestLogEntry>& GetQuestLog() const;

protected:
    virtual void BeginPlay() override;

    // Replicated quest log
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Log", ReplicatedUsing=OnRep_QuestLog)
    TArray<FQuestLogEntry> QuestLog;

    // RepNotify for QuestLog
    UFUNCTION()
    void OnRep_QuestLog();

public:
    // Server RPCs for quest log actions
    UFUNCTION(Server, Reliable)
    void ServerAddOrUpdateQuest(FName QuestID, const FQuestData& QuestData);

    UFUNCTION(Server, Reliable)
    void ServerUpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress);

    UFUNCTION(Server, Reliable)
    void ServerSetQuestStage(FName QuestID, FName StageID);

    UFUNCTION(Server, Reliable)
    void ServerCompleteQuest(FName QuestID);

    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};