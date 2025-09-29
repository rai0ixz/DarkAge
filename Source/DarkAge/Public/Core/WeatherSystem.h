#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/WeatherData.h"
#include "WeatherSystem.generated.h"

UCLASS()
class DARKAGE_API UWeatherSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintPure, Category = "Weather")
    EWeatherType GetCurrentWeather() const;

    UFUNCTION(BlueprintPure, Category = "Weather")
    float GetCurrentTemperature() const;

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetWeather(EWeatherType NewWeather);

private:
    void UpdateWeather();

    UPROPERTY()
    EWeatherType CurrentWeather;

    UPROPERTY()
    float CurrentTemperature;

    FTimerHandle WeatherUpdateTimer;
};