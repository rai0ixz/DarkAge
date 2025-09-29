#include "Components/ClimateAdaptationComponent.h"
#include "Components/StatlineComponent.h"
#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
// RepNotify for AdaptationLevels
void UClimateAdaptationComponent::OnRep_Adaptation()
{
    // Optionally broadcast an event or update UI
}

// RepNotify for CurrentClimate
void UClimateAdaptationComponent::OnRep_CurrentClimate()
{
    // Optionally broadcast an event or update UI
}

// Server RPC for updating adaptation
void UClimateAdaptationComponent::ServerUpdateAdaptation_Implementation(float DeltaTime)
{
    UpdateAdaptation(DeltaTime);
}

// Server RPC for applying climate effects
void UClimateAdaptationComponent::ServerApplyClimateEffects_Implementation()
{
    ApplyClimateEffects();
}

void UClimateAdaptationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UClimateAdaptationComponent, AdaptationLevels);
    DOREPLIFETIME(UClimateAdaptationComponent, CurrentClimate);
}

UClimateAdaptationComponent::UClimateAdaptationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UClimateAdaptationComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UClimateAdaptationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateAdaptation(DeltaTime);
}


void UClimateAdaptationComponent::UpdateAdaptation(float DeltaTime)
{
    // Example: Gradually increase adaptation to current climate
    FClimateAdaptationEntry* Entry = AdaptationLevels.FindByPredicate([&](const FClimateAdaptationEntry& InEntry) {
        return InEntry.Climate == CurrentClimate;
    });

    if (Entry)
    {
        Entry->Level = FMath::Clamp(Entry->Level + DeltaTime * 0.1f, 0.0f, 1.0f);
    }
    else
    {
        AdaptationLevels.Add({CurrentClimate, 0.1f});
    }
}


void UClimateAdaptationComponent::ApplyClimateEffects()
{
    // Example: If adaptation is low, apply penalties to Statline (e.g., stamina, health)
    float AdaptLevel = GetAdaptationLevel(CurrentClimate);
    if (AdaptLevel < 0.3f)
    {
        UStatlineComponent* Statline = GetOwner()->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            Statline->UpdateStat(FName(TEXT("Stamina")), -5.0f);
        }
    }
    else if (AdaptLevel > 0.7f)
    {
        UStatlineComponent* Statline = GetOwner()->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            Statline->UpdateStat(FName(TEXT("Stamina")), 2.0f);
        }
    }
}

float UClimateAdaptationComponent::GetAdaptationLevel(EClimateType Climate) const
{
    const FClimateAdaptationEntry* Entry = AdaptationLevels.FindByPredicate([&](const FClimateAdaptationEntry& InEntry) {
        return InEntry.Climate == Climate;
    });

    return Entry ? Entry->Level : 0.0f;
}