#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NeedsCalculationSubsystem.generated.h"

// Forward Declarations
class UStatlineComponent;
class UStatusEffectComponent;
class UEnvironmentalFactorsComponent;
class UDiseaseManagementComponent;
class ACharacter;

USTRUCT()
struct FCharacterNeedsProfile
{
    GENERATED_BODY()

    UPROPERTY()
    TWeakObjectPtr<ACharacter> Character;

    UPROPERTY()
    TWeakObjectPtr<UStatlineComponent> Statline;

    UPROPERTY()
    TWeakObjectPtr<UStatusEffectComponent> StatusEffects;

    UPROPERTY()
    TWeakObjectPtr<UEnvironmentalFactorsComponent> EnvironmentalFactors;

    UPROPERTY()
    TWeakObjectPtr<UDiseaseManagementComponent> DiseaseManagement;

    bool IsValid() const;
};

UCLASS()
class DARKAGE_API UNeedsCalculationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    using Super = UGameInstanceSubsystem;
    UNeedsCalculationSubsystem();

    UFUNCTION(BlueprintCallable, Category = "Needs System")
    void RegisterCharacter(ACharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Needs System")
    void UnregisterCharacter(ACharacter* Character);

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void OnSystemTick(float DeltaTime);

private:
    void CalculateNeedsForCharacter(FCharacterNeedsProfile& Profile, float DeltaTime);

    UPROPERTY()
    TArray<FCharacterNeedsProfile> RegisteredCharacters;

    // Configuration for needs depletion
    UPROPERTY(EditDefaultsOnly, Category = "Needs Config")
    float BaseHungerRate; // per second

    UPROPERTY(EditDefaultsOnly, Category = "Needs Config")
    float BaseThirstRate; // per second

    // System info
    FString SystemName;
    float TickInterval;
};