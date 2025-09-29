// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectTypes.h"  // For FGameplayModifierEvaluatedData
#include "DACombatExecutionCalculation.generated.h"

// Helper struct for attribute definitions (use GameplayTags)
struct FDACombatStatics
{
    static FGameplayAttribute BaseDamageAttribute();
    static FGameplayAttribute STRModifierAttribute();
    static FGameplayAttribute ArmorDRAttribute();
    static FGameplayAttribute HealthAttribute();

    // Define in cpp
};

UCLASS()
class DARKAGE_API UDACombatExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UDACombatExecutionCalculation();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};