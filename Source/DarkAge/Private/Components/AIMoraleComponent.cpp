#include "Components/AIMoraleComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// Switches behavior based on morale state
void UAIMoraleComponent::SwitchBehaviorByMorale()
{
	switch (MoraleState)
	{
	case EAIMoraleState::Heroic:
		// Example: Increase aggression, seek out enemies
		UE_LOG(LogTemp, Log, TEXT("%s is feeling heroic!"), *GetOwner()->GetName());
		break;
	case EAIMoraleState::High:
		// Example: Normal combat behavior
		UE_LOG(LogTemp, Log, TEXT("%s morale is high."), *GetOwner()->GetName());
		break;
	case EAIMoraleState::Average:
		// Example: Defensive or cautious
		UE_LOG(LogTemp, Log, TEXT("%s morale is average."), *GetOwner()->GetName());
		break;
	case EAIMoraleState::Low:
		// Example: Consider fleeing
		UE_LOG(LogTemp, Log, TEXT("%s morale is low! Considering retreat."), *GetOwner()->GetName());
		break;
	case EAIMoraleState::Broken:
		// Example: Flee or surrender
		UE_LOG(LogTemp, Log, TEXT("%s morale is broken! Fleeing or surrendering."), *GetOwner()->GetName());
		break;
	case EAIMoraleState::Fleeing:
		// Example: Already fleeing
		break;
	case EAIMoraleState::Surrendered:
		// Example: Already surrendered
		break;
	default:
		break;
	}
}
#include "Components/FactionReputationComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"

UAIMoraleComponent::UAIMoraleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	MaxMorale = 100.f;
	CurrentMorale = MaxMorale;
	MoraleState = EAIMoraleState::Average;
	Loyalty = 50.f;
	Fear = 0.f;
}

void UAIMoraleComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentMorale = MaxMorale;
	UpdateMoraleState();

	FactionReputationComponent = GetOwner()->FindComponentByClass<UFactionReputationComponent>();
}

void UAIMoraleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Example: Fear decays over time when not in a threatening situation
	if (Fear > 0)
	{
		UpdateFear(-0.1f * DeltaTime, "Decay");
	}

	   UpdateGroupMorale();
}

void UAIMoraleComponent::ApplyMoraleChange(const FMoraleModifier& Modifier)
{
	float TotalChange = Modifier.BaseChange;
	TotalChange += Modifier.DamageTaken * DamageTakenMultiplier * (1.0f - Personality.Bravery);
	TotalChange += Modifier.AllyWitnessedDeath * AllyDeathMultiplier * (1.0f - Personality.Bravery);
	TotalChange += Modifier.LeadershipAura;
	TotalChange += Modifier.EnvironmentalHardship * (1.0f - Personality.Optimism);
	TotalChange += Modifier.IdeologicalBelief;
	TotalChange += Modifier.SupplyStatus;

	// Factor in psychological state
	float LoyaltyBonus = (Loyalty / MaxLoyalty) * 10.f; // Example: max 10 bonus morale
	float FearPenalty = (Fear / MaxFear) * 20.f;       // Example: max 20 morale penalty
	TotalChange += LoyaltyBonus - FearPenalty;

	   SetEmotionalState(Modifier.EmotionalImpact);

	if (FMath::IsNearlyZero(TotalChange))
	{
		return;
	}

	CurrentMorale = FMath::Clamp(CurrentMorale + TotalChange, 0.f, MaxMorale);

	FString ActorName = GetOwner()->GetName();
	UE_LOG(LogTemp, Log, TEXT("Morale Change on %s: %f. Reason: %s. New Morale: %f"), *ActorName, TotalChange, *Modifier.Reason, CurrentMorale);

	UpdateMoraleState();
	CheckSurrender();
}

EAIMoraleState UAIMoraleComponent::GetMoraleState() const
{
	return MoraleState;
}
float UAIMoraleComponent::GetCurrentMorale() const
{
	return CurrentMorale;
}

EEmotionalState UAIMoraleComponent::GetEmotionalState() const
{
	   return EmotionalState;
}

void UAIMoraleComponent::UpdateLoyalty(float Amount, FString Reason)
{
	Loyalty = FMath::Clamp(Loyalty + Amount, 0.f, MaxLoyalty);
	UE_LOG(LogTemp, Log, TEXT("Loyalty changed by %f on %s. Reason: %s. New Loyalty: %f"), Amount, *GetOwner()->GetName(), *Reason, Loyalty);
}

void UAIMoraleComponent::UpdateFear(float Amount, FString Reason)
{
	Fear = FMath::Clamp(Fear + Amount, 0.f, MaxFear);
	UE_LOG(LogTemp, Log, TEXT("Fear changed by %f on %s. Reason: %s. New Fear: %f"), Amount, *GetOwner()->GetName(), *Reason, Fear);
}

void UAIMoraleComponent::SetMoraleState(EAIMoraleState NewState)
{
	if (MoraleState != NewState)
	{
		MoraleState = NewState;
		FString ActorName = GetOwner()->GetName();
		FString EnumDisplayName = UEnum::GetValueAsString(NewState);
		UE_LOG(LogTemp, Warning, TEXT("%s new morale state is: %s"), *ActorName, *EnumDisplayName);

		OnMoraleStateChanged.Broadcast(NewState);
	}
}

void UAIMoraleComponent::UpdateMoraleState()
{
	EAIMoraleState NewState;
	if (CurrentMorale >= HeroicThreshold)
	{
		NewState = EAIMoraleState::Heroic;
	}
	else if (CurrentMorale >= HighThreshold)
	{
		NewState = EAIMoraleState::High;
	}
	else if (CurrentMorale >= AverageThreshold)
	{
		NewState = EAIMoraleState::Average;
	}
	else if (CurrentMorale >= LowThreshold)
	{
		NewState = EAIMoraleState::Low;
	}
	else
	{
		NewState = EAIMoraleState::Broken;
	}

	if (MoraleState != EAIMoraleState::Fleeing && MoraleState != EAIMoraleState::Surrendered)
	{
		SetMoraleState(NewState);
	}
}

void UAIMoraleComponent::CheckSurrender()
{
	if (MoraleState == EAIMoraleState::Broken || CurrentMorale <= SurrenderThreshold)
	{
		// More complex logic can be added here based on GDD (e.g., check odds)
		SetMoraleState(EAIMoraleState::Surrendered);
		OnSurrender.Broadcast(GetOwner());
	}
}

void UAIMoraleComponent::SetEmotionalState(EEmotionalState NewState)
{
	if (EmotionalState != NewState)
	{
		EmotionalState = NewState;
	}
}

void UAIMoraleComponent::UpdateGroupMorale()
{
	if (UWorld* World = GetWorld())
	{
		TArray<FOverlapResult> OverlappingActors;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(500.0f); // 5m radius
		World->OverlapMultiByChannel(OverlappingActors, GetOwner()->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere);
	
		float GroupMorale = 0.0f;
		int32 GroupSize = 0;

	for (const FOverlapResult& Overlap : OverlappingActors)
	{
		AActor* Actor = Overlap.GetActor();
		if (Actor && Actor != GetOwner())
		{
			UAIMoraleComponent* OtherMorale = Actor->FindComponentByClass<UAIMoraleComponent>();
			if (OtherMorale)
			{
				GroupMorale += OtherMorale->GetCurrentMorale();
				GroupSize++;
			}
		}
	}

		if (GroupSize > 0)
		{
			float AverageGroupMorale = GroupMorale / GroupSize;
			CurrentMorale = FMath::Lerp(CurrentMorale, AverageGroupMorale, 0.1f);
			UpdateMoraleState();
		}
	}
}