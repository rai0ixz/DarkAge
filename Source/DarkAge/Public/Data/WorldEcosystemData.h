#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WorldEcosystemData.generated.h"

UENUM(BlueprintType)
enum class EBasicResourceType : uint8
{
	Wood,
	Stone,
	Iron,
	Gold,
	Food,
	Water
};


UENUM(BlueprintType)
enum class ESeasonType : uint8
{
    Spring,
    Summer,
    Autumn,
    Winter
};

UENUM(BlueprintType)
enum class EWeatherPattern : uint8
{
    Clear,
    Cloudy,
    Rain,
    Snow,
    Storm
};

UENUM(BlueprintType)
enum class EEnvironmentalEventType : uint8
{
    Drought,
    Flood,
    Wildfire,
    Blight,
    AnimalMigration
};

UENUM(BlueprintType)
enum class EClimateZoneType : uint8
{
    Temperate,
    Arid,
    Tundra,
    Tropical
};

UENUM(BlueprintType)
enum class EAnimalDiet : uint8
{
    Herbivore,
    Carnivore,
    Omnivore
};

USTRUCT(BlueprintType)
struct FSeasonalData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESeasonType Season = ESeasonType::Spring;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationInDays = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseTemperature = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseHumidity = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EWeatherPattern> CommonWeatherPatterns;
};

USTRUCT(BlueprintType)
struct FEnvironmentalEventData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEnvironmentalEventType EventType = EEnvironmentalEventType::Drought;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Severity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationInDays = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeRemaining = 0.0f;
    FORCEINLINE FEnvironmentalEventData()
        : EventType(EEnvironmentalEventType::Drought)
        , Severity(0.0f)
        , DurationInDays(0.0f)
        , TimeRemaining(0.0f)
    {
    }
};

USTRUCT(BlueprintType)
struct FAnimalPopulationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> AnimalClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Population = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GrowthRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CarryingCapacity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EnvironmentalStress = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanMigrate = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAnimalDiet Diet = EAnimalDiet::Herbivore;
    FORCEINLINE FAnimalPopulationData()
        : AnimalClass(nullptr)
        , Population(0)
        , GrowthRate(0.0f)
        , CarryingCapacity(0)
        , EnvironmentalStress(0.0f)
        , bCanMigrate(false)
        , Diet(EAnimalDiet::Herbivore)
    {
    }
};

USTRUCT(BlueprintType)
struct FResourceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBasicResourceType ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Availability = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RegenerationRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxCapacity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Quality = 0.0f;
    FORCEINLINE FResourceData()
        : ResourceType(EBasicResourceType::Wood)
        , Availability(0.0f)
        , RegenerationRate(0.0f)
        , MaxCapacity(0.0f)
        , Quality(0.0f)
    {
    }

 FORCEINLINE FResourceData(EBasicResourceType InType, float InAvailability, float InRegenRate, float InMaxCapacity, float InQuality)
  : ResourceType(InType)
  , Availability(InAvailability)
  , RegenerationRate(InRegenRate)
  , MaxCapacity(InMaxCapacity)
  , Quality(InQuality)
 {
 }
};

USTRUCT(BlueprintType)
struct FWeatherSystemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeatherPattern CurrentWeather = EWeatherPattern::Clear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WeatherIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeToNextChange = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseChangeInterval = 24.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrentTemperature = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrentHumidity = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Visibility = 1.0f;
    FORCEINLINE FWeatherSystemData()
        : CurrentWeather(EWeatherPattern::Clear)
        , WeatherIntensity(0.0f)
        , TimeToNextChange(0.0f)
        , BaseChangeInterval(24.0f)
        , CurrentTemperature(15.0f)
        , CurrentHumidity(0.5f)
        , WindSpeed(0.0f)
        , Visibility(1.0f)
    {
    }
};

USTRUCT(BlueprintType)
struct FRegionalEcosystemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RegionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EClimateZoneType ClimateZone = EClimateZoneType::Temperate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESeasonType CurrentSeason = ESeasonType::Spring;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DaysSinceSeasonStart = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LastSeasonUpdateTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeatherSystemData WeatherData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LastWeatherUpdateTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAnimalPopulationData> AnimalPopulations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FResourceData> Resources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEnvironmentalEventData> ActiveEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EcosystemHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BiodiversityIndex = 0.0f;
    FORCEINLINE FRegionalEcosystemData()
        : RegionID(NAME_None)
        , ClimateZone(EClimateZoneType::Temperate)
        , CurrentSeason(ESeasonType::Spring)
        , DaysSinceSeasonStart(0.0f)
        , LastSeasonUpdateTime(0.0f)
        , LastWeatherUpdateTime(0.0f)
        , EcosystemHealth(100.0f)
        , BiodiversityIndex(0.0f)
    {
    }
};