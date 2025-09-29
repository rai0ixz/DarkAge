
#pragma once

#include "CoreMinimal.h"
#include "ClimateData.h"
#include "AdvancedSurvivalData.generated.h"

UENUM(BlueprintType)
enum class ESurvivalNeedType : uint8
{
    Hunger      UMETA(DisplayName = "Hunger"),
    Thirst      UMETA(DisplayName = "Thirst")
};

UENUM(BlueprintType)
enum class ESurvivalDiseaseType : uint8
{
    None        UMETA(DisplayName = "None"),
    TheShakes   UMETA(DisplayName = "The Shakes")
};

USTRUCT(BlueprintType)
struct FSurvivalNeedConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
    float MaxValue = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
    float DecrementRate = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
    float CriticalThreshold = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
    float HealthPenaltyRate = 0.5f;
};

USTRUCT(BlueprintType)
struct FDiseaseConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    float DexterityPenalty = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    float StaminaDrainRate = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    float ShiverCheckInterval = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Disease")
    float ChanceToShiver = 0.f;
};

USTRUCT(BlueprintType)
struct FClimateSurvivalInfo
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Climate")
   TMap<ESurvivalDiseaseType, float> DiseaseRisk;
};

USTRUCT(BlueprintType)
struct FSurvivalState
{
    GENERATED_BODY()

    UPROPERTY()
    float CurrentHunger = 100.f;

    UPROPERTY()
    float CurrentThirst = 100.f;

    UPROPERTY()
    TMap<ESurvivalDiseaseType, bool> ActiveDiseases;

    UPROPERTY()
    float TimeSinceLastShiverCheck = 0.f;
};
