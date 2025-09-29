#include "Core/WorldEpochSubsystem.h"
#include "Core/QuestManagementSubsystem.h"

void UWorldEpochSubsystem::AdvanceAge()
{
    if (CurrentEpochIndex + 1 < Epochs.Num())
    {
        ++CurrentEpochIndex;
        const FEpochData& NewEpoch = Epochs[CurrentEpochIndex];

        // Broadcast new age event for notification system
        OnEpochAdvanced.Broadcast(NewEpoch);

        UE_LOG(LogTemp, Log, TEXT("Advanced to new epoch: %s"), *NewEpoch.EpochName.ToString());

        // Example of unlocking content:
        // This could unlock new quests, technologies, building types, etc.
        UQuestManagementSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestManagementSubsystem>();
        if (QuestSubsystem)
        {
            // A function like this would need to be implemented in the Quest subsystem
            // QuestSubsystem->UnlockQuestsForEpoch(NewEpoch.EpochName);
            UE_LOG(LogTemp, Log, TEXT("Unlocking quests for epoch %s"), *NewEpoch.EpochName.ToString());
        }

        // Further world state changes could be triggered here,
        // such as modifying faction relations, changing NPC inventories,
        // or altering the physical world map.
    }
}

FEpochData UWorldEpochSubsystem::GetCurrentEpoch() const
{
    if (Epochs.IsValidIndex(CurrentEpochIndex))
    {
        return Epochs[CurrentEpochIndex];
    }
    return FEpochData();
}
