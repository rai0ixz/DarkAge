// Copyright (c) 2024, Raio Inc. All rights reserved.

#include "Components/AICombatBehaviorComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StatlineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "NavigationSystem.h"

// Combat action stubs
void UAICombatBehaviorComponent::PerformAttack()
{
    if (CurrentTarget)
    {
        UE_LOG(LogTemp, Log, TEXT("%s performs an attack on %s!"), *GetOwner()->GetName(), *CurrentTarget->GetName());
        UStatlineComponent* TargetStats = CurrentTarget->FindComponentByClass<UStatlineComponent>();
        if (TargetStats)
        {
            // Assuming a base damage of 10 for now
            TargetStats->UpdateStat(FName("Health"), -10.0f);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s tried to attack but has no target!"), *GetOwner()->GetName());
    }
}

void UAICombatBehaviorComponent::FleeFromCombat()
{
    UE_LOG(LogTemp, Log, TEXT("%s is fleeing from combat!"), *GetOwner()->GetName());
    EnterCombatState(EAICombatState::Flee);
}
// Sets default values for this component's properties
UAICombatBehaviorComponent::UAICombatBehaviorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CombatState = EAICombatState::Idle;
    CurrentTarget = nullptr;
}

void UAICombatBehaviorComponent::BeginPlay()
{
    Super::BeginPlay();
    CombatState = EAICombatState::Idle;
    CurrentTarget = nullptr;
}

void UAICombatBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    HandleCombatState(DeltaTime);
}

void UAICombatBehaviorComponent::SetTarget(AActor* NewTarget)
{
    CurrentTarget = NewTarget;
    if (CurrentTarget)
    {
        EnterCombatState(EAICombatState::Chase);
    }
    else
    {
        EnterCombatState(EAICombatState::Idle);
    }
}

void UAICombatBehaviorComponent::ClearTarget()
{
    CurrentTarget = nullptr;
    EnterCombatState(EAICombatState::Idle);
}

void UAICombatBehaviorComponent::EnterCombatState(EAICombatState NewState)
{
    CombatState = NewState;
}

void UAICombatBehaviorComponent::HandleCombatState(float DeltaTime)
{
    switch (CombatState)
    {
    case EAICombatState::Idle:
        HandleIdleState(DeltaTime);
        break;
    case EAICombatState::Patrol:
        HandlePatrolState(DeltaTime);
        break;
    case EAICombatState::Chase:
        HandleChaseState(DeltaTime);
        break;
    case EAICombatState::Attack:
        HandleAttackState(DeltaTime);
        break;
    case EAICombatState::Flee:
        HandleFleeState(DeltaTime);
        break;
    default:
        break;
    }
}

void UAICombatBehaviorComponent::HandleIdleState(float DeltaTime)
{
    // Scan for nearby enemies
    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> OverlappingActors;
        UGameplayStatics::GetAllActorsOfClass(World, ACharacter::StaticClass(), OverlappingActors);

        for (AActor* Actor : OverlappingActors)
        {
            if (Actor != GetOwner() && FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation()) < 1000.0f)
            {
                // Basic check for hostility (e.g., different faction)
                // This should be replaced with a proper faction check
                SetTarget(Actor);
                return;
            }
        }
    }
}

void UAICombatBehaviorComponent::HandlePatrolState(float DeltaTime)
{
    // This is a stub. A full implementation would require a patrol path.
    // For now, we just transition back to idle to allow for threat scanning.
    EnterCombatState(EAICombatState::Idle);
}

void UAICombatBehaviorComponent::HandleChaseState(float DeltaTime)
{
    if (CurrentTarget)
    {
        AAIController* AIController = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController());
        if (AIController)
        {
            AIController->MoveToActor(CurrentTarget, 100.0f);
        }

        // Example: If close enough, switch to attack
        float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());
        if (Distance < 200.0f) // Example attack range
        {
            EnterCombatState(EAICombatState::Attack);
        }
    }
    else
    {
        EnterCombatState(EAICombatState::Idle);
    }
}

void UAICombatBehaviorComponent::HandleAttackState(float DeltaTime)
{
    if (CurrentTarget)
    {
        float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());
        if (Distance < 200.0f)
        {
            PerformAttack();
        }
        else
        {
            EnterCombatState(EAICombatState::Chase);
        }
    }
    else
    {
        EnterCombatState(EAICombatState::Idle);
    }
}

void UAICombatBehaviorComponent::HandleFleeState(float DeltaTime)
{
    if (CurrentTarget)
    {
        if (UWorld* World = GetWorld())
        {
            AAIController* AIController = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController());
            UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
            if (AIController && NavSys)
            {
                FVector DirectionAwayFromTarget = (GetOwner()->GetActorLocation() - CurrentTarget->GetActorLocation()).GetSafeNormal();
                FVector TentativeFleeLocation = GetOwner()->GetActorLocation() + DirectionAwayFromTarget * 500.0f; // 500 units away

                FNavLocation NavLocation;
                if (NavSys->GetRandomPointInNavigableRadius(TentativeFleeLocation, 100.0f, NavLocation))
                {
                    AIController->MoveToLocation(NavLocation.Location);
                }
            }
        }
    }
    else
    {
        // If we are fleeing but have no target, go back to idle
        EnterCombatState(EAICombatState::Idle);
    }
}