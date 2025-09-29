// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS/DASurvivalDepletionAbility.h"
#include "AbilitySystemComponent.h"  // For UAbilitySystemComponent
#include "AbilitySystemBlueprintLibrary.h"  // For UAbilitySystemBlueprintLibrary

UDASurvivalDepletionAbility::UDASurvivalDepletionAbility()
{
    // Set as passive ability
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;  // For multiplayer prediction
}

void UDASurvivalDepletionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // Start ticking loop every second
    UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 1.f);
    WaitDelayTask->OnFinish.AddDynamic(this, &UDASurvivalDepletionAbility::DepleteStats);
    WaitDelayTask->ReadyForActivation();
}

void UDASurvivalDepletionAbility::DepleteStats()
{
    if (!GetAbilitySystemComponentFromActorInfo() || !IsValid(this)) return;

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    // Apply depletion effect with SetByCaller for rates
    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(DepletionEffectClass, GetAbilityLevel(), Context);

    // Set magnitudes
    Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.HungerDepletion")), HungerDepletionRate);
    Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.ThirstDepletion")), ThirstDepletionRate);
    // Add for others

    ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

    // Restart the loop (recursive tick)
    UAbilityTask_WaitDelay* NextTask = UAbilityTask_WaitDelay::WaitDelay(this, 1.f);
    NextTask->OnFinish.AddDynamic(this, &UDASurvivalDepletionAbility::DepleteStats);
    NextTask->ReadyForActivation();
}