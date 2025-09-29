#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/EnvironmentalFactorsData.h"
#include "WeatherData.generated.h"

UENUM(BlueprintType)
enum class ESeason : uint8
{
    Spring,
    Summer,
    Autumn,
    Winter
};

USTRUCT(BlueprintType)
struct DARKAGE_API FWeatherData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    EWeatherType WeatherType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float MinTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float MaxTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WindSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float Precipitation;
    FORCEINLINE FWeatherData()
        : WeatherType(EWeatherType::Clear)
        , MinTemperature(0.0f)
        , MaxTemperature(0.0f)
        , WindSpeed(0.0f)
        , Precipitation(0.0f)
    {
    }
};