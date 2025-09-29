// EnvironmentalFactorsData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentalFactorsData.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    None        UMETA(DisplayName = "None"),
    Clear       UMETA(DisplayName = "Clear"),
    Cloudy      UMETA(DisplayName = "Cloudy"),
    Rain        UMETA(DisplayName = "Rain"),
    Storm       UMETA(DisplayName = "Storm"),
    Snow        UMETA(DisplayName = "Snow"),
    Fog         UMETA(DisplayName = "Fog")
};

/**
 * Environmental Effect Types
 */
UENUM(BlueprintType)
enum class EEnvironmentalEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    Temperature     UMETA(DisplayName = "Temperature"),    // Hot/cold effects
    Weather         UMETA(DisplayName = "Weather"),        // Rain, snow, wind effects
    Exposure        UMETA(DisplayName = "Exposure"),       // Lack of shelter
    Altitude        UMETA(DisplayName = "Altitude"),       // High/low altitude effects
    Humidity        UMETA(DisplayName = "Humidity"),       // Wet/dry conditions
    AirQuality      UMETA(DisplayName = "Air Quality"),    // Pollution, smoke, toxic air
    Radiation       UMETA(DisplayName = "Radiation"),      // Magical or natural radiation
    Terrain         UMETA(DisplayName = "Terrain")         // Swamp, desert, arctic effects
};

/**
 * Temperature Ranges
 */
UENUM(BlueprintType)
enum class ETemperatureRange : uint8
{
    Freezing        UMETA(DisplayName = "Freezing"),       // Below 32°F / 0°C
    Cold            UMETA(DisplayName = "Cold"),           // 32-50°F / 0-10°C
    Cool            UMETA(DisplayName = "Cool"),           // 50-60°F / 10-15°C
    Comfortable     UMETA(DisplayName = "Comfortable"),    // 60-80°F / 15-27°C
    Warm            UMETA(DisplayName = "Warm"),           // 80-90°F / 27-32°C
    Hot             UMETA(DisplayName = "Hot"),            // 90-100°F / 32-38°C
    Scorching       UMETA(DisplayName = "Scorching")       // Above 100°F / 38°C
};

/**
 * Shelter Quality Levels
 */
UENUM(BlueprintType)
enum class EShelterQuality : uint8
{
    None            UMETA(DisplayName = "None"),           // No shelter
    Minimal         UMETA(DisplayName = "Minimal"),        // Basic cover (trees, overhang)
    Basic           UMETA(DisplayName = "Basic"),          // Simple shelter (tent, lean-to)
    Good            UMETA(DisplayName = "Good"),           // Solid shelter (cabin, house)
    Excellent       UMETA(DisplayName = "Excellent"),      // Superior shelter (castle, manor)
    Perfect         UMETA(DisplayName = "Perfect")         // Ideal environment (palace, magical protection)
};

/**
 * Environmental Resistance Information
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FEnvironmentalResistance
{
    GENERATED_BODY()

    FEnvironmentalResistance()
        : EffectType(EEnvironmentalEffectType::None)
        , ResistanceValue(0.0f)
        , bIsPercentage(true)
        , Duration(0.0f)
        , bIsPermanent(false)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance") EEnvironmentalEffectType EffectType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance") float ResistanceValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance") bool bIsPercentage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance", meta = (ClampMin = "0.0")) float Duration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance") bool bIsPermanent;
};

/**
 * Environmental Effect Definition
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FEnvironmentalEffectData : public FTableRowBase
{
    GENERATED_BODY()

    FEnvironmentalEffectData()
        : EffectType(EEnvironmentalEffectType::None)
        , BaseSeverity(1.0f)
        , ApplicationRate(1.0f)
        , AccumulationRate(0.0f)
        , MaxAccumulation(100.0f)
        , DecayRate(0.0f)
        , bRequiresContinuousExposure(true)
        , bCanKill(false)
        , MinimumExposureTime(0.0f)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info") FName EffectID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info") FText EffectName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info") FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info") EEnvironmentalEffectType EffectType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float BaseSeverity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float ApplicationRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float AccumulationRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float MaxAccumulation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float DecayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters") bool bRequiresContinuousExposure;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters") bool bCanKill;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Parameters", meta = (ClampMin = "0.0")) float MinimumExposureTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects") TArray<FName> StatusEffectsToApply;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance") TArray<FEnvironmentalResistance> ResistanceFactors;
};

/**
 * Active Environmental Effect Instance
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FActiveEnvironmentalEffect
{
    GENERATED_BODY()

    FActiveEnvironmentalEffect()
        : CurrentSeverity(0.0f)
        , AccumulatedLevel(0.0f)
        , ExposureTime(0.0f)
        , LastApplicationTime(0.0f)
        , bIsActive(true)
        , bHasMinimumExposure(false)
        , SourceLocation(FVector::ZeroVector)
        , Severity(0.0f) // Added based on CPP errors
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    FDataTableRowHandle EffectDataRowHandle; // Changed from FEnvironmentalEffectData*

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    FName EffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    float CurrentSeverity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    float AccumulatedLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    float ExposureTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    float LastApplicationTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    bool bHasMinimumExposure;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    TArray<FName> AppliedStatusEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    FVector SourceLocation;

    // Added based on CPP error: D:\WorkStation\RaioCore\DarkAge\Source\DarkAge\Private\Components\EnvironmentalFactorsComponent.cpp(886): error C2039: 'Severity': is not a member of 'FActiveEnvironmentalEffect'
    // This likely refers to the specific instance's current severity, which might be different from the base severity in FEnvironmentalEffectData
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Effect")
    float Severity;
};

/**
 * Environmental Condition Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FEnvironmentalConditionData
{
    GENERATED_BODY()

    FEnvironmentalConditionData()
        : Temperature(70.0f)
        , TemperatureRange(ETemperatureRange::Comfortable)
        , Humidity(0.5f)
        , WindSpeed(0.0f)
        , Precipitation(0.0f)
        , ShelterQuality(EShelterQuality::None)
        , AirQuality(1.0f)
        , Visibility(1.0f)
        , bIsIndoors(false)
        , bHasWind(false)
        , bIsRaining(false)
        , bIsSnowing(false)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") float Temperature;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") ETemperatureRange TemperatureRange;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions", meta = (ClampMin = "0.0", ClampMax = "1.0")) float Humidity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions", meta = (ClampMin = "0.0")) float WindSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions", meta = (ClampMin = "0.0", ClampMax = "1.0")) float Precipitation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") EShelterQuality ShelterQuality;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions", meta = (ClampMin = "0.0", ClampMax = "1.0")) float AirQuality;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions", meta = (ClampMin = "0.0", ClampMax = "1.0")) float Visibility;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") bool bIsIndoors;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") bool bHasWind;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") bool bIsRaining;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") bool bIsSnowing;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") TArray<FName> ActiveWeatherEffects;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Conditions") TMap<FName, float> EnvironmentalModifiers;
};

/**
 * Shelter Information
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FShelterData
{
    GENERATED_BODY()

    FShelterData()
        : Quality(EShelterQuality::None)
        , Capacity(1)
        , TemperatureProtection(0.0f)
        , WeatherProtection(0.0f)
        , ComfortLevel(0.0f)
        , bProvidesWarmth(false)
        , bProvidesCooling(false)
        , bProvidesVentilation(false)
        , bIsSecure(false)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") FName ShelterID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") FText ShelterName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") EShelterQuality Quality;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter", meta = (ClampMin = "1")) int32 Capacity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter", meta = (ClampMin = "0.0", ClampMax = "1.0")) float TemperatureProtection;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter", meta = (ClampMin = "0.0", ClampMax = "1.0")) float WeatherProtection;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter", meta = (ClampMin = "0.0", ClampMax = "1.0")) float ComfortLevel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") bool bProvidesWarmth;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") bool bProvidesCooling;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") bool bProvidesVentilation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") bool bIsSecure;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shelter") TArray<FEnvironmentalResistance> ProvidedResistances;
};

USTRUCT(BlueprintType)
struct DARKAGE_API FEnvironmentalFactors
{
    GENERATED_BODY()

    FEnvironmentalFactors()
        : AirTemperature(0.0f)
        , Weather(EWeatherType::Clear)
    {
    }

    UPROPERTY(BlueprintReadWrite, Category = "Environmental Factors")
    float AirTemperature;

    UPROPERTY(BlueprintReadWrite, Category = "Environmental Factors")
    EWeatherType Weather;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnvironmentalEffectApplied, FName, EffectID, float, Severity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnvironmentalEffectRemoved, FName, EffectID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShelterStatusChanged, EShelterQuality, NewQuality, EShelterQuality, OldQuality);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTemperatureChanged, ETemperatureRange, NewRange, float, Temperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExposureRiskChanged, EEnvironmentalEffectType, EffectType, float, RiskLevel);