#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/FactionReputationComponent.h"
#include "AIMoraleComponent.generated.h"

UENUM(BlueprintType)
enum class EAIMoraleState : uint8
{
	Heroic,
	High,
	Average,
	Low,
	Broken,
	Fleeing,
	Surrendered
};

UENUM(BlueprintType)
enum class EEmotionalState : uint8
{
	   Neutral,
	   Hopeful,
	   Fearful,
	   Angry,
	   Despairing
};

USTRUCT(BlueprintType)
struct FPersonalityTraits
{
	   GENERATED_BODY()

	   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
	   float Bravery = 0.5f; // 0.0 = Cowardly, 1.0 = Brave

	   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
	   float Optimism = 0.5f; // 0.0 = Pessimistic, 1.0 = Optimistic

	   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
	   float Aggressiveness = 0.5f; // 0.0 = Passive, 1.0 = Aggressive
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoraleStateChanged, EAIMoraleState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurrender, AActor*, SurrenderingActor);

USTRUCT(BlueprintType)
struct FMoraleModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	FString Reason;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float BaseChange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float DamageTaken = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float AllyWitnessedDeath = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float LeadershipAura = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float EnvironmentalHardship = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float IdeologicalBelief = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	float SupplyStatus = 0.f;

	   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale Modifier")
	   EEmotionalState EmotionalImpact = EEmotionalState::Neutral;

	FMoraleModifier() {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UAIMoraleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIMoraleComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "AI Morale")
	FOnMoraleStateChanged OnMoraleStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "AI Morale")
	FOnSurrender OnSurrender;

	UFUNCTION(BlueprintCallable, Category = "AI Morale")
	void ApplyMoraleChange(const FMoraleModifier& Modifier);

	UFUNCTION(BlueprintPure, Category = "AI Morale")
	EAIMoraleState GetMoraleState() const;

	UFUNCTION(BlueprintPure, Category = "AI Morale")
	float GetCurrentMorale() const;

	UFUNCTION(BlueprintCallable, Category = "AI Morale")
	void UpdateLoyalty(float Amount, FString Reason);

	UFUNCTION(BlueprintCallable, Category = "AI Morale")
	void UpdateFear(float Amount, FString Reason);

	   UFUNCTION(BlueprintPure, Category = "AI Morale")
	   EEmotionalState GetEmotionalState() const;
		
		// Switches behavior based on morale state
		UFUNCTION(BlueprintCallable, Category = "AI Morale")
		void SwitchBehaviorByMorale();

protected:
	virtual void BeginPlay() override;

	   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	   FPersonalityTraits Personality;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	float Loyalty = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	float Fear = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	float MaxLoyalty = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	float MaxFear = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Configuration")
	float SurrenderThreshold = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Modifiers")
	float DamageTakenMultiplier = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Modifiers")
	float AllyDeathMultiplier = -15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Thresholds")
	float HeroicThreshold = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Thresholds")
	float HighThreshold = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Thresholds")
	float AverageThreshold = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale|Thresholds")
	float LowThreshold = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Morale")
	float MaxMorale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Morale")
	float CurrentMorale;

	   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Morale")
	   EEmotionalState EmotionalState;

private:
	void SetMoraleState(EAIMoraleState NewState);
	   void SetEmotionalState(EEmotionalState NewState);
	void UpdateMoraleState();
	void CheckSurrender();
	   void UpdateGroupMorale();

	UPROPERTY()
	UFactionReputationComponent* FactionReputationComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI Morale")
	EAIMoraleState MoraleState;
};