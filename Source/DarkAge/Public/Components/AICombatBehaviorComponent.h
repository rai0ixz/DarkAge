// Copyright (c) 2024, Raio Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AICombatBehaviorComponent.generated.h"

UENUM(BlueprintType)
enum class EAICombatState : uint8
{
    Idle,
    Patrol,
    Chase,
    Attack,
    Flee
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UAICombatBehaviorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UAICombatBehaviorComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    EAICombatState CombatState = EAICombatState::Idle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    AActor* CurrentTarget = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ClearTarget();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EnterCombatState(EAICombatState NewState);

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void HandleCombatState(float DeltaTime);
    void HandleIdleState(float DeltaTime);
    void HandlePatrolState(float DeltaTime);
    void HandleChaseState(float DeltaTime);
    void HandleAttackState(float DeltaTime);
    void HandleFleeState(float DeltaTime);
        
        // Combat action stubs
        UFUNCTION(BlueprintCallable, Category = "Combat")
        void PerformAttack();
        
        UFUNCTION(BlueprintCallable, Category = "Combat")
        void FleeFromCombat();
};