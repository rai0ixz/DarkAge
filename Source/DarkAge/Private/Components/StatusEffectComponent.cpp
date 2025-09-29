#include "Components/StatusEffectComponent.h"
#include "Components/StatlineComponent.h"
#include "Engine/World.h"

UStatusEffectComponent::UStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStatusEffectComponent::BeginPlay()
{
    Super::BeginPlay();
    StatlineComponent = GetOwner()->FindComponentByClass<UStatlineComponent>();
}

void UStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
    {
        FActiveStatusEffect& Effect = ActiveEffects[i];
        if (Effect.bIsActive)
        {
            HandleEffectTick(Effect, DeltaTime);
        }
        else
        {
            ActiveEffects.RemoveAt(i);
        }
    }
}

void UStatusEffectComponent::ApplyEffect(const FDataTableRowHandle& EffectDataRowHandle)
{
    if (!EffectDataRowHandle.DataTable || EffectDataRowHandle.RowName.IsNone())
    {
        return;
    }

    const FStatusEffectData* EffectData = EffectDataRowHandle.GetRow<FStatusEffectData>("");
    if (!EffectData)
    {
        return;
    }

    FActiveStatusEffect NewEffect;
    NewEffect.EffectDataRowHandle = EffectDataRowHandle;
    NewEffect.EffectID = EffectData->EffectID;
    NewEffect.RemainingDuration = EffectData->BaseDuration;
    if (UWorld* World = GetWorld())
    {
        NewEffect.ApplicationTime = World->GetTimeSeconds();
        NewEffect.LastTickTime = World->GetTimeSeconds();
    }

    ActiveEffects.Add(NewEffect);
    ApplyStatModifiers(EffectData, true);
    OnStatusEffectAdded.Broadcast(NewEffect.EffectID, true);
}

void UStatusEffectComponent::RemoveEffect(FName EffectID)
{
    for (int32 i = 0; i < ActiveEffects.Num(); ++i)
    {
        if (ActiveEffects[i].EffectID == EffectID)
        {
            const FStatusEffectData* EffectData = ActiveEffects[i].EffectDataRowHandle.GetRow<FStatusEffectData>("");
            ApplyStatModifiers(EffectData, false);
            OnStatusEffectRemoved.Broadcast(EffectID, false);
            ActiveEffects.RemoveAt(i);
            return;
        }
    }
}

bool UStatusEffectComponent::HasEffect(FName EffectID) const
{
    return ActiveEffects.ContainsByPredicate([EffectID](const FActiveStatusEffect& Effect)
    {
        return Effect.EffectID == EffectID;
    });
}

void UStatusEffectComponent::HandleEffectTick(FActiveStatusEffect& Effect, float DeltaTime)
{
    Effect.RemainingDuration -= DeltaTime;

    if (Effect.RemainingDuration <= 0.0f)
    {
        Effect.bIsActive = false;
        return;
    }

    const FStatusEffectData* EffectData = Effect.EffectDataRowHandle.GetRow<FStatusEffectData>("");
    if (EffectData && EffectData->ApplicationMethod == EStatusEffectApplication::OverTime)
    {
        if (UWorld* World = GetWorld())
        {
            if (World->GetTimeSeconds() - Effect.LastTickTime >= EffectData->TickInterval)
            {
                Effect.LastTickTime = World->GetTimeSeconds();
                OnStatusEffectTicked.Broadcast(Effect.EffectID, Effect.CurrentStacks, Effect.RemainingDuration);
            }
        }
    }
}

void UStatusEffectComponent::ApplyStatModifiers(const FStatusEffectData* EffectData, bool bIsApplying)
{
    if (!StatlineComponent.IsValid() || !EffectData)
    {
        return;
    }

    for (const FStatusEffectStatModifier& Modifier : EffectData->StatModifiers)
    {
        float Value = bIsApplying ? Modifier.ModifierValue : -Modifier.ModifierValue;
        // The new UpdateStat function only takes the stat name and the delta.
        // The logic for percentage and additive modifiers is now handled within the StatlineComponent.
        // This function will need to be updated to use the new status effect system.
        // For now, I will just pass the value to the new UpdateStat function.
        StatlineComponent->UpdateStat(Modifier.StatName, Value);
    }
}