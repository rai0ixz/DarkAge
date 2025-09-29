// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"  // For TSubclassOf<UGameplayEffect>
#include "AbilityTasks/AbilityTask_WaitDelay.h"  // For UAbilityTask_WaitDelay
#include "DASurvivalDepletionAbility.generated.h"

UCLASS()
class DARKAGE_API UDASurvivalDepletionAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UDASurvivalDepletionAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, Category = "Depletion")
    float HungerDepletionRate = 0.1f;  // Per second

    UPROPERTY(EditDefaultsOnly, Category = "Depletion")
    float ThirstDepletionRate = 0.2f;

    // Add rates for Warmth, Sleep (variable)

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    TSubclassOf<UGameplayEffect> DepletionEffectClass;  // Assign in BP: Effect with SetByCaller modifiers

protected:
    UFUNCTION()
    void DepleteStats();
};