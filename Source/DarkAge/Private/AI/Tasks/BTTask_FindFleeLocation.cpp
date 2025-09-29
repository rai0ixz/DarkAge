#include "AI/Tasks/BTTask_FindFleeLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindFleeLocation::UBTTask_FindFleeLocation()
{
	NodeName = "Find Flee Location";
}

EBTNodeResult::Type UBTTask_FindFleeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	APawn* ControlledPawn = AIController->GetPawn();

	if (!AIController || !BlackboardComp || !ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// Get player location (the threat to flee from)
	if (UWorld* World = GetWorld())
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (!PlayerPawn)
		{
			return EBTNodeResult::Failed;
		}

		// Find a location on the NavMesh away from the player
		FVector FleeDirection = (ControlledPawn->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
		FVector FleeLocation = ControlledPawn->GetActorLocation() + FleeDirection * 1500.0f; // Flee 1500 units away

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
		if (NavSys)
		{
			FNavLocation RandomPoint;
			// Project the point onto the navigation mesh to ensure it's reachable
			if (NavSys->GetRandomPointInNavigableRadius(FleeLocation, 500.0f, RandomPoint))
			{
				BlackboardComp->SetValueAsVector(FleeLocationKey.SelectedKeyName, RandomPoint.Location);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
