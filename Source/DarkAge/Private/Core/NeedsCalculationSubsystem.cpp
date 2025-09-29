#include "Core/NeedsCalculationSubsystem.h"

// ...existing code...

void UNeedsCalculationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("NeedsCalculationSubsystem Initialized."));
}

void UNeedsCalculationSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("NeedsCalculationSubsystem Stopped."));
    Super::Deinitialize();
}

#include "Components/StatlineComponent.h"
#include "Components/StatusEffectComponent.h"
#include "Core/EnvironmentalFactorsComponent.h"
#include "Components/DiseaseManagementComponent.h"
#include "GameFramework/Character.h"

bool FCharacterNeedsProfile::IsValid() const
{
    return Character.IsValid() && Statline.IsValid() && StatusEffects.IsValid() && EnvironmentalFactors.IsValid() && DiseaseManagement.IsValid();
}

// Duplicate constructor removed

UNeedsCalculationSubsystem::UNeedsCalculationSubsystem() = default;

void UNeedsCalculationSubsystem::RegisterCharacter(ACharacter* Character)
{
    if (!Character)
    {
        return;
    }

    // Avoid duplicate entries
    if (RegisteredCharacters.ContainsByPredicate([Character](const FCharacterNeedsProfile& Profile){ return Profile.Character == Character; }))
    {
        return;
    }

    FCharacterNeedsProfile NewProfile;
    NewProfile.Character = Character;
    NewProfile.Statline = Character->FindComponentByClass<UStatlineComponent>();
    NewProfile.StatusEffects = Character->FindComponentByClass<UStatusEffectComponent>();
    NewProfile.EnvironmentalFactors = Character->FindComponentByClass<UEnvironmentalFactorsComponent>();
    NewProfile.DiseaseManagement = Character->FindComponentByClass<UDiseaseManagementComponent>();

    if (NewProfile.IsValid())
    {
        RegisteredCharacters.Add(NewProfile);
    }
}

void UNeedsCalculationSubsystem::UnregisterCharacter(ACharacter* Character)
{
    if (!Character)
    {
        return;
    }

    RegisteredCharacters.RemoveAll([Character](const FCharacterNeedsProfile& Profile){ return Profile.Character == Character; });
}

void UNeedsCalculationSubsystem::OnSystemTick(float DeltaTime)
{
    // Super::OnSystemTick(DeltaTime); // Removed: not present in base class

    for (FCharacterNeedsProfile& Profile : RegisteredCharacters)
    {
        if (Profile.IsValid())
        {
            CalculateNeedsForCharacter(Profile, DeltaTime);
        }
    }
}

void UNeedsCalculationSubsystem::CalculateNeedsForCharacter(FCharacterNeedsProfile& Profile, float DeltaTime)
{
    // This is a simplified calculation. A real implementation would be more complex.
    float HungerDelta = BaseHungerRate * DeltaTime;
    float ThirstDelta = BaseThirstRate * DeltaTime;

    // Modify deltas based on status effects
    if (Profile.StatusEffects->HasEffect(FName("WellFed")))
    {
        HungerDelta *= 0.5f; // 50% slower hunger
    }
    if (Profile.StatusEffects->HasEffect(FName("Starving")))
    {
        HungerDelta *= 2.0f; // 2x faster hunger
    }

    // Modify deltas based on environmental factors
    if (Profile.EnvironmentalFactors->GetCurrentTemperature() > 30.0f) // Hot
    {
        ThirstDelta *= 1.5f; // 50% faster thirst
    }
    else if (Profile.EnvironmentalFactors->GetCurrentTemperature() < 5.0f) // Cold
    {
        HungerDelta *= 1.2f; // 20% faster hunger
    }

    // Modify deltas based on diseases
    if (Profile.DiseaseManagement->IsInfectedWith(FName("StomachFlu")))
    {
        HungerDelta *= 1.5f;
        ThirstDelta *= 1.5f;
    }

    Profile.Statline->UpdateStat(FName("Hunger"), HungerDelta);
    Profile.Statline->UpdateStat(FName("Thirst"), ThirstDelta);
}