// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS/DACombatExecutionCalculation.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"  // For FGameplayTag

// Define static attributes using tags (setup in GameplayTags.ini)
FGameplayAttribute FDACombatStatics::BaseDamageAttribute()
{
    static FGameplayAttribute Attr(FGameplayAttribute::GetPropertyFromString("BaseDamage"));
    return Attr;
}

FGameplayAttribute FDACombatStatics::STRModifierAttribute()
{
    static FGameplayAttribute Attr(FGameplayAttribute::GetPropertyFromString("STRModifier"));
    return Attr;
}

FGameplayAttribute FDACombatStatics::ArmorDRAttribute()
{
    static FGameplayAttribute Attr(FGameplayAttribute::GetPropertyFromString("ArmorDR"));
    return Attr;
}

FGameplayAttribute FDACombatStatics::HealthAttribute()
{
    static FGameplayAttribute Attr(FGameplayAttribute::GetPropertyFromString("Health"));
    return Attr;
}

UDACombatExecutionCalculation::UDACombatExecutionCalculation()
{
    // Define captured attributes in constructor for GAS to know what to snapshot
    RelevantAttributesToCapture.Add(FDACombatStatics::BaseDamageAttribute());
    RelevantAttributesToCapture.Add(FDACombatStatics::STRModifierAttribute());
    RelevantAttributesToCapture.Add(FDACombatStatics::ArmorDRAttribute());
}

void UDACombatExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    if (!SourceASC || !TargetASC) return;

    FAggregatorEvaluateParameters EvalParams;

    float BaseDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDACombatStatics::BaseDamageAttribute(), EvalParams, BaseDamage);

    float STRModifier = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDACombatStatics::STRModifierAttribute(), EvalParams, STRModifier);

    float ArmorDR = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDACombatStatics::ArmorDRAttribute(), EvalParams, ArmorDR);

    // Your damage formula (adjust per weapon type via tags or params)
    float WeaponModifier = 0.5f;  // Example for swords; pass via SetByCaller
    float FinalDamage = (BaseDamage + (STRModifier * WeaponModifier)) - ArmorDR;

    if (FinalDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(FDACombatStatics::HealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
    }
}