#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Data/EnvironmentalFactorsData.h"
#include "AdvancedWeatherImpactSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FSurvivalMultipliers
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HungerRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ThirstRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WarmthLoss = 1.0f;

    FSurvivalMultipliers()
    {
        HungerRate = 1.0f;
        ThirstRate = 1.0f;
        WarmthLoss = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct FWeatherImpactData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSurvivalMultipliers SurvivalMultipliers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovementSpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VisibilityRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CropGrowthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TradeEfficiencyMultiplier = 1.0f;

    FWeatherImpactData()
    {
        MovementSpeedMultiplier = 1.0f;
        VisibilityRange = 1000.0f;
        CropGrowthMultiplier = 1.0f;
        TradeEfficiencyMultiplier = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct FEnvironmentalModifiers
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CropGrowthRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VisibilityRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovementSpeedMultiplier = 1.0f;

    FEnvironmentalModifiers()
    {
        CropGrowthRate = 1.0f;
        VisibilityRange = 1000.0f;
        MovementSpeedMultiplier = 1.0f;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAdvancedWeatherChanged, EWeatherType, NewWeatherType, float, Intensity);

/**
 * Advanced Weather Impact Subsystem
 * Provides sophisticated weather simulation with gameplay impacts on survival, NPCs, economy, and environment
 */
UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedWeatherImpactSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedWeatherImpactSubsystem, STATGROUP_Tickables); }

    // Weather Control Functions
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void ForceWeatherChange(EWeatherType NewWeatherType, float Intensity = 0.5f);

    // Weather Query Functions
    UFUNCTION(BlueprintPure, Category = "Weather")
    EWeatherType GetCurrentWeatherType() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    float GetCurrentWeatherIntensity() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    float GetCurrentTemperature() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    bool IsWeatherSevere() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    float GetWeatherSeverityLevel() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    FEnvironmentalModifiers GetCurrentEnvironmentalModifiers() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Weather Events")
    FOnAdvancedWeatherChanged OnWeatherChanged;

protected:
    // Weather system initialization
    void InitializeWeatherEffects();

    // Weather update functions
    void UpdateWeatherEffects();
    void UpdateWeatherConditions();

    // Weather application functions
    void ApplyWeatherToSurvival();
    void ApplyWeatherToNPCs();
    void ApplyWeatherToEconomy();
    void ApplyEnvironmentalEffects();
    void ApplyContinuousWeatherEffects(float DeltaTime);

    // Helper functions
    float GetSeasonalWeatherFactor() const;
    float GetSeasonalTemperature() const;

private:
    // Weather state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather State", meta = (AllowPrivateAccess = "true"))
    EWeatherType CurrentWeatherType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather State", meta = (AllowPrivateAccess = "true"))
    float CurrentWeatherIntensity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather State", meta = (AllowPrivateAccess = "true"))
    float CurrentTemperature;

    // Weather effects data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather Configuration", meta = (AllowPrivateAccess = "true"))
    TMap<EWeatherType, FWeatherImpactData> WeatherEffects;

    // Current environmental modifiers
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather State", meta = (AllowPrivateAccess = "true"))
    FEnvironmentalModifiers CurrentEnvironmentalModifiers;

    // Update timing
    float WeatherUpdateTimer;
};