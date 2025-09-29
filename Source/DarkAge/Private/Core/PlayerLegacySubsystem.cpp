#include "Core/PlayerLegacySubsystem.h"

void UPlayerLegacySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UPlayerLegacySubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UPlayerLegacySubsystem::AddLegacyPoints(const FString& PlayerID, int32 PointsToAdd)
{
    FLegacyData& LegacyData = PlayerLegacyData.FindOrAdd(PlayerID);
    LegacyData.LegacyPoints += PointsToAdd;
}

FLegacyData UPlayerLegacySubsystem::GetLegacyDataForPlayer(const FString& PlayerID) const
{
    return PlayerLegacyData.FindRef(PlayerID);
}

void UPlayerLegacySubsystem::RecordWorldEvent(const FString& PlayerID, const FString& EventName, const FString& EventDescription)
{
    FWorldEventData NewEvent;
    NewEvent.EventID = FGuid::NewGuid();
    NewEvent.PlayerResponsible = PlayerID;
    NewEvent.EventName = EventName;
    NewEvent.Description = EventDescription;
    NewEvent.Timestamp = FDateTime::UtcNow().ToUnixTimestamp();
    WorldEvents.Add(NewEvent);
}

TArray<FWorldEventData> UPlayerLegacySubsystem::GetAllWorldEvents() const
{
    return WorldEvents;
}
bool UPlayerLegacySubsystem::HasVisitedRegion(FName RegionID) const
{
    // This is a placeholder implementation. A real implementation would require a way to track visited regions.
    return true;
}