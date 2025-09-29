#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AINeedsPlanningComponent.generated.h"

// Forward declarations
class UDA_NPCManagerSubsystem;

UENUM(BlueprintType)
enum class EAINeed : uint8
{
	Hunger,
	Thirst,
	Rest,
	Safety,
	Social
};

UENUM(BlueprintType)
enum class EAIGoal : uint8
{
	None,
	FindFood,
	FindWater,
	FindShelter,
	FulfillSocial,
	IncreaseSafety,
	Work,
	Trade
};

USTRUCT(BlueprintType)
struct FNeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Needs")
	float Value = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Needs")
	float DecayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Needs")
	float UrgencyThreshold = 30.f;
};

USTRUCT(BlueprintType, meta=(ScriptName = "AIGoalInfo"))
struct FAIGoal
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Goal")
	EAIGoal Type = EAIGoal::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Goal")
	float Priority = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Goal")
	AActor* TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Goal")
	FVector TargetLocation = FVector::ZeroVector;

	// Overload the << operator for easy logging
    friend FArchive& operator<<(FArchive& Ar, FAIGoal& Goal)
    {
        Ar << Goal.Type;
        Ar << Goal.Priority;
        Ar << Goal.TargetActor;
        Ar << Goal.TargetLocation;
        return Ar;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UAINeedsPlanningComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAINeedsPlanningComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** A replacement for TickComponent, called exclusively by the DA_NPCManagerSubsystem. */
	   void ManagedTick(float DeltaTime);

	// Executes the current goal (stub for integration with world/AI)
	UFUNCTION(BlueprintCallable, Category = "AI Needs")
	void ExecuteCurrentGoal();

	// Event hook for when a need becomes urgent
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNeedUrgent, EAINeed, UrgentNeed);
	UPROPERTY(BlueprintAssignable, Category = "AI Needs")
	FOnNeedUrgent OnNeedUrgent;

	UFUNCTION(BlueprintCallable, Category = "AI Needs")
	void SatisfyNeed(EAINeed Need, float Amount);

	UFUNCTION(BlueprintPure, Category = "AI Needs")
	float GetNeedValue(EAINeed Need) const;

	UFUNCTION(BlueprintPure, Category = "AI Needs")
	bool IsNeedUrgent(EAINeed Need) const;

	UFUNCTION(BlueprintPure, Category = "AI Needs")
	FAIGoal GetHighestPriorityGoal() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void InitializeNeeds();
	EAIGoal DetermineGoalFromNeed(EAINeed Need) const;

	// Advanced AI behavior methods
	void UpdateNeedsWithContext(float DeltaTime);
	float GetTimeOfDay() const;
	float EvaluateDangerLevel() const;
	float GetSocialIsolationMultiplier() const;
	float GetContextualPriorityModifier(EAINeed NeedType) const;
	void UpdateGoalProgress(const FAIGoal& Goal);

	// Behavior execution methods
	void ExecuteFoodSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger);
	void ExecuteWaterSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger);
	void ExecuteShelterSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger);
	void ExecuteSafetyBehavior(const FVector& Location, float TimeOfDay);
	void ExecuteSocialBehavior(const FVector& Location, float TimeOfDay, bool bInDanger);
	void ExecuteWorkBehavior(const FVector& Location, float TimeOfDay);
	void ExecuteTradeBehavior(const FVector& Location, float TimeOfDay);
	void ExecuteIdleBehavior(const FVector& Location, float TimeOfDay);

	// Specific action methods
	void AttemptForaging();
	void AttemptWaterCollection();
	void AttemptShelterSeeking(bool bHighPriority);
	void AttemptFlee();
	void AttemptDefensivePosition();
	void AttemptPatrol();
	void AttemptSeekHelp();
	void AttemptTavernSocialization();
	void AttemptCasualSocialization();
	void AttemptWorkSocialization();
	void AttemptWork();
	void AttemptMaintenance();
	void AttemptTrading();
	void AttemptTradePreparation();

	UPROPERTY(EditAnywhere, Category = "AI Needs")
	TMap<EAINeed, FNeed> Needs;

	// Additional member variables
	UPROPERTY(VisibleAnywhere, Category = "AI State")
	float CurrentGoalProgress;

	UPROPERTY(VisibleAnywhere, Category = "AI State")
	float GoalExecutionTimer;
};