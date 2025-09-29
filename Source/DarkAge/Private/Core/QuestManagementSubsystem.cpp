// Copyright RaioCore


#include "Core/QuestManagementSubsystem.h"
#include "Data/QuestData.h"
#include "Components/DAQuestLogComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/StatlineComponent.h"
#include "Core/SocialSimulationSubsystem.h"
#include "Data/ItemData.h"
#include "Items/DABaseItem.h"

void UQuestManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UQuestManagementSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool UQuestManagementSubsystem::StartQuest(FName QuestID, const FString& QuestGiverID)
{
    FQuestData QuestData;
    if (!GetQuestData(QuestID, QuestData))
    {
        return false;
    }

    if (!CheckQuestPrerequisites(QuestData))
    {
        return false;
    }

    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (QuestLog)
    {
        QuestLog->AddOrUpdateQuest(QuestID, QuestData);
        OnQuestStarted.Broadcast(QuestID);
        return true;
    }

    return false;
}

bool UQuestManagementSubsystem::IsQuestAvailable(FName QuestID) const
{
    FQuestData QuestData;
    if (!GetQuestData(QuestID, QuestData))
    {
        return false;
    }

    return GetQuestState(QuestID) == EQuestState::QS_NotStarted && CheckQuestPrerequisites(QuestData);
}

bool UQuestManagementSubsystem::IsQuestActive(FName QuestID) const
{
    return GetQuestState(QuestID) == EQuestState::QS_InProgress;
}

EQuestState UQuestManagementSubsystem::GetQuestState(FName QuestID) const
{
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (QuestLog)
    {
        return QuestLog->GetQuestState(QuestID);
    }
    return EQuestState::QS_NotStarted;
}

bool UQuestManagementSubsystem::UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Amount)
{
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (!QuestLog || !IsQuestActive(QuestID))
    {
        return false;
    }

    FQuestLogEntry QuestEntry;
    if (!QuestLog->GetQuestLogEntry(QuestID, QuestEntry))
    {
        return false;
    }

    FQuestData QuestData;
    if (!GetQuestData(QuestID, QuestData))
    {
        return false;
    }

    const FQuestStageEntry* CurrentStageEntry = QuestData.Stages.FindByPredicate([&](const FQuestStageEntry& Entry){ return Entry.StageID == QuestEntry.CurrentStageID; });
    if (!CurrentStageEntry)
    {
        return false;
    }
    const FQuestStage* CurrentStage = &CurrentStageEntry->Stage;
    if (!CurrentStage)
    {
        return false;
    }

    const FBasicQuestObjective* Objective = CurrentStage->Objectives.FindByPredicate([&](const FBasicQuestObjective& Obj) { return Obj.ObjectiveID == ObjectiveID; });
    if (!Objective)
    {
        return false;
    }

    int32 CurrentProgress = 0;
    if (FObjectiveProgressEntry* ObjEntry = QuestEntry.ObjectiveProgress.FindByPredicate([&](const FObjectiveProgressEntry& Entry){ return Entry.ObjectiveID == ObjectiveID; }))
    {
        CurrentProgress = ObjEntry->Progress;
    }
    int32 NewProgress = FMath::Clamp(CurrentProgress + Amount, 0, Objective->RequiredCount);
    QuestLog->UpdateObjectiveProgress(QuestID, ObjectiveID, NewProgress);
    OnQuestObjectiveUpdated.Broadcast(QuestID, ObjectiveID, NewProgress);

    // Check if stage is complete
    bool bStageComplete = true;
    for (const FBasicQuestObjective& StageObjective : CurrentStage->Objectives)
    {
        int32 StageObjectiveProgress = 0;
        if (FObjectiveProgressEntry* ObjEntry = QuestEntry.ObjectiveProgress.FindByPredicate([&](const FObjectiveProgressEntry& Entry){ return Entry.ObjectiveID == StageObjective.ObjectiveID; }))
        {
            StageObjectiveProgress = ObjEntry->Progress;
        }
        if (StageObjectiveProgress < StageObjective.RequiredCount)
        {
            bStageComplete = false;
            break;
        }
    }

    if (bStageComplete)
    {
        CompleteQuestStage(QuestID);
    }

    return true;
}

bool UQuestManagementSubsystem::GetQuestStage(FName QuestID, FQuestStage& OutStage) const
{
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (QuestLog)
    {
        FQuestLogEntry LogEntry;
        if (QuestLog->GetQuestLogEntry(QuestID, LogEntry))
        {
            FQuestData QuestData;
            if (GetQuestData(QuestID, QuestData))
            {
                if (const FQuestStageEntry* StageEntry = QuestData.Stages.FindByPredicate([&](const FQuestStageEntry& Entry){ return Entry.StageID == LogEntry.CurrentStageID; }))
                {
                    OutStage = StageEntry->Stage;
                    return true;
                }
            }
        }
    }
    return false;
}

bool UQuestManagementSubsystem::ActivateQuestStage(FName QuestID, FName StageID)
{
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (QuestLog)
    {
        QuestLog->SetQuestStage(QuestID, StageID);
        OnQuestStateChanged.Broadcast(QuestID, GetQuestState(QuestID));
        return true;
    }
    return false;
}

bool UQuestManagementSubsystem::CompleteQuestStage(FName QuestID)
{
    FQuestStage CurrentStage;
    if (!GetQuestStage(QuestID, CurrentStage))
    {
        return false;
    }

    // Iterate through the branches and find the first one whose conditions are met
    for (const FQuestBranch& Branch : CurrentStage.Branches)
    {
        if (AreBranchConditionsMet(Branch))
        {
            return ActivateQuestStage(QuestID, Branch.NextStageID);
        }
    }

    // If no branch conditions are met, or if there are no branches, check for quest completion
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (QuestLog)
    {
        QuestLog->CompleteQuest(QuestID);
        OnQuestCompleted.Broadcast(QuestID);
        return true;
    }

    return false;
}

bool UQuestManagementSubsystem::GetQuestData(FName QuestID, FQuestData& OutData) const
{
    if (!QuestDataTable)
    {
        return false;
    }

    FQuestData* Row = QuestDataTable->FindRow<FQuestData>(QuestID, TEXT(""));
    if (Row)
    {
        OutData = *Row;
        return true;
    }
    return false;
}

bool UQuestManagementSubsystem::CheckQuestPrerequisites(const FQuestData& QuestData) const
{
    UDAQuestLogComponent* QuestLog = GetPlayerQuestLog();
    if (!QuestLog)
    {
        return false;
    }

    // Check for required items
    ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(GetPlayerQuestLog()->GetOwner());
    if (PlayerCharacter)
    {
        UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
        if (InventoryComponent)
        {
            for (const auto& RequiredItemClass : QuestData.RequiredItems)
            {
                if (RequiredItemClass)
                {
                   ADABaseItem* DefaultItem = RequiredItemClass->GetDefaultObject<ADABaseItem>();
                   if (DefaultItem && !InventoryComponent->HasItem(DefaultItem->GetItemData(), 1))
                   {
                       return false;
                   }
                }
            }
        }
    }

    return true;
}

bool UQuestManagementSubsystem::AreBranchConditionsMet(const FQuestBranch& Branch) const
{
    for (const FQuestCondition& Condition : Branch.Conditions)
    {
        if (!CheckQuestCondition(Condition))
        {
            return false; // If any condition fails, the branch is not taken
        }
    }
    return true; // All conditions met
}

bool UQuestManagementSubsystem::CheckQuestCondition(const FQuestCondition& Condition) const
{
    int32 PlayerValue = 0;
    ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(GetPlayerQuestLog()->GetOwner());
    if (!PlayerCharacter)
    {
        return false;
    }

    switch (Condition.ConditionType)
    {
        case EQuestConditionType::QCT_HasItem:
        {
            UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
            if (InventoryComponent)
            {
               PlayerValue = InventoryComponent->GetItemQuantity(Condition.TargetID);
            }
            break;
        }
        case EQuestConditionType::QCT_SkillLevel:
        {
            UStatlineComponent* StatlineComponent = PlayerCharacter->FindComponentByClass<UStatlineComponent>();
            if (StatlineComponent)
            {
                PlayerValue = StatlineComponent->GetCurrentStatValue(Condition.TargetID);
            }
            break;
        }
        case EQuestConditionType::QCT_FactionReputation:
        {
            USocialSimulationSubsystem* SocialSubsystem = GetGameInstance()->GetSubsystem<USocialSimulationSubsystem>();
            if (SocialSubsystem)
            {
                // Assuming GetCharacterFactionReputation exists and returns an int
                // PlayerValue = SocialSubsystem->GetCharacterFactionReputation(PlayerCharacter->GetCharacterID(), Condition.TargetID);
            }
            break;
        }
        case EQuestConditionType::QCT_QuestCompleted:
        {
            PlayerValue = (GetQuestState(Condition.TargetID) == EQuestState::QS_Completed);
            break;
        }
        default:
            return false;
    }

    switch (Condition.Operator)
    {
        case EComparisonOperator::CO_EqualTo:
            return PlayerValue == Condition.Value;
        case EComparisonOperator::CO_NotEqualTo:
            return PlayerValue != Condition.Value;
        case EComparisonOperator::CO_GreaterThan:
            return PlayerValue > Condition.Value;
        case EComparisonOperator::CO_LessThan:
            return PlayerValue < Condition.Value;
        case EComparisonOperator::CO_GreaterThanOrEqualTo:
            return PlayerValue >= Condition.Value;
        case EComparisonOperator::CO_LessThanOrEqualTo:
            return PlayerValue <= Condition.Value;
        default:
            return false;
    }

}

UDAQuestLogComponent* UQuestManagementSubsystem::GetPlayerQuestLog() const
{
    if (GetGameInstance() && GetGameInstance()->GetFirstLocalPlayerController())
    {
        return GetGameInstance()->GetFirstLocalPlayerController()->FindComponentByClass<UDAQuestLogComponent>();
    }
    return nullptr;
}
TArray<FName> UQuestManagementSubsystem::GetAvailableQuests() const
{
    TArray<FName> AvailableQuests;
    if (!QuestDataTable)
    {
        return AvailableQuests;
    }

    TArray<FName> QuestIDs = QuestDataTable->GetRowNames();
    for (FName QuestID : QuestIDs)
    {
        if (IsQuestAvailable(QuestID))
        {
            AvailableQuests.Add(QuestID);
        }
    }
    return AvailableQuests;
}

TArray<FName> UQuestManagementSubsystem::GetAvailableQuestsInRegion(const FString& RegionID) const
{
    TArray<FName> AvailableQuests;
    if (!QuestDataTable)
    {
        return AvailableQuests;
    }

    TArray<FName> QuestIDs = QuestDataTable->GetRowNames();
    for (FName QuestID : QuestIDs)
    {
        FQuestData QuestData;
        if (GetQuestData(QuestID, QuestData) && IsQuestAvailable(QuestID))
        {
            if (QuestData.RegionID == FName(*RegionID))
            {
                 AvailableQuests.Add(QuestID);
            }
        }
    }
    return AvailableQuests;
}

TArray<FName> UQuestManagementSubsystem::GetAvailableQuestsFromGiver(const FString& QuestGiverID) const
{
    TArray<FName> AvailableQuests;
    if (!QuestDataTable)
    {
        return AvailableQuests;
    }

    TArray<FName> QuestIDs = QuestDataTable->GetRowNames();
    for (FName QuestID : QuestIDs)
    {
        FQuestData QuestData;
        if (GetQuestData(QuestID, QuestData) && IsQuestAvailable(QuestID) && QuestData.QuestGiver.ToString() == QuestGiverID)
        {
            AvailableQuests.Add(QuestID);
        }
    }
    return AvailableQuests;
}

void UQuestManagementSubsystem::UpdateQuestAvailability()
{
    // This could be called when player level, reputation, etc. changes.
    // It could broadcast an event to UI to refresh available quests.
    UE_LOG(LogTemp, Log, TEXT("Quest availability updated."));
}

bool UQuestManagementSubsystem::DoesQuestExist(FName QuestID) const
{
    FQuestData QuestData;
    return GetQuestData(QuestID, QuestData);
}

TArray<FName> UQuestManagementSubsystem::GetQuestsByType(EQuestType QuestType) const
{
    TArray<FName> Quests;
    if (!QuestDataTable)
    {
        return Quests;
    }

    TArray<FName> QuestIDs = QuestDataTable->GetRowNames();
    for (FName QuestID : QuestIDs)
    {
        FQuestData QuestData;
        if (GetQuestData(QuestID, QuestData) && QuestData.QuestType == QuestType)
        {
            Quests.Add(QuestID);
        }
    }
    return Quests;
}

TArray<FName> UQuestManagementSubsystem::GetQuestsByTag(const FString& Tag) const
{
    TArray<FName> Quests;
    if (!QuestDataTable)
    {
        return Quests;
    }

    TArray<FName> QuestIDs = QuestDataTable->GetRowNames();
    for (FName QuestID : QuestIDs)
    {
        FQuestData QuestData;
        if (GetQuestData(QuestID, QuestData) && QuestData.Tags.Contains(FName(*Tag)))
        {
            Quests.Add(QuestID);
        }
    }
    return Quests;
}

FName UQuestManagementSubsystem::GenerateDynamicQuest(EQuestType QuestType, const FString& RegionID)
{
    // This would involve selecting a template, populating it with context-specific data
    // (e.g., random NPC, location in region), and adding it to the quest log.
    UE_LOG(LogTemp, Log, TEXT("Generating dynamic quest of type %s in region %s"), *UEnum::GetValueAsString(QuestType), *RegionID);
    return NAME_None;
}

void UQuestManagementSubsystem::OnWorldTimeUpdate(float CurrentWorldTime)
{
    // Check for timed quests that may have expired.
    // UE_LOG(LogTemp, Log, TEXT("Checking for timed quest updates at time %f"), CurrentWorldTime);
}

void UQuestManagementSubsystem::OnPlayerRegionChanged(const FString& NewRegionID, const FString& PreviousRegionID)
{
    // Could trigger new environmental quests or update existing ones.
    UE_LOG(LogTemp, Log, TEXT("Player changed region from %s to %s. Updating quests."), *PreviousRegionID, *NewRegionID);
}