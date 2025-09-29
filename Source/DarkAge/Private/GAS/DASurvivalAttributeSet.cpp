// Copyright Epic Games, Inc. All Rights Reserved.

#include "Net/UnrealNetwork.h"  // For DOREPLIFETIME
#include "AbilitySystemComponent.h"  // For GAS types
#include "GameplayEffectTypes.h"  // For FGameplayEffectModCallbackData
#include "GameplayEffect.h"  // For FGameplayEffectModCallbackData
#include "GAS/DASurvivalAttributeSet.h"

UDASurvivalAttributeSet::UDASurvivalAttributeSet()
{
    // Set initial values (or load from DataTable in owning actor's BeginPlay)
    Hunger.SetBaseValue(100.f);
    Hunger.SetCurrentValue(100.f);
    Thirst.SetBaseValue(100.f);
    Thirst.SetCurrentValue(100.f);
    Warmth.SetBaseValue(100.f);
    Warmth.SetCurrentValue(100.f);
    Sleep.SetBaseValue(100.f);
    Sleep.SetCurrentValue(100.f);
    Strength.SetBaseValue(10.f);
    Strength.SetCurrentValue(10.f);
    Dexterity.SetBaseValue(10.f);
    Dexterity.SetCurrentValue(10.f);
    Health.SetBaseValue(100.f);
    Health.SetCurrentValue(100.f);
    Stamina.SetBaseValue(100.f);
    Stamina.SetCurrentValue(100.f);
    BaseDamage.SetBaseValue(10.f);
    BaseDamage.SetCurrentValue(10.f);
    STRModifier.SetBaseValue(0.f);
    STRModifier.SetCurrentValue(0.f);
    ArmorDR.SetBaseValue(0.f);
    ArmorDR.SetCurrentValue(0.f);
}

void UDASurvivalAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UDASurvivalAttributeSet, Hunger);
    DOREPLIFETIME(UDASurvivalAttributeSet, Thirst);
    DOREPLIFETIME(UDASurvivalAttributeSet, Warmth);
    DOREPLIFETIME(UDASurvivalAttributeSet, Sleep);
    DOREPLIFETIME(UDASurvivalAttributeSet, Strength);
    DOREPLIFETIME(UDASurvivalAttributeSet, Dexterity);
    DOREPLIFETIME(UDASurvivalAttributeSet, Health);
    DOREPLIFETIME(UDASurvivalAttributeSet, Stamina);
    DOREPLIFETIME(UDASurvivalAttributeSet, BaseDamage);
    DOREPLIFETIME(UDASurvivalAttributeSet, STRModifier);
    DOREPLIFETIME(UDASurvivalAttributeSet, ArmorDR);
}

void UDASurvivalAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // Clamp values (e.g., 0-100 for survival needs)
    if (Attribute == GetHungerAttribute() || Attribute == GetThirstAttribute() ||
        Attribute == GetWarmthAttribute() || Attribute == GetSleepAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
    }
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, Health.GetBaseValue());
    }
}

/*
void UDASurvivalAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Apply damage when thresholds hit
    if (Data.EvaluatedData.Attribute == GetHungerAttribute() && Hunger.GetCurrentValue() <= 0.f)
    {
        // Damage health (example rate from your needs table)
        SetHealth(FMath::Clamp(GetHealth() - 0.5f, 0.f, Health.GetBaseValue()));
    }
    if (Data.EvaluatedData.Attribute == GetThirstAttribute() && Thirst.GetCurrentValue() <= 0.f)
    {
        SetHealth(FMath::Clamp(GetHealth() - 1.f, 0.f, Health.GetBaseValue()));
    }
    // Add similar for Warmth (e.g., -1 HP/15s), Sleep (hallucinations via effects)
}
*/