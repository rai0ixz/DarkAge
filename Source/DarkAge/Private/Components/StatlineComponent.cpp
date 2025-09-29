// StatlineComponent.cpp
// Clean recreation with hunger/thirst support

#include "Components/StatlineComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

UStatlineComponent::UStatlineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UStatlineComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize default stats if none are set
	if (Stats.Num() == 0)
	{
		// Add default stats
		Stats.Add(FStatEntry{FName("Health"), FStat(100.0f, 100.0f, EStatCategory::Primary)});
		Stats.Add(FStatEntry{FName("Stamina"), FStat(100.0f, 100.0f, EStatCategory::Primary)});
		Stats.Add(FStatEntry{FName("Mana"), FStat(100.0f, 100.0f, EStatCategory::Primary)});
		Stats.Add(FStatEntry{FName("Hunger"), FStat(100.0f, 100.0f, EStatCategory::Survival)});
		Stats.Add(FStatEntry{FName("Thirst"), FStat(100.0f, 100.0f, EStatCategory::Survival)});
	}
}

void UStatlineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ProcessStatusEffects(DeltaTime);
}

void UStatlineComponent::UpdateStat(FName StatName, float Delta)
{
	if (GetOwner()->HasAuthority())
	{
		for (FStatEntry& StatEntry : Stats)
		{
			if (StatEntry.StatName == StatName)
			{
				float OldValue = StatEntry.Stat.CurrentValue;
				StatEntry.Stat.CurrentValue = FMath::Clamp(StatEntry.Stat.CurrentValue + Delta, 0.0f, StatEntry.Stat.BaseValue);

				if (OldValue != StatEntry.Stat.CurrentValue)
				{
					OnStatChanged.Broadcast(StatName, StatEntry.Stat.CurrentValue);

					// Check for death
					if (StatName == FName("Health") && StatEntry.Stat.CurrentValue <= 0.0f)
					{
						OnDeath.Broadcast();
					}
				}
				break;
			}
		}
	}
	else
	{
		ServerUpdateStat(StatName, Delta);
	}
}

float UStatlineComponent::GetCurrentStatValue(FName StatName) const
{
	for (const FStatEntry& StatEntry : Stats)
	{
		if (StatEntry.StatName == StatName)
		{
			return StatEntry.Stat.CurrentValue;
		}
	}
	return 0.0f;
}

float UStatlineComponent::GetBaseStatValue(FName StatName) const
{
	for (const FStatEntry& StatEntry : Stats)
	{
		if (StatEntry.StatName == StatName)
		{
			return StatEntry.Stat.BaseValue;
		}
	}
	return 0.0f;
}

float UStatlineComponent::GetMaxStatValue(FName StatName) const
{
	return GetBaseStatValue(StatName);
}

float UStatlineComponent::GetStatPercentage(FName StatName) const
{
	float Current = GetCurrentStatValue(StatName);
	float Max = GetMaxStatValue(StatName);
	return Max > 0.0f ? (Current / Max) : 0.0f;
}

void UStatlineComponent::ApplyStatusEffect(const FDataTableRowHandle& EffectDataRowHandle, UObject* Instigator)
{
	// Implementation would depend on your status effect system
	// For now, just log that it was called
	UE_LOG(LogTemp, Log, TEXT("ApplyStatusEffect called on %s"), *GetOwner()->GetName());
}

void UStatlineComponent::RemoveStatusEffect(FName EffectID)
{
	// Implementation would depend on your status effect system
	UE_LOG(LogTemp, Log, TEXT("RemoveStatusEffect called for %s on %s"), *EffectID.ToString(), *GetOwner()->GetName());
}

void UStatlineComponent::RecalculateStat(FName StatName)
{
	// Recalculate stat based on base value and modifiers
	for (FStatEntry& StatEntry : Stats)
	{
		if (StatEntry.StatName == StatName)
		{
			// For now, just ensure current value doesn't exceed base value
			StatEntry.Stat.CurrentValue = FMath::Min(StatEntry.Stat.CurrentValue, StatEntry.Stat.BaseValue);
			OnStatChanged.Broadcast(StatName, StatEntry.Stat.CurrentValue);
			break;
		}
	}
}

void UStatlineComponent::ProcessStatusEffects(float DeltaTime)
{
	// Process active status effects
	for (int32 i = ActiveStatusEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& Effect = ActiveStatusEffects[i];
		Effect.RemainingDuration -= DeltaTime;

		if (Effect.RemainingDuration <= 0.0f)
		{
			OnStatusEffectExpired.Broadcast(Effect.EffectID);
			ActiveStatusEffects.RemoveAt(i);
		}
		else
		{
			OnStatusEffectTick.Broadcast(Effect.EffectID, Effect.CurrentStacks, Effect.RemainingDuration);
		}
	}
}

void UStatlineComponent::OnRep_Stats()
{
	// Handle replication of stats
	for (const FStatEntry& StatEntry : Stats)
	{
		OnStatChanged.Broadcast(StatEntry.StatName, StatEntry.Stat.CurrentValue);
	}
}

void UStatlineComponent::ServerUpdateStat_Implementation(FName StatName, float Delta)
{
	UpdateStat(StatName, Delta);
}

void UStatlineComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStatlineComponent, Stats);
}

void UStatlineComponent::SetStats(const TArray<FStatEntry>& InStats)
{
	Stats = InStats;
	if (GetOwner()->HasAuthority())
	{
		for (const FStatEntry& StatEntry : Stats)
		{
			OnStatChanged.Broadcast(StatEntry.StatName, StatEntry.Stat.CurrentValue);
		}
	}
}

void UStatlineComponent::PrintStats()
{
	UE_LOG(LogTemp, Log, TEXT("=== Stats for %s ==="), *GetOwner()->GetName());
	for (const FStatEntry& StatEntry : Stats)
	{
		UE_LOG(LogTemp, Log, TEXT("%s: %.2f/%.2f"),
			*StatEntry.StatName.ToString(),
			StatEntry.Stat.CurrentValue,
			StatEntry.Stat.BaseValue);
	}
}

// Convenience methods implementation
float UStatlineComponent::GetCurrentStamina() const
{
	return GetCurrentStatValue(FName("Stamina"));
}

void UStatlineComponent::ModifyStamina(float Delta)
{
	UpdateStat(FName("Stamina"), Delta);
}

float UStatlineComponent::GetCurrentHealth() const
{
	return GetCurrentStatValue(FName("Health"));
}

void UStatlineComponent::ModifyHealth(float Delta)
{
	UpdateStat(FName("Health"), Delta);
}

float UStatlineComponent::GetCurrentMana() const
{
	return GetCurrentStatValue(FName("Mana"));
}

void UStatlineComponent::ModifyMana(float Delta)
{
	UpdateStat(FName("Mana"), Delta);
}

float UStatlineComponent::GetCurrentHunger() const
{
	return GetCurrentStatValue(FName("Hunger"));
}

void UStatlineComponent::ModifyHunger(float Delta)
{
	UpdateStat(FName("Hunger"), Delta);
}

float UStatlineComponent::GetCurrentThirst() const
{
	return GetCurrentStatValue(FName("Thirst"));
}

void UStatlineComponent::ModifyThirst(float Delta)
{
	UpdateStat(FName("Thirst"), Delta);
}
