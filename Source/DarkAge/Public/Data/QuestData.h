#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Items/DABaseItem.h"
#include "QuestData.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	QS_NotStarted UMETA(DisplayName = "Not Started"),
	QS_InProgress UMETA(DisplayName = "In Progress"),
	QS_Completed UMETA(DisplayName = "Completed"),
	QS_Failed UMETA(DisplayName = "Failed"),
	QS_Abandoned UMETA(DisplayName = "Abandoned")
};

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	QT_MainStory UMETA(DisplayName = "Main Story"),
	QT_SideQuest UMETA(DisplayName = "Side Quest"),
	QT_Task UMETA(DisplayName = "Task"),
	QT_Event UMETA(DisplayName = "Event"),
	Survival UMETA(DisplayName = "Survival"),
	Combat UMETA(DisplayName = "Combat")
};

UENUM(BlueprintType)
enum class EQuestConditionType : uint8
{
	QCT_HasItem UMETA(DisplayName = "Has Item"),
	QCT_SkillLevel UMETA(DisplayName = "Skill Level"),
	QCT_FactionReputation UMETA(DisplayName = "Faction Reputation"),
	QCT_QuestCompleted UMETA(DisplayName = "Quest Completed")
};

UENUM(BlueprintType)
enum class EComparisonOperator : uint8
{
	CO_EqualTo UMETA(DisplayName = "Equal To"),
	CO_NotEqualTo UMETA(DisplayName = "Not Equal To"),
	CO_GreaterThan UMETA(DisplayName = "Greater Than"),
	CO_LessThan UMETA(DisplayName = "Less Than"),
	CO_GreaterThanOrEqualTo UMETA(DisplayName = "Greater Than or Equal To"),
	CO_LessThanOrEqualTo UMETA(DisplayName = "Less Than or Equal To")
};

USTRUCT(BlueprintType)
struct FBasicQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ObjectiveID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredCount = 0;
};

USTRUCT(BlueprintType)
struct FQuestCondition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQuestConditionType ConditionType = EQuestConditionType::QCT_HasItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EComparisonOperator Operator = EComparisonOperator::CO_EqualTo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FQuestBranch
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NextStageID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FQuestCondition> Conditions;
};

USTRUCT(BlueprintType)
struct FQuestStage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StageID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBasicQuestObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuestBranch> Branches;
};

USTRUCT(BlueprintType)
struct FFactionReputationReward
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FactionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ReputationChange = 0;
};

USTRUCT(BlueprintType)
struct FItemReward
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity = 0;
};

USTRUCT(BlueprintType)
struct FQuestStageEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName StageID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FQuestStage Stage;
};

USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestType QuestType = EQuestType::QT_SideQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InitialStage = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuestStageEntry> Stages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ADABaseItem>> RequiredItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QuestGiver;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  FName RegionID;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  TArray<FName> Tags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EQuestState State = EQuestState::QS_NotStarted;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, FName, QuestID, EQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestObjectiveUpdated, FName, QuestID, FName, ObjectiveID, int32, NewProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStarted, FName, QuestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, FName, QuestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed, FName, QuestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAbandoned, FName, QuestID);