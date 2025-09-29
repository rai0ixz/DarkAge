#include "AI/DAAIController.h"
#include "AI/DAAIBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Components/AIMoraleComponent.h"
#include "Components/AIMemoryComponent.h"
#include "Components/AINeedsPlanningComponent.h"
#include "Core/CrimeManagerSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/NotorietyComponent.h"
#include "Components/InventoryComponent.h"
#include "Data/InventoryData.h"
#include "Components/AICombatBehaviorComponent.h"

const FName ADAAIController::BlackboardKey_AIState = FName("AIState");
const FName ADAAIController::BlackboardKey_CurrentTactic = FName("CurrentTactic");
const FName ADAAIController::BlackboardKey_SocialTarget = FName("SocialTarget");
const FName ADAAIController::BlackboardKey_NeedsUrgency = FName("NeedsUrgency");
const FName ADAAIController::BlackboardKey_AdaptationLevel = FName("AdaptationLevel");
const FName ADAAIController::BlackboardKey_MoraleState = FName("MoraleState");
const FName ADAAIController::BlackboardKey_PlayerTrust = FName("PlayerTrust");

ADAAIController::ADAAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	MemoryComponent = CreateDefaultSubobject<UAIMemoryComponent>(TEXT("MemoryComponent"));
	NeedsPlanningComponent = CreateDefaultSubobject<UAINeedsPlanningComponent>(TEXT("NeedsPlanningComponent"));

	// Perception system setup
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	UAIPerceptionComponent* Perception = GetPerceptionComponent();
	if (Perception && SightConfig)
	{
		SightConfig->SightRadius = 2000.f;
		SightConfig->LoseSightRadius = 2200.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		Perception->ConfigureSense(*SightConfig);
	}
	if (Perception && HearingConfig)
	{
		HearingConfig->HearingRange = 1500.f;
		HearingConfig->SetMaxAge(5.f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		Perception->ConfigureSense(*HearingConfig);
	}
	if (Perception && SightConfig)
	{
		Perception->SetDominantSense(SightConfig->GetSenseImplementation());
		Perception->OnPerceptionUpdated.AddDynamic(this, &ADAAIController::OnPerceptionUpdated);
	}
}

void ADAAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDecisionMaking();
}

void ADAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADAAIBaseCharacter* AICharacter = Cast<ADAAIBaseCharacter>(InPawn);
	if (AICharacter)
	{
		if (BehaviorTree)
		{
			if (BehaviorTree->BlackboardAsset)
			{
				BlackboardComponent->InitializeBlackboard(*(BehaviorTree->BlackboardAsset));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DAAIController::OnPossess - BehaviorTree for %s has no BlackboardAsset assigned!"), *GetName());
			}
			RunBehaviorTree(BehaviorTree);
		}

		UAIMoraleComponent* MoraleComponent = AICharacter->GetMoraleComponent();
		if (MoraleComponent)
		{
			MoraleComponent->OnMoraleStateChanged.AddDynamic(this, &ADAAIController::HandleMoraleStateChange);
		}
		UpdatePlayerTrust();
	}
}

void ADAAIController::HandleMoraleStateChange(EAIMoraleState NewState)
{
	if (!BlackboardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADAAIController::HandleMoraleStateChange - BlackboardComponent is null for %s"), *GetName());
		return;
	}

	BlackboardComponent->SetValueAsEnum(BlackboardKey_MoraleState, static_cast<uint8>(NewState));
	UE_LOG(LogTemp, Log, TEXT("AI %s morale state changed to: %s. Blackboard updated."), *GetNameSafe(GetPawn()), *UEnum::GetValueAsString(NewState));

	switch (NewState)
	{
		case EAIMoraleState::Fleeing:
			HandleFleeingState();
			break;
		case EAIMoraleState::Surrendered:
			if (BrainComponent)
			{
				BrainComponent->StopLogic("Morale collapsed. Surrendering.");
				UE_LOG(LogTemp, Log, TEXT("AI %s surrendering: Brain logic stopped."), *GetNameSafe(GetPawn()));
			}
			if (ADAAIBaseCharacter* AICharacter = Cast<ADAAIBaseCharacter>(GetPawn()))
			{
				AICharacter->DropWeapon();
				AICharacter->Surrender();
			}
			break;
		case EAIMoraleState::Heroic:
		case EAIMoraleState::High:
		case EAIMoraleState::Average:
		case EAIMoraleState::Low:
		case EAIMoraleState::Broken:
			// These states are primarily for the Behavior Tree to use.
			// The Behavior Tree will now have access to the MoraleState and can react accordingly.
			// For example, a "Broken" AI might cower, while a "Heroic" one might use a special ability.
			break;
	}
}

void ADAAIController::WitnessAndReportCrime(AActor* Perpetrator, ECrimeType CrimeCommitted, float BaseNotorietyValue, const FVector& CrimeLocation)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("DAAIController::WitnessAndReportCrime - Controller has no pawn to act as witness."));
		return;
	}

	if (!Perpetrator)
	{
		UE_LOG(LogTemp, Warning, TEXT("DAAIController::WitnessAndReportCrime - Perpetrator is null."));
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("DAAIController::WitnessAndReportCrime - Failed to get GameInstance."));
		return;
	}

	UCrimeManagerSubsystem* CrimeManager = GameInstance->GetSubsystem<UCrimeManagerSubsystem>();
	if (!CrimeManager)
	{
		UE_LOG(LogTemp, Error, TEXT("DAAIController::WitnessAndReportCrime - Failed to get CrimeManagerSubsystem."));
		return;
	}

	FString RegionID = TEXT("OakhavenStartingRegion"); // Placeholder for region

	const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/DarkAge.ECrimeType"), true);
	FString CrimeString = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CrimeCommitted)) : FString::FromInt(static_cast<int32>(CrimeCommitted));

	UE_LOG(LogTemp, Log, TEXT("AI %s is reporting a crime (type: %s) committed by %s in region %s."),
		*ControlledPawn->GetName(), *CrimeString, *Perpetrator->GetName(), *RegionID);
	CrimeManager->ReportCrime(Perpetrator, ControlledPawn, CrimeCommitted, BaseNotorietyValue, CrimeLocation, RegionID);
}

void ADAAIController::InitializeAdvancedAI()
{
	// Implementation to be added
}

bool ADAAIController::ShouldAdaptBehavior() const
{
	if (MemoryComponent)
	{
		// Example logic: if the AI has any memories, it should consider adapting.
		// A more complex implementation would analyze the memories to make a more informed decision.
		FAIMemory TempMemory; // Dummy variable to check if any memory exists
		return MemoryComponent->GetMemory(FString(), TempMemory);
	}
	return false;
}

bool ADAAIController::HasUrgentNeeds() const
{
	if (NeedsPlanningComponent)
	{
		if (NeedsPlanningComponent->IsNeedUrgent(EAINeed::Hunger) ||
			NeedsPlanningComponent->IsNeedUrgent(EAINeed::Thirst) ||
			NeedsPlanningComponent->IsNeedUrgent(EAINeed::Rest))
		{
			return true;
		}
	}
	return false;
}

void ADAAIController::InitiateSocialInteraction(AActor* TargetActor)
{
	// Implementation to be added
}

void ADAAIController::RecordBehaviorOutcome(const FString& BehaviorName, bool bWasSuccessful)
{
	// Implementation to be added
}

void ADAAIController::UpdateCombatTactics()
{
	// Implementation to be added
}

void ADAAIController::SetupEnhancedComponents()
{
	// Implementation to be added
}

void ADAAIController::UpdateAdaptiveBehavior()
{
	if (ShouldAdaptBehavior() && BlackboardComponent)
	{
		// This is a placeholder for more complex logic.
		// For now, we'll just set a blackboard value to indicate that the AI should adapt.
		BlackboardComponent->SetValueAsBool(BlackboardKey_AdaptationLevel, true);
	}
}

float ADAAIController::GetMemoryBasedInfluence(EMemoryType MemoryType) const
{
	if (!MemoryComponent)
	{
		return 0.f;
	}

	float TotalInfluence = 0.f;
	for (const FAIMemory& Memory : MemoryComponent->GetMemories())
	{
		if (Memory.MemoryType == MemoryType)
		{
			TotalInfluence += Memory.EmotionalImpact * Memory.MemoryStrength;
		}
	}
	return TotalInfluence;
}

void ADAAIController::UpdatePlayerTrust()
{
	if (!BlackboardComponent)
	{
		return;
	}

	float TrustFromKindness = GetMemoryBasedInfluence(EMemoryType::Kindness);
	float DistrustFromBetrayal = GetMemoryBasedInfluence(EMemoryType::Betrayal);

	// Simple calculation for now, can be expanded
	float PlayerTrust = 50.f + TrustFromKindness - DistrustFromBetrayal;
	PlayerTrust = FMath::Clamp(PlayerTrust, 0.f, 100.f);

	BlackboardComponent->SetValueAsFloat(BlackboardKey_PlayerTrust, PlayerTrust);
	UE_LOG(LogTemp, Log, TEXT("AI %s updated player trust to: %f"), *GetNameSafe(GetPawn()), PlayerTrust);
}

void ADAAIController::UpdateDecisionMaking()
{
	if (!NeedsPlanningComponent)
	{
		return;
	}

	FAIGoal CurrentGoal = NeedsPlanningComponent->GetHighestPriorityGoal();
	if (CurrentGoal.Type == EAIGoal::None)
	{
		return; // No urgent needs
	}

	UInventoryComponent* Inventory = GetPawn() ? GetPawn()->FindComponentByClass<UInventoryComponent>() : nullptr;
	if (!Inventory)
	{
		return;
	}

	UEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
	if (!Economy)
	{
		return;
	}

	if (CurrentGoal.Type == EAIGoal::FindFood)
	{
		// Simplified logic: find a food item and try to buy it
		FName FoodItemId = "Bread"; // Example item
		float Price = Economy->GetItemPrice(GetCurrentRegion(), FoodItemId);

		if (Inventory->CanAfford(Price))
		{
			Inventory->RemoveCurrency(Price);
			// Add item to inventory
		          FItemData PurchasedItem;
		          GetGameInstance()->GetSubsystem<UEconomySubsystem>()->GetItemData(FoodItemId, PurchasedItem);
		 Inventory->AddItem(PurchasedItem, 1);
			NeedsPlanningComponent->SatisfyNeed(EAINeed::Hunger, 50.f); // Example value
			UE_LOG(LogTemp, Log, TEXT("AI %s bought %s."), *GetNameSafe(GetPawn()), *FoodItemId.ToString());
		}
		else
		{
			// Can't afford it, consider stealing
			UCrimeManagerSubsystem* CrimeManager = GetGameInstance()->GetSubsystem<UCrimeManagerSubsystem>();
			if (CrimeManager)
			{
				// For simplicity, we'll just log this for now.
				// A real implementation would involve a complex decision-making process.
				UE_LOG(LogTemp, Warning, TEXT("AI %s cannot afford %s and is considering stealing."), *GetNameSafe(GetPawn()), *FoodItemId.ToString());
			}
		}
	}
}

void ADAAIController::HandleNeedsInterruption()
{
	if (HasUrgentNeeds() && BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(BlackboardKey_NeedsUrgency, true);
		// This will trigger a new branch in the behavior tree to handle the urgent need.
		// The behavior tree will then be responsible for clearing this value once the need is met.
	}
}

void ADAAIController::HandleFleeingState()
{
	if (BrainComponent)
	{
		// We could have a specific "Flee" behavior in the tree,
		// but for now, stopping logic is a clear response.
		BrainComponent->StopLogic("Morale broken. Fleeing.");
		UE_LOG(LogTemp, Log, TEXT("AI %s is fleeing: Brain logic stopped."), *GetNameSafe(GetPawn()));
	}

	if (ADAAIBaseCharacter* AICharacter = Cast<ADAAIBaseCharacter>(GetPawn()))
	{
		AICharacter->DropWeapon();
		// Here you would typically trigger a "flee" animation and have the AI run away from the threat.
		// This logic would likely be in a Behavior Tree Task that is triggered by the Fleeing state.
		UE_LOG(LogTemp, Log, TEXT("AI %s is now in a fleeing state."), *GetNameSafe(GetPawn()));
	}
}

void ADAAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	UAIPerceptionComponent* Perception = GetPerceptionComponent();
	if (!Perception || !BlackboardComponent)
		return;

	AActor* BestTarget = nullptr;
	float ClosestDist = TNumericLimits<float>::Max();

	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		Perception->GetActorsPerception(Actor, Info);
		bool bCanSee = false;
		for (const FAIStimulus& Stim : Info.LastSensedStimuli)
		{
			if (Stim.WasSuccessfullySensed())
			{
				bCanSee = true;
				break;
			}
		}
		if (bCanSee && Actor && Actor != GetPawn())
		{
			float Dist = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				BestTarget = Actor;
			}
		}
	}

	// Find the AICombatBehaviorComponent
	UAICombatBehaviorComponent* CombatComp = GetPawn() ? GetPawn()->FindComponentByClass<UAICombatBehaviorComponent>() : nullptr;

	if (BestTarget)
	{
		BlackboardComponent->SetValueAsObject(FName("TargetActor"), BestTarget);
		if (CombatComp)
		{
			CombatComp->SetTarget(BestTarget);
		}
	}
	else
	{
		BlackboardComponent->ClearValue(FName("TargetActor"));
		if (CombatComp)
		{
			CombatComp->ClearTarget();
		}
	}
}

FName ADAAIController::GetCurrentRegion() const
{
			 // This is a placeholder. In a real game, you would get the AI's current region from the game state.
			 return FName(TEXT("Heartlands"));
}