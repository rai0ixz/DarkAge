// Copyright RaioCore


#include "Components/DAQuestLogComponent.h"
#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
// RepNotify for QuestLog
void UDAQuestLogComponent::OnRep_QuestLog()
{
    // Broadcast quest state changed for all quests
    for (const auto& Entry : QuestLog)
    {
        OnQuestStateChanged.Broadcast(Entry.QuestID, Entry.QuestState);
    }
}

// Server RPCs
void UDAQuestLogComponent::ServerAddOrUpdateQuest_Implementation(FName QuestID, const FQuestData& QuestData)
{
    AddOrUpdateQuest(QuestID, QuestData);
}

void UDAQuestLogComponent::ServerUpdateObjectiveProgress_Implementation(FName QuestID, FName ObjectiveID, int32 Progress)
{
    UpdateObjectiveProgress(QuestID, ObjectiveID, Progress);
}

void UDAQuestLogComponent::ServerSetQuestStage_Implementation(FName QuestID, FName StageID)
{
    SetQuestStage(QuestID, StageID);
}

void UDAQuestLogComponent::ServerCompleteQuest_Implementation(FName QuestID)
{
    CompleteQuest(QuestID);
}

void UDAQuestLogComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDAQuestLogComponent, QuestLog);
}

UDAQuestLogComponent::UDAQuestLogComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDAQuestLogComponent::AddOrUpdateQuest(FName QuestID, const FQuestData& QuestData)
{
    FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; });
    if (!Entry)
    {
        Entry = &QuestLog.AddDefaulted_GetRef();
    }

    Entry->QuestID = QuestID;
    Entry->QuestState = EQuestState::QS_InProgress;
    Entry->CurrentStageID = QuestData.InitialStage;

    // Initialize objective progress
    for (const auto& StagePair : QuestData.Stages)
    {
        for (const auto& Objective : StagePair.Stage.Objectives)
        {
            FObjectiveProgressEntry& ObjEntry = Entry->ObjectiveProgress.AddDefaulted_GetRef();
            ObjEntry.ObjectiveID = Objective.ObjectiveID;
            ObjEntry.Progress = 0;
        }
    }

    OnQuestStateChanged.Broadcast(Entry->QuestID, Entry->QuestState);
}

bool UDAQuestLogComponent::UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress)
{
    if (FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; }))
    {
        if (FObjectiveProgressEntry* ObjEntry = Entry->ObjectiveProgress.FindByPredicate([&](const FObjectiveProgressEntry& InObjEntry){ return InObjEntry.ObjectiveID == ObjectiveID; }))
        {
            ObjEntry->Progress = Progress;
            return true;
        }
    }
    return false;
}

void UDAQuestLogComponent::SetQuestStage(FName QuestID, FName StageID)
{
    if (FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; }))
    {
        Entry->CurrentStageID = StageID;
    }
}

void UDAQuestLogComponent::CompleteQuest(FName QuestID)
{
    if (FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; }))
    {
        Entry->QuestState = EQuestState::QS_Completed;
        OnQuestStateChanged.Broadcast(QuestID, EQuestState::QS_Completed);
    }
}

EQuestState UDAQuestLogComponent::GetQuestState(FName QuestID) const
{
    if (const FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; }))
    {
        return Entry->QuestState;
    }
    return EQuestState::QS_NotStarted;
}

bool UDAQuestLogComponent::GetQuestLogEntry(FName QuestID, FQuestLogEntry& OutEntry) const
{
    if (const FQuestLogEntry* Entry = QuestLog.FindByPredicate([&](const FQuestLogEntry& InEntry){ return InEntry.QuestID == QuestID; }))
    {
        OutEntry = *Entry;
        return true;
    }
    return false;
}

TArray<FQuestLogEntry> UDAQuestLogComponent::GetActiveQuests() const
{
    TArray<FQuestLogEntry> ActiveQuests;
    for (const auto& Entry : QuestLog)
    {
        if (Entry.QuestState == EQuestState::QS_InProgress)
        {
            ActiveQuests.Add(Entry);
        }
    }
    return ActiveQuests;
}

void UDAQuestLogComponent::SetQuestLog(const TArray<FQuestLogEntry>& InQuestLog)
{
    QuestLog = InQuestLog;
}

const TArray<FQuestLogEntry>& UDAQuestLogComponent::GetQuestLog() const
{
    return QuestLog;
}

void UDAQuestLogComponent::BeginPlay()
{
    Super::BeginPlay();
}