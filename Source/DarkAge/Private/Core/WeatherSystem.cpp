#include "Core/WeatherSystem.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "GAS/DASurvivalAttributeSet.h"

void UWeatherSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentWeather = EWeatherType::Clear;
    CurrentTemperature = 70.0f;

    // Start weather updates
    if (UWorld* World = GetWorld())
    {
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &UWeatherSystem::UpdateWeather);
        World->GetTimerManager().SetTimer(
            WeatherUpdateTimer,
            TimerDelegate,
            60.0f, // Update every 60 seconds
            true
        );
    }
}

void UWeatherSystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(WeatherUpdateTimer);
    }
    Super::Deinitialize();
}

EWeatherType UWeatherSystem::GetCurrentWeather() const
{
    return CurrentWeather;
}

float UWeatherSystem::GetCurrentTemperature() const
{
    return CurrentTemperature;
}

void UWeatherSystem::SetWeather(EWeatherType NewWeather)
{
    CurrentWeather = NewWeather;
}

void UWeatherSystem::UpdateWeather()
{
    // In a real implementation, this would be a complex simulation.
    // For now, we'll just randomly change the weather.
    int32 WeatherTypeCount = (int32)EWeatherType::Fog + 1;
    EWeatherType NewWeather = (EWeatherType)(FMath::RandRange(0, WeatherTypeCount - 1));
    this->SetWeather(NewWeather);

    // Apply weather effects to all characters
    // TODO: Get all characters and apply effects
}

void UWeatherSystem::ApplyWeatherEffectsToCharacter(AActor* Character)
{
    if (!Character) return;

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
    if (!ASC) return;

    // Remove previous weather effects
    // TODO: Track and remove old effects

    // Apply new weather effects based on current weather
    switch (CurrentWeather)
    {
    case EWeatherType::Rain:
        // Apply wetness effect, increased thirst depletion
        // Create and apply a GameplayEffect for rain
        break;
    case EWeatherType::Snow:
        // Apply cold effect, warmth depletion
        break;
    case EWeatherType::Storm:
        // Apply storm effects, damage over time
        break;
    case EWeatherType::Fog:
        // Apply visibility reduction, perhaps debuff to perception
        break;
    case EWeatherType::Clear:
    default:
        // No effects
        break;
    }
}