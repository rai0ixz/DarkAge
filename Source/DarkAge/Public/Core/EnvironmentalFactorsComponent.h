#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Data/EnvironmentalFactorsData.h"
#include "Components/StatusEffectComponent.h"
#include "Core/WeatherSystem.h"
#include "EnvironmentalFactorsComponent.generated.h"

// Forward declarations
struct FEnvironmentalFactors;
class UStatusEffectComponent;
class UWeatherSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UEnvironmentalFactorsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnvironmentalFactorsComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Environmental Factors")
    void UpdateEnvironmentalFactors(const FEnvironmentalFactors& CurrentFactors);

    UFUNCTION(BlueprintPure, Category = "Environmental Factors")
    float GetCurrentTemperature() const;

private:
    void UpdateTemperatureEffects(float AirTemperature);
    void UpdateWeatherEffects(EWeatherType WeatherType);
    void UpdateTimeOfDayEffects(float TimeOfDay);

    UPROPERTY(VisibleAnywhere, Category = "Environmental Factors")
    FEnvironmentalFactors LastReceivedFactors;

    UPROPERTY()
    TWeakObjectPtr<UStatusEffectComponent> StatusEffectComponent;

    UPROPERTY()
    TWeakObjectPtr<UWeatherSystem> WeatherSystem;
};
