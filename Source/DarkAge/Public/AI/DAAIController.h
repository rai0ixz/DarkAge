#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/AIMoraleComponent.h"
#include "Components/AIMemoryComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "DAAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class UAIMemoryComponent;
class UAINeedsPlanningComponent;

enum class ECrimeType : uint8;

UCLASS()
class DARKAGE_API ADAAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Perception system

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	ADAAIController();

	static const FName BlackboardKey_AIState;
	static const FName BlackboardKey_CurrentTactic;
	static const FName BlackboardKey_SocialTarget;
	static const FName BlackboardKey_NeedsUrgency;
	static const FName BlackboardKey_AdaptationLevel;
	static const FName BlackboardKey_MoraleState;
	static const FName BlackboardKey_PlayerTrust;

	UFUNCTION(BlueprintCallable, Category = "AI|Crime")
	void WitnessAndReportCrime(AActor* Perpetrator, ECrimeType CrimeCommitted, float BaseNotorietyValue, const FVector& CrimeLocation);

	UFUNCTION(BlueprintCallable, Category = "AI|Advanced")
	void InitializeAdvancedAI();

	UFUNCTION(BlueprintPure, Category = "AI|Memory")
	bool ShouldAdaptBehavior() const;

	UFUNCTION(BlueprintPure, Category = "AI|Needs")
	bool HasUrgentNeeds() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Social")
	void InitiateSocialInteraction(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Learning")
	void RecordBehaviorOutcome(const FString& BehaviorName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void UpdateCombatTactics();

	UFUNCTION(BlueprintPure, Category = "AI")
	FName GetCurrentRegion() const;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UAIMemoryComponent> MemoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UAINeedsPlanningComponent> NeedsPlanningComponent;

	UFUNCTION()
	void HandleMoraleStateChange(EAIMoraleState NewState);

private:
	void SetupEnhancedComponents();
	void UpdateAdaptiveBehavior();
	void HandleNeedsInterruption();
	void HandleFleeingState();
	float GetMemoryBasedInfluence(EMemoryType MemoryType) const;
	void UpdatePlayerTrust();
	void UpdateDecisionMaking();
};