#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/PlayerLegacyData.h"
#include "PlayerLegacySubsystem.generated.h"

UCLASS()
class DARKAGE_API UPlayerLegacySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Legacy Management
    UFUNCTION(BlueprintCallable, Category = "Legacy")
    void AddLegacyPoints(const FString& PlayerID, int32 PointsToAdd);

    UFUNCTION(BlueprintPure, Category = "Legacy")
    FLegacyData GetLegacyDataForPlayer(const FString& PlayerID) const;

    // World Event Management
    UFUNCTION(BlueprintCallable, Category = "Legacy")
    void RecordWorldEvent(const FString& PlayerID, const FString& EventName, const FString& EventDescription);

    UFUNCTION(BlueprintPure, Category = "Legacy")
    TArray<FWorldEventData> GetAllWorldEvents() const;
   
    UFUNCTION(BlueprintPure, Category = "Legacy")
    bool HasVisitedRegion(FName RegionID) const;
   
   protected:
    UPROPERTY()
    TMap<FString, FLegacyData> PlayerLegacyData;

    UPROPERTY()
    TArray<FWorldEventData> WorldEvents;
};