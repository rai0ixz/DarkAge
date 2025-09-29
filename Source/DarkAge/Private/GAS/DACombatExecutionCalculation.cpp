// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS/DACombatExecutionCalculation.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"  // For FGameplayTag
#include "GAS/DASurvivalAttributeSet.h"  // For attribute accessors

// Define static attributes using attribute set accessors
FGameplayAttribute FDACombatStatics::BaseDamageAttribute()
{
    return UDASurvivalAttributeSet::GetBaseDamageAttribute();
}

FGameplayAttribute FDACombatStatics::STRModifierAttribute()
{
    return UDASurvivalAttributeSet::GetSTRModifierAttribute();
}

FGameplayAttribute FDACombatStatics::ArmorDRAttribute()
{
    return UDASurvivalAttributeSet::GetArmorDRAttribute();
}

FGameplayAttribute FDACombatStatics::HealthAttribute()
{
    return UDASurvivalAttributeSet::GetHealthAttribute();
}

UDACombatExecutionCalculation::UDACombatExecutionCalculation()
{
    // Define captured attributes in constructor for GAS to know what to snapshot
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(FDACombatStatics::BaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(FDACombatStatics::STRModifierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(FDACombatStatics::ArmorDRAttribute(), EGameplayEffectAttributeCaptureSource::Target, true));
}

void UDACombatExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    if (!SourceASC || !TargetASC) return;

    FAggregatorEvaluateParameters EvalParams;

    float BaseDamage = 0.f;
    FGameplayEffectAttributeCaptureDefinition BaseDamageCapture(FDACombatStatics::BaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BaseDamageCapture, EvalParams, BaseDamage);

    float STRModifier = 0.f;
    FGameplayEffectAttributeCaptureDefinition STRModifierCapture(FDACombatStatics::STRModifierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(STRModifierCapture, EvalParams, STRModifier);

    float ArmorDR = 0.f;
    FGameplayEffectAttributeCaptureDefinition ArmorDRCapture(FDACombatStatics::ArmorDRAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ArmorDRCapture, EvalParams, ArmorDR);

    // Your damage formula (adjust per weapon type via tags or params)
    float WeaponModifier = 0.5f;  // Example for swords; pass via SetByCaller
    float FinalDamage = (BaseDamage + (STRModifier * WeaponModifier)) - ArmorDR;

    if (FinalDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(FDACombatStatics::HealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
    }
}