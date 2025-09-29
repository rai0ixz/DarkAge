#include "EnvironmentalFactorsComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/StatusEffectComponent.h"
#include "Core/WeatherSystem.h"

UEnvironmentalFactorsComponent::UEnvironmentalFactorsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEnvironmentalFactorsComponent::BeginPlay()
{
    Super::BeginPlay();

    StatusEffectComponent = GetOwner()->FindComponentByClass<UStatusEffectComponent>();
    if (GetWorld())
    {
        WeatherSystem = GetWorld()->GetGameInstance()->GetSubsystem<UWeatherSystem>();
    }
}

void UEnvironmentalFactorsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (WeatherSystem.IsValid())
    {
        FEnvironmentalFactors CurrentFactors;
        CurrentFactors.AirTemperature = WeatherSystem->GetCurrentTemperature();
        CurrentFactors.Weather = WeatherSystem->GetCurrentWeather();
        UpdateEnvironmentalFactors(CurrentFactors);
    }
}

void UEnvironmentalFactorsComponent::UpdateEnvironmentalFactors(const FEnvironmentalFactors& CurrentFactors)
{
    LastReceivedFactors = CurrentFactors;

    UpdateTemperatureEffects(LastReceivedFactors.AirTemperature);
    UpdateWeatherEffects(LastReceivedFactors.Weather);
    // Time of day effects are not yet implemented in the new system.
    // UpdateTimeOfDayEffects(LastReceivedFactors.TimeOfDay);
}

void UEnvironmentalFactorsComponent::UpdateTemperatureEffects(float AirTemperature)
{
    if (!StatusEffectComponent.IsValid())
    {
        return;
    }

    // Example logic: Apply "Cold" or "Hot" status effects based on temperature thresholds.
    if (AirTemperature < 10.0f) // Cold threshold
    {
        // Apply "Cold" status effect
        // FDataTableRowHandle ColdEffectHandle;
        // ColdEffectHandle.DataTable = ...;
        // ColdEffectHandle.RowName = "Cold";
        // StatusEffectComponent->ApplyEffect(ColdEffectHandle);
    }
    else if (AirTemperature > 30.0f) // Hot threshold
    {
        // Apply "Hot" status effect
        // FDataTableRowHandle HotEffectHandle;
        // HotEffectHandle.DataTable = ...;
        // HotEffectHandle.RowName = "Hot";
        // StatusEffectComponent->ApplyEffect(HotEffectHandle);
    }
}

void UEnvironmentalFactorsComponent::UpdateWeatherEffects(EWeatherType WeatherType)
{
    if (!StatusEffectComponent.IsValid())
    {
        return;
    }

    // Example logic: Apply "Wet" status effect during rain.
    if (WeatherType == EWeatherType::Rain)
    {
        // Apply "Wet" status effect
        // FDataTableRowHandle WetEffectHandle;
        // WetEffectHandle.DataTable = ...;
        // WetEffectHandle.RowName = "Wet";
        // StatusEffectComponent->ApplyEffect(WetEffectHandle);
    }
}

void UEnvironmentalFactorsComponent::UpdateTimeOfDayEffects(float TimeOfDay)
{
    // Example logic: Apply "Vulnerable" status effect at night.
    if (TimeOfDay > 18.0f || TimeOfDay < 6.0f) // Night time
    {
        // Apply "Vulnerable" status effect
    }
}
float UEnvironmentalFactorsComponent::GetCurrentTemperature() const
{
    return LastReceivedFactors.AirTemperature;
}