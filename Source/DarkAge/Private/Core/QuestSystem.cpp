#include "Core/QuestSystem.h"
#include "DarkAge.h"
#include "Data/QuestData.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Kismet/GameplayStatics.h"

UQuestSystem::UQuestSystem()
{
    TimeSinceLastGeneration = 0.f;
}

void UQuestSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EconomySubsystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
	FactionManagerSubsystem = GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
}

void UQuestSystem::Deinitialize()
{
    Super::Deinitialize();
}

void UQuestSystem::UpdateQuests(float DeltaTime)
{
    TimeSinceLastGeneration += DeltaTime;
    if (TimeSinceLastGeneration > 30.f) // Generate new quests every 30 seconds
    {
        GenerateDynamicQuests();
        TimeSinceLastGeneration = 0.f;
    }

    CheckQuestExpiration();
}

FName UQuestSystem::GenerateQuest(EQuestType Type, FName Region, int32 Difficulty)
{
    return NAME_None;
}

bool UQuestSystem::AcceptQuest(FName QuestID)
{
    UE_LOG(LogTemp, Warning, TEXT("[QuestSystem] Attempting to accept quest: %s"), *QuestID.ToString());
    // Accept a quest: set state to InProgress if available
    if (FQuestData* Quest = Quests.Find(QuestID))
    {
        if (Quest->State == EQuestState::QS_NotStarted)
        {
            Quest->State = EQuestState::QS_InProgress;
            OnQuestStatusChanged.Broadcast(QuestID, Quest->State);
            UE_LOG(LogTemp, Warning, TEXT("[QuestSystem] Quest accepted: %s"), *QuestID.ToString());
            return true;
        }
    }
    return false;
}

bool UQuestSystem::AbandonQuest(FName QuestID)
{
    // This logic is now handled by the DAQuestLogComponent on the player.
    return false;
}

bool UQuestSystem::CompleteQuest(FName QuestID)
{
    // This logic is now handled by the DAQuestLogComponent on the player.
    return false;
}

bool UQuestSystem::FailQuest(FName QuestID)
{
    // This logic is now handled by the DAQuestLogComponent on the player.
    return false;
}

bool UQuestSystem::UpdateObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Progress)
{
    // This logic is now handled by the DAQuestLogComponent on the player.
    return false;
}

TArray<FQuestData> UQuestSystem::GetAvailableQuests() const
{
    // This would need to be implemented based on player location, level, etc.
    return TArray<FQuestData>();
}

TArray<FQuestData> UQuestSystem::GetActiveQuests() const
{
    // This logic is now on the DAQuestLogComponent.
    return TArray<FQuestData>();
}

FQuestData UQuestSystem::GetQuestData(FName QuestID) const
{
    if (const FQuestData* Quest = Quests.Find(QuestID))
    {
        return *Quest;
    }
    return FQuestData();
}

bool UQuestSystem::AreAllObjectivesCompleted(FName QuestID) const
{
    // This logic is now on the DAQuestLogComponent.
    return false;
}

FName UQuestSystem::GenerateUniqueQuestID() const
{
    return FName(*FString::Printf(TEXT("QST_%d"), FMath::Rand()));
}

FQuestData UQuestSystem::GenerateEconomicQuest(FName Region, int32 Difficulty)
{
    return FQuestData();
}

FQuestData UQuestSystem::GeneratePoliticalQuest(FName Region, int32 Difficulty)
{
    return FQuestData();
}

FQuestData UQuestSystem::GenerateSocialQuest(FName Region, int32 Difficulty)
{
    return FQuestData();
}

FQuestData UQuestSystem::GenerateEnvironmentalQuest(FName Region, int32 Difficulty)
{
    return FQuestData();
}

void UQuestSystem::CheckQuestExpiration()
{
    // This is a simplified example. A real implementation would need to store the expiration time.
    for (auto& Elem : Quests)
    {
        if (Elem.Value.State == EQuestState::QS_InProgress)
        {
            // Assuming a simple expiration condition for demonstration
            if (false) // Replace with actual expiration logic
            {
                Elem.Value.State = EQuestState::QS_Failed;
                OnQuestStatusChanged.Broadcast(Elem.Key, Elem.Value.State);
            }
        }
    }
}

void UQuestSystem::GenerateDynamicQuests()
{
    // In a real game, this would be a complex system based on world state, player actions, etc.
    // For now, we'll just generate one of each type of quest in a test region.
    FName PlayerRegion = GetCurrentPlayerRegion();
    GenerateQuest(EQuestType::QT_Event, PlayerRegion, 1);
}

FName UQuestSystem::GetCurrentPlayerRegion() const
{
    // This is a placeholder. In a real game, you would get the player's current region from the game state.
    return FName(TEXT("Heartlands"));
}

// Blueprint helper: Generate and accept a test quest in one call (for debugging)
FName UQuestSystem::GenerateAndAcceptTestQuest()
{
    FName QuestID = GenerateQuest(EQuestType::QT_SideQuest, FName(TEXT("TestRegion")), 1);
    AcceptQuest(QuestID);
    UE_LOG(LogTemp, Warning, TEXT("[QuestSystem] GenerateAndAcceptTestQuest called. QuestID: %s"), *QuestID.ToString());
    return QuestID;
   }
   
   FQuestData* UQuestSystem::GetMutableQuestData(FName QuestID)
   {
       return Quests.Find(QuestID);
   }