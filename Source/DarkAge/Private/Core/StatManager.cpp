#include "Core/StatManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"

void UStatManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FString SkillDefinitionsDataPath = TEXT("/Game/_DA/Data/DT_SkillDefinitions.DT_SkillDefinitions");
    SkillDefinitionsDataTable = LoadObject<UDataTable>(nullptr, *SkillDefinitionsDataPath);
    if (!SkillDefinitionsDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load SkillDefinitionsDataTable at path: %s"), *SkillDefinitionsDataPath);
    }
}

void UStatManager::AddStat(FName StatName, int32 Amount)
{
    for (FTrackedStat& Stat : Stats)
    {
        if (Stat.StatName == StatName)
        {
            Stat.Value += Amount;
            return;
        }
    }
    FTrackedStat NewStat;
    NewStat.StatName = StatName;
    NewStat.Value = Amount;
    Stats.Add(NewStat);
}

int32 UStatManager::GetStat(FName StatName) const
{
    for (const FTrackedStat& Stat : Stats)
    {
        if (Stat.StatName == StatName)
        {
            return Stat.Value;
        }
    }
    return 0;
}

const TArray<FTrackedStat>& UStatManager::GetAllStats() const
{
    return Stats;
}
