#include "Core/DynamicQuestSubsystem.h"
#include "Core/WorldManagementSubsystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Data/FactionData.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Misc/DateTime.h"

UDynamicQuestSubsystem::UDynamicQuestSubsystem()
{
	PossibleQuestTypes = {EDynamicQuestType::Delivery, EDynamicQuestType::Elimination, EDynamicQuestType::Gathering, EDynamicQuestType::Exploration, EDynamicQuestType::Social, EDynamicQuestType::Protection, EDynamicQuestType::Investigation};
}

void UDynamicQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Get reference to world management subsystem
	WorldManagementSubsystem = GetGameInstance()->GetSubsystem<UWorldManagementSubsystem>();
	EconomySubsystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
	
	// Initialize quest templates
	InitializeQuestTemplates();
	
	// Start quest update timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QuestUpdateTimer, this, &UDynamicQuestSubsystem::UpdateActiveQuests, QuestUpdateInterval, true);
	}
	
	UE_LOG(LogTemp, Log, TEXT("DynamicQuestSubsystem initialized"));
}

void UDynamicQuestSubsystem::Deinitialize()
{
	// Clean up timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QuestUpdateTimer);
	}
	
	UE_LOG(LogTemp, Log, TEXT("DynamicQuestSubsystem deinitialized"));
	Super::Deinitialize();
}

FDynamicQuest UDynamicQuestSubsystem::GenerateQuest(const FQuestGenerationParams& Params)
{
	// Select a random quest type from preferred types or all types
	TArray<EDynamicQuestType> AvailableTypes = Params.PreferredTypes.Num() > 0 ?
		Params.PreferredTypes :
		PossibleQuestTypes;

	EDynamicQuestType SelectedType = AvailableTypes[FMath::RandRange(0, AvailableTypes.Num() - 1)];
	
	return GenerateQuestOfType(SelectedType, Params);
}

TArray<FDynamicQuest> UDynamicQuestSubsystem::GenerateQuests(int32 Count, const FQuestGenerationParams& Params)
{
	TArray<FDynamicQuest> GeneratedQuests;
	
	for (int32 i = 0; i < Count; ++i)
	{
		FDynamicQuest NewQuest = GenerateQuest(Params);
		GeneratedQuests.Add(NewQuest);
	}
	
	return GeneratedQuests;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateQuestOfType(EDynamicQuestType QuestType, const FQuestGenerationParams& Params)
{
	FDynamicQuest NewQuest;
	
	// Generate based on quest type
	switch (QuestType)
	{
		case EDynamicQuestType::Delivery:
			NewQuest = GenerateDeliveryQuest(Params);
			break;
		case EDynamicQuestType::Elimination:
			NewQuest = GenerateEliminationQuest(Params);
			break;
		case EDynamicQuestType::Gathering:
			NewQuest = GenerateGatheringQuest(Params);
			break;
		case EDynamicQuestType::Exploration:
			NewQuest = GenerateExplorationQuest(Params);
			break;
		case EDynamicQuestType::Social:
			NewQuest = GenerateSocialQuest(Params);
			break;
		case EDynamicQuestType::Protection:
			NewQuest = GenerateProtectionQuest(Params);
			break;
		case EDynamicQuestType::Investigation:
			NewQuest = GenerateInvestigationQuest(Params);
			break;
		default:
			NewQuest = GenerateDeliveryQuest(Params);
			break;
	}
	
	// Set common properties
	NewQuest.QuestName = GenerateQuestName(QuestType, NewQuest.Difficulty);
	NewQuest.Description = GenerateQuestDescription(NewQuest);
	
	// Calculate rewards
	float RewardAmount = CalculateQuestReward(NewQuest);
	NewQuest.Rewards.Add("Gold", RewardAmount);
	NewQuest.Rewards.Add("Experience", RewardAmount * 0.5f);
	
	UE_LOG(LogTemp, Log, TEXT("Generated quest: %s (%s)"), *NewQuest.QuestName, *UEnum::GetValueAsString(QuestType));
	
	return NewQuest;
}

bool UDynamicQuestSubsystem::AddQuest(const FDynamicQuest& Quest)
{
	if (ActiveQuests.Contains(Quest.QuestID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest with ID %s already exists"), *Quest.QuestID);
		return false;
	}
	
	ActiveQuests.Add(Quest.QuestID, Quest);
	LogQuestEvent(Quest.QuestID, EQuestEventType::Accepted, TEXT("Player"), TEXT(""));
	return true;
}

bool UDynamicQuestSubsystem::RemoveQuest(const FString& QuestID)
{
	return ActiveQuests.Remove(QuestID) > 0;
}

FDynamicQuest UDynamicQuestSubsystem::GetQuest(const FString& QuestID) const
{
	const FDynamicQuest* Quest = ActiveQuests.Find(QuestID);
	return Quest ? *Quest : FDynamicQuest();
}

TArray<FDynamicQuest> UDynamicQuestSubsystem::GetActiveQuests() const
{
	TArray<FDynamicQuest> Quests;
	ActiveQuests.GenerateValueArray(Quests);
	return Quests;
}

TArray<FDynamicQuest> UDynamicQuestSubsystem::GetQuestsByStatus(EDynamicQuestStatus Status) const
{
	TArray<FDynamicQuest> FilteredQuests;
	
	for (const auto& QuestPair : ActiveQuests)
	{
		if (QuestPair.Value.Status == Status)
		{
			FilteredQuests.Add(QuestPair.Value);
		}
	}
	
	return FilteredQuests;
}

TArray<FDynamicQuest> UDynamicQuestSubsystem::GetQuestsInRegion(const FString& RegionID) const
{
	TArray<FDynamicQuest> RegionQuests;
	
	for (const auto& QuestPair : ActiveQuests)
	{
		if (QuestPair.Value.RegionID == RegionID)
		{
			RegionQuests.Add(QuestPair.Value);
		}
	}
	
	return RegionQuests;
}

bool UDynamicQuestSubsystem::UpdateQuestProgress(const FString& QuestID, const FString& ObjectiveKey, float ProgressAmount)
{
	FDynamicQuest* Quest = ActiveQuests.Find(QuestID);
	if (!Quest)
	{
		return false;
	}
	
	// Update progress
	float* CurrentProgress = Quest->Progress.Find(ObjectiveKey);
	if (CurrentProgress)
	{
		*CurrentProgress = FMath::Clamp(*CurrentProgress + ProgressAmount, 0.0f, 100.0f);
	}
	else
	{
		Quest->Progress.Add(ObjectiveKey, FMath::Clamp(ProgressAmount, 0.0f, 100.0f));
	}
	
	// Check if quest is completed
	if (AreObjectivesCompleted(QuestID))
	{
		CompleteQuest(QuestID);
	}
	
	return true;
}

bool UDynamicQuestSubsystem::CompleteQuest(const FString& QuestID)
{
	FDynamicQuest* Quest = ActiveQuests.Find(QuestID);
	if (!Quest)
	{
		return false;
	}
	
	Quest->Status = EDynamicQuestStatus::Completed;
	UE_LOG(LogTemp, Log, TEXT("Quest completed: %s"), *Quest->QuestName);
	LogQuestEvent(QuestID, EQuestEventType::Completed, TEXT("Player"), TEXT(""));
	
	// Award rewards to player
	if (UFactionManagerSubsystem* FactionManager = GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>())
	{
		if (Quest->FactionID != NAME_None)
		{
			// Award reputation based on quest difficulty
			float ReputationChange = 0.0f;
			switch (Quest->Difficulty)
			{
			case EDynamicQuestDifficulty::Easy:
				ReputationChange = 5.0f;
				break;
			case EDynamicQuestDifficulty::Medium:
				ReputationChange = 10.0f;
				break;
			case EDynamicQuestDifficulty::Hard:
				ReputationChange = 20.0f;
				break;
			case EDynamicQuestDifficulty::Extreme:
				ReputationChange = 50.0f;
				break;
			}
			FactionManager->ModifyPlayerReputation(Quest->FactionID, ReputationChange);
		}
	}
	
	return true;
}

bool UDynamicQuestSubsystem::FailQuest(const FString& QuestID)
{
	FDynamicQuest* Quest = ActiveQuests.Find(QuestID);
	if (!Quest)
	{
		return false;
	}
	
	Quest->Status = EDynamicQuestStatus::Failed;
	UE_LOG(LogTemp, Log, TEXT("Quest failed: %s"), *Quest->QuestName);
	LogQuestEvent(QuestID, EQuestEventType::Failed, TEXT("Player"), TEXT(""));
	
	return true;
}

bool UDynamicQuestSubsystem::AreObjectivesCompleted(const FString& QuestID) const
{
	const FDynamicQuest* Quest = ActiveQuests.Find(QuestID);
	if (!Quest)
	{
		return false;
	}
	
	// Check if all objectives are completed
	for (const FQuestObjective& Objective : Quest->Objectives)
	{
		if (!Objective.bIsCompleted)
		{
			return false;
		}
	}
	
	return true;
}

TArray<FDynamicQuest> UDynamicQuestSubsystem::GenerateEventQuests(const FString& EventType, const FVector& EventLocation)
{
	TArray<FDynamicQuest> EventQuests;
	
	FQuestGenerationParams Params;
	
	// Generate context-appropriate quests based on event type
	if (EventType == "BanditAttack")
	{
		Params.PreferredTypes = { EDynamicQuestType::Elimination, EDynamicQuestType::Protection };
		Params.MinDifficulty = EDynamicQuestDifficulty::Medium;
		EventQuests.Add(GenerateQuest(Params));
	}
	else if (EventType == "MerchantArrival")
	{
		Params.PreferredTypes = { EDynamicQuestType::Delivery, EDynamicQuestType::Gathering };
		EventQuests.Add(GenerateQuest(Params));
	}
	else if (EventType == "StrangeOccurrence")
	{
		Params.PreferredTypes = { EDynamicQuestType::Investigation, EDynamicQuestType::Exploration };
		EventQuests.Add(GenerateQuest(Params));
	}
	
	// Set event location for generated quests
	for (FDynamicQuest& Quest : EventQuests)
	{
		Quest.TargetLocation = EventLocation;
		Quest.Status = EDynamicQuestStatus::Available;
	}
	
	return EventQuests;
}

void UDynamicQuestSubsystem::ReactToWorldStateChange(const FString& StateChange, const FString& RegionID)
{
	UE_LOG(LogTemp, Log, TEXT("Reacting to world state change: %s in region %s"), *StateChange, *RegionID);
	
	// Generate appropriate quests based on world state changes
	TArray<FDynamicQuest> NewQuests = GenerateEventQuests(StateChange, FVector::ZeroVector);
	
	for (const FDynamicQuest& Quest : NewQuests)
	{
		AddQuest(Quest);
	}
}

void UDynamicQuestSubsystem::AddQuestTemplate(const FString& TemplateName, const FDynamicQuest& Template)
{
	QuestTemplates.Add(TemplateName, Template);
}

FDynamicQuest UDynamicQuestSubsystem::GenerateFromTemplate(const FString& TemplateName, const FQuestGenerationParams& Params)
{
	const FDynamicQuest* Template = QuestTemplates.Find(TemplateName);
	if (!Template)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest template not found: %s"), *TemplateName);
		return GenerateQuest(Params);
	}
	
	FDynamicQuest NewQuest = *Template;
	NewQuest.QuestID = FGuid::NewGuid().ToString();
	
	// Apply parameters to customize the quest
	if (!Params.TargetRegion.IsEmpty())
	{
		NewQuest.RegionID = Params.TargetRegion;
	}
	
	// Adjust difficulty based on parameters
	if (NewQuest.Difficulty < Params.MinDifficulty)
	{
		NewQuest.Difficulty = Params.MinDifficulty;
	}
	else if (NewQuest.Difficulty > Params.MaxDifficulty)
	{
		NewQuest.Difficulty = Params.MaxDifficulty;
	}
	
	return NewQuest;
}

void UDynamicQuestSubsystem::InitializeQuestTemplates()
{
	// Initialize basic quest templates
	FDynamicQuest DeliveryTemplate;
	DeliveryTemplate.QuestName = "Delivery Quest Template";
	DeliveryTemplate.QuestType = EDynamicQuestType::Delivery;
	// Create proper FQuestObjective structs for delivery template
	FQuestObjective PickupObjective;
	PickupObjective.ObjectiveType = EObjectiveType::Collect;
	PickupObjective.TargetItem = "Package";
	PickupObjective.RequiredCount = 1;
	
	FQuestObjective DeliverObjective;
	DeliverObjective.ObjectiveType = EObjectiveType::Deliver;
	DeliverObjective.TargetItem = "Package";
	DeliverObjective.RequiredCount = 1;
	
	DeliveryTemplate.Objectives = { PickupObjective, DeliverObjective };
	QuestTemplates.Add("BasicDelivery", DeliveryTemplate);
	
	FDynamicQuest GatheringTemplate;
	GatheringTemplate.QuestName = "Gathering Quest Template";
	GatheringTemplate.QuestType = EDynamicQuestType::Gathering;
	// Create proper FQuestObjective struct for gathering template
	FQuestObjective CollectObjective;
	CollectObjective.ObjectiveType = EObjectiveType::Collect;
	CollectObjective.TargetItem = "Resources";
	CollectObjective.RequiredCount = 10;
	
	GatheringTemplate.Objectives = { CollectObjective };
	GatheringTemplate.Requirements.Add("Items", 10);
	QuestTemplates.Add("BasicGathering", GatheringTemplate);
}

void UDynamicQuestSubsystem::UpdateActiveQuests()
{
	GenerateResourceShortageQuests();

	TArray<FString> QuestsToRemove;
	
	for (auto& QuestPair : ActiveQuests)
	{
		FDynamicQuest& Quest = QuestPair.Value;
		
		// Update time-limited quests
		if (Quest.TimeLimit > 0.0f && Quest.Status == EDynamicQuestStatus::Active)
		{
			Quest.RemainingTime -= QuestUpdateInterval;
			
			if (Quest.RemainingTime <= 0.0f)
			{
				Quest.Status = EDynamicQuestStatus::Expired;
				UE_LOG(LogTemp, Log, TEXT("Quest expired: %s"), *Quest.QuestName);
			}
		}
		
		// Clean up completed/failed/expired quests after some time
		if (Quest.Status == EDynamicQuestStatus::Completed || 
			Quest.Status == EDynamicQuestStatus::Failed || 
			Quest.Status == EDynamicQuestStatus::Expired)
		{
			QuestsToRemove.Add(QuestPair.Key);
		}
	}
	
	// Remove old quests
	for (const FString& QuestID : QuestsToRemove)
	{
		RemoveQuest(QuestID);
	}
}

// Helper to log quest events
void UDynamicQuestSubsystem::LogQuestEvent(const FString& QuestID, EQuestEventType EventType, const FString& PlayerID, const FString& ChoiceOrDetail)
{
    FQuestEventLogEntry Entry;
    Entry.QuestID = QuestID;
    Entry.EventType = EventType;
    Entry.PlayerID = PlayerID;
    Entry.ChoiceOrDetail = ChoiceOrDetail;

    if (UWorld* World = GetWorld())
    {
        Entry.Timestamp = World->GetTimeSeconds();
    }
    else
    {
        Entry.Timestamp = FDateTime::UtcNow().ToUnixTimestamp();
    }

    this->QuestEventLog.Add(Entry);
}

void UDynamicQuestSubsystem::LogPlayerQuestChoice(const FString& QuestID, const FString& PlayerID, const FString& ChoiceDetail)
{
	LogQuestEvent(QuestID, EQuestEventType::ChoiceMade, PlayerID, ChoiceDetail);
}

TArray<FQuestEventLogEntry> UDynamicQuestSubsystem::QueryQuestEvents(const FString& QuestID, EQuestEventType EventType, int32 MaxResults) const
{
	TArray<FQuestEventLogEntry> Results;
	for (int32 i = QuestEventLog.Num() - 1; i >= 0 && Results.Num() < MaxResults; --i)
	{
		const FQuestEventLogEntry& Entry = QuestEventLog[i];
		if ((QuestID.IsEmpty() || Entry.QuestID == QuestID) && Entry.EventType == EventType)
		{
			Results.Add(Entry);
		}
	}
	return Results;
}

int32 UDynamicQuestSubsystem::GetQuestEventCount(const FString& QuestID, EQuestEventType EventType) const
{
	int32 Count = 0;
	for (const FQuestEventLogEntry& Entry : QuestEventLog)
	{
		if ((QuestID.IsEmpty() || Entry.QuestID == QuestID) && Entry.EventType == EventType)
		{
			++Count;
		}
	}
	return Count;
}

int32 UDynamicQuestSubsystem::GetQuestTypeCompletionCount(EDynamicQuestType QuestType) const
{
	int32 Count = 0;
	for (const FQuestEventLogEntry& Entry : QuestEventLog)
	{
		if (Entry.EventType == EQuestEventType::Completed)
		{
			const FDynamicQuest* Quest = ActiveQuests.Find(Entry.QuestID);
			if (Quest && Quest->QuestType == QuestType)
			{
				++Count;
			}
		}
	}
	return Count;
}

// Quest generation implementations for each type
FDynamicQuest UDynamicQuestSubsystem::GenerateDeliveryQuest(const FQuestGenerationParams& Params)
{
    FDynamicQuest Quest;
    Quest.QuestType = EDynamicQuestType::Delivery;
    Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
    Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
    // Create proper FQuestObjective structs for delivery quest
    FQuestObjective PickupObjective;
    PickupObjective.ObjectiveType = EObjectiveType::Collect;
    PickupObjective.TargetItem = "Package";
    PickupObjective.RequiredCount = 1;
    
    FQuestObjective DeliverObjective;
    DeliverObjective.ObjectiveType = EObjectiveType::Deliver;
    DeliverObjective.TargetItem = "Package";
    DeliverObjective.RequiredCount = 1;
    
    Quest.Objectives = { PickupObjective, DeliverObjective };
    Quest.TimeLimit = Params.bAllowTimeLimited ? FMath::RandRange(300.0f, 1800.0f) : 0.0f; // 5-30 minutes
    Quest.RemainingTime = Quest.TimeLimit;
    Quest.TargetLocation = FindQuestLocation(Quest.RegionID, FName(""));
    Quest.Status = EDynamicQuestStatus::Available;
    Quest.FactionID = Params.TargetFaction;
    return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateEliminationQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Elimination;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective struct for elimination quest
	FQuestObjective EliminateObjective;
	EliminateObjective.ObjectiveType = EObjectiveType::Kill;
	EliminateObjective.TargetEnemy = "Bandits";
	EliminateObjective.RequiredCount = FMath::RandRange(3, 10);
	
	Quest.Objectives = { EliminateObjective };
	Quest.Requirements.Add("Targets", FMath::RandRange(3, 10));
	   if (WorldManagementSubsystem.IsValid())
	   {
	       float Stability = WorldManagementSubsystem->GetPoliticalStability(Quest.RegionID);
	       if (Stability < 0.4f) // More targets in unstable regions
	       {
	           Quest.Requirements["Targets"] *= 1.5f;
	       }
	   }
	   Quest.TargetLocation = FindQuestLocation(Quest.RegionID, FName(""));
	   Quest.Status = EDynamicQuestStatus::Available;
	   Quest.FactionID = Params.TargetFaction;
	   return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateGatheringQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Gathering;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective struct for gathering quest
	FQuestObjective CollectObjective;
	CollectObjective.ObjectiveType = EObjectiveType::Collect;
	CollectObjective.TargetItem = "Resources";
	CollectObjective.RequiredCount = FMath::RandRange(5, 20);
	
	Quest.Objectives = { CollectObjective };
	Quest.Requirements.Add("Resources", FMath::RandRange(5, 20));
	Quest.Status = EDynamicQuestStatus::Available;
	Quest.FactionID = Params.TargetFaction;
	return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateExplorationQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Exploration;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective struct for exploration quest
	FQuestObjective ExploreObjective;
	ExploreObjective.ObjectiveType = EObjectiveType::Investigate;
	ExploreObjective.TargetLocation = "Unknown Area";
	ExploreObjective.RequiredCount = 1;
	
	Quest.Objectives = { ExploreObjective };
	   if (WorldManagementSubsystem.IsValid())
	   {
	       TMap<FName, FVector> POIs = WorldManagementSubsystem->GetPointsOfInterest(Quest.RegionID);
	       if (POIs.Num() > 0)
	       {
	           TArray<FName> POIKeys;
	           POIs.GetKeys(POIKeys);
	           FName RandomPOI = POIKeys[FMath::RandRange(0, POIKeys.Num() - 1)];
	           Quest.TargetLocation = POIs[RandomPOI];
	           // Create proper FQuestObjective struct for specific POI exploration
	           FQuestObjective SpecificExploreObjective;
	           SpecificExploreObjective.ObjectiveType = EObjectiveType::Investigate;
	           SpecificExploreObjective.TargetLocation = RandomPOI.ToString();
	           SpecificExploreObjective.RequiredCount = 1;
	           
	           Quest.Objectives = { SpecificExploreObjective };
	       }
	   }
	   else
	   {
	       Quest.TargetLocation = FindQuestLocation(Quest.RegionID, FName(""));
	   }
	   Quest.Status = EDynamicQuestStatus::Available;
	   Quest.FactionID = Params.TargetFaction;
	   return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateSocialQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Social;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective struct for social quest
	FQuestObjective TalkObjective;
	TalkObjective.ObjectiveType = EObjectiveType::TalkTo;
	TalkObjective.TargetNPC = "Important Person";
	TalkObjective.RequiredCount = 1;
	
	Quest.Objectives = { TalkObjective };
	Quest.Status = EDynamicQuestStatus::Available;
	Quest.FactionID = Params.TargetFaction;
	return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateProtectionQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Protection;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective struct for protection quest
	FQuestObjective ProtectObjective;
	ProtectObjective.ObjectiveType = EObjectiveType::TalkTo; // Using TalkTo as closest available type
	ProtectObjective.TargetNPC = "Protected Person";
	ProtectObjective.RequiredCount = 1;
	
	Quest.Objectives = { ProtectObjective };
	   Quest.TimeLimit = FMath::RandRange(600.0f, 1800.0f); // 10-30 minutes
	   Quest.RemainingTime = Quest.TimeLimit;
	   if (WorldManagementSubsystem.IsValid())
	   {
	       float Stability = WorldManagementSubsystem->GetPoliticalStability(Quest.RegionID);
	       if (Stability < 0.5f)
	       {
	           Quest.Difficulty = EDynamicQuestDifficulty::Hard;
	       }
	   }
	   Quest.Status = EDynamicQuestStatus::Available;
	   Quest.FactionID = Params.TargetFaction;
	   return Quest;
}

FDynamicQuest UDynamicQuestSubsystem::GenerateInvestigationQuest(const FQuestGenerationParams& Params)
{
	FDynamicQuest Quest;
	Quest.QuestType = EDynamicQuestType::Investigation;
	Quest.Difficulty = static_cast<EDynamicQuestDifficulty>(FMath::RandRange(static_cast<int32>(Params.MinDifficulty), static_cast<int32>(Params.MaxDifficulty)));
	Quest.RegionID = Params.TargetRegion.IsEmpty() ? "DefaultRegion" : Params.TargetRegion;
	// Create proper FQuestObjective structs for investigation quest
	FQuestObjective GatherObjective;
	GatherObjective.ObjectiveType = EObjectiveType::Collect;
	GatherObjective.TargetItem = "Clues";
	GatherObjective.RequiredCount = FMath::RandRange(3, 8);
	
	FQuestObjective SolveObjective;
	SolveObjective.ObjectiveType = EObjectiveType::Investigate;
	SolveObjective.TargetLocation = "Crime Scene";
	SolveObjective.RequiredCount = 1;
	
	Quest.Objectives = { GatherObjective, SolveObjective };
	   Quest.Requirements.Add("Clues", FMath::RandRange(3, 8));
	   if (WorldManagementSubsystem.IsValid())
	   {
	       TMap<FName, FVector> POIs = WorldManagementSubsystem->GetPointsOfInterest(Quest.RegionID);
	       if (POIs.Num() > 0)
	       {
	           TArray<FName> POIKeys;
	           POIs.GetKeys(POIKeys);
	           FName RandomPOI = POIKeys[FMath::RandRange(0, POIKeys.Num() - 1)];
	           Quest.TargetLocation = POIs[RandomPOI];
	           // Create proper FQuestObjective struct for specific POI investigation
	           FQuestObjective InvestigateObjective;
	           InvestigateObjective.ObjectiveType = EObjectiveType::Investigate;
	           InvestigateObjective.TargetLocation = RandomPOI.ToString();
	           InvestigateObjective.RequiredCount = 1;
	           
	           Quest.Objectives.Add(InvestigateObjective);
	       }
	   }
	   Quest.Status = EDynamicQuestStatus::Available;
	   Quest.FactionID = Params.TargetFaction;
	   return Quest;
}

void UDynamicQuestSubsystem::GenerateResourceShortageQuests()
{
	   if (!EconomySubsystem.IsValid() || !WorldManagementSubsystem.IsValid())
	   {
	       return;
	   }

	   TArray<FName> RegionIDs = WorldManagementSubsystem->GetAllRegionIDs();
	   for (const FName& RegionID : RegionIDs)
	   {
	       TArray<FName> LowSupplyItems = EconomySubsystem->GetItemsWithLowSupply(RegionID);
	       for (const FName& ItemID : LowSupplyItems)
	       {
	           FQuestGenerationParams Params;
	           Params.TargetRegion = RegionID.ToString();
	           Params.PreferredTypes = { EDynamicQuestType::Gathering };
	           
	           FDynamicQuest NewQuest = GenerateGatheringQuest(Params);
	           NewQuest.QuestName = FString::Printf(TEXT("Resource Shortage: %s"), *ItemID.ToString());
	           NewQuest.Description = FString::Printf(TEXT("The settlement in %s is running low on %s. Gather some to help them out."), *RegionID.ToString(), *ItemID.ToString());
	           NewQuest.Requirements.Empty();
	           NewQuest.Requirements.Add(ItemID.ToString(), FMath::RandRange(10, 20)); // Request 10-20 of the item

	           AddQuest(NewQuest);
	       }
	   }
}

FString UDynamicQuestSubsystem::GenerateQuestName(EDynamicQuestType QuestType, EDynamicQuestDifficulty Difficulty)
{
	TArray<FString> TypeNames = {
		"Delivery Mission", "Elimination Contract", "Gathering Task", 
		"Exploration Journey", "Social Encounter", "Protection Duty", "Investigation Case"
	};
	
	TArray<FString> DifficultyModifiers = { "Simple", "Challenging", "Dangerous", "Legendary" };
	
	FString TypeName = TypeNames[static_cast<int32>(QuestType)];
	FString DifficultyModifier = DifficultyModifiers[static_cast<int32>(Difficulty)];
	
	return FString::Printf(TEXT("%s %s"), *DifficultyModifier, *TypeName);
}

FString UDynamicQuestSubsystem::GenerateQuestDescription(const FDynamicQuest& Quest)
{
    FString RegionName = Quest.RegionID;
    if (WorldManagementSubsystem.IsValid())
    {
        FRegionData Data = WorldManagementSubsystem->GetRegionData(Quest.RegionID);
        if (!Data.RegionName.IsEmpty())
        {
            RegionName = Data.RegionName.ToString();
        }
    }

    switch (Quest.QuestType)
    {
    case EDynamicQuestType::Delivery:
        return FString::Printf(TEXT("A package needs to be delivered within %s."), *RegionName);
    case EDynamicQuestType::Elimination:
        return FString::Printf(TEXT("Hostile targets are causing trouble in %s. They must be eliminated."), *RegionName);
    case EDynamicQuestType::Gathering:
        return FString::Printf(TEXT("Someone in %s requires resources. Gather the necessary items."), *RegionName);
    case EDynamicQuestType::Exploration:
        if (Quest.Objectives.Num() > 0 && Quest.Objectives[0].TargetLocation.Contains(TEXT("Explore")))
        {
            return FString::Printf(TEXT("An unknown area has been reported in %s. Explore %s."), *RegionName, *Quest.Objectives[0].TargetLocation);
        }
        return FString::Printf(TEXT("Explore the region of %s to uncover its secrets."), *RegionName);
    case EDynamicQuestType::Social:
        return FString::Printf(TEXT("A person of interest in %s needs to be spoken with."), *RegionName);
    case EDynamicQuestType::Protection:
        return FString::Printf(TEXT("A valuable asset in %s is under threat and requires protection."), *RegionName);
    case EDynamicQuestType::Investigation:
        if (Quest.Objectives.Num() > 1 && Quest.Objectives[1].TargetLocation.Contains(TEXT("Investigate")))
        {
            return FString::Printf(TEXT("A mystery is unfolding in %s. Investigate %s."), *RegionName, *Quest.Objectives[1].TargetLocation);
        }
        return FString::Printf(TEXT("Investigate a strange occurrence in %s and report your findings."), *RegionName);
    default:
        return TEXT("Complete the assigned objectives.");
    }
}

float UDynamicQuestSubsystem::CalculateQuestReward(const FDynamicQuest& Quest)
{
	float BaseReward = DefaultQuestReward;
	
	// Difficulty multiplier
	float DifficultyMultiplier = 1.0f + static_cast<float>(Quest.Difficulty) * 0.5f;
	
	// Type multiplier
	float TypeMultiplier = 1.0f;
	switch (Quest.QuestType)
	{
		case EDynamicQuestType::Elimination:
		case EDynamicQuestType::Protection:
			TypeMultiplier = 1.5f; // Combat quests are more rewarding
			break;
		case EDynamicQuestType::Investigation:
		case EDynamicQuestType::Exploration:
			TypeMultiplier = 1.3f; // Complex quests are moderately more rewarding
			break;
		default:
			TypeMultiplier = 1.0f;
			break;
	}
	
	// Time limit bonus
	float TimeBonusMultiplier = Quest.TimeLimit > 0.0f ? 1.2f : 1.0f;
	
	return BaseReward * DifficultyMultiplier * TypeMultiplier * TimeBonusMultiplier;
}

FVector UDynamicQuestSubsystem::FindQuestLocation(const FString& RegionID, FName PointOfInterestID)
{
    if (WorldManagementSubsystem.IsValid())
    {
        FRegionData RegionData = WorldManagementSubsystem->GetRegionData(RegionID);
        if (RegionData.PointsOfInterest.Contains(PointOfInterestID))
        {
            return RegionData.PointsOfInterest[PointOfInterestID];
        }

        if (RegionData.PointsOfInterest.Num() > 0)
        {
            TArray<FName> POIKeys;
            RegionData.PointsOfInterest.GetKeys(POIKeys);
            return RegionData.PointsOfInterest[POIKeys[FMath::RandRange(0, POIKeys.Num() - 1)]];
        }
    }

    // Fallback to random location
    return FVector(FMath::RandRange(-5000.0f, 5000.0f), FMath::RandRange(-5000.0f, 5000.0f), 0.0f);
}