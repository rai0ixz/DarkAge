#pragma once

#include "CoreMinimal.h"
#include "RegionState.generated.h"

USTRUCT(BlueprintType)
struct FRegionState
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    FName RegionID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    FName ControllingFaction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    int32 Population;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float Prosperity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float CrimeRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float DiseaseRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    TMap<FName, float> ResourceLevels;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    TArray<FString> RecentEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    bool bIsDiscovered;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float PlayerReputationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    FString CurrentWeather;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float LastVisitTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float ActiveBountyValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float CrimeHeatLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    float SupplyChainDisruption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region State")
    TArray<FName> CurrentEvents;
    
    FRegionState()
        : RegionID(NAME_None)
        , ControllingFaction(NAME_None)
        , Population(1000)
        , Prosperity(0.5f)
        , CrimeRate(0.2f)
        , DiseaseRate(0.1f)
        , bIsDiscovered(false)
        , PlayerReputationLevel(0.f)
        , CurrentWeather(TEXT("Clear"))
        , LastVisitTime(0.f)
        , ActiveBountyValue(0.f)
        , CrimeHeatLevel(0.f)
        , SupplyChainDisruption(0.f)
    {
    }
};