#include "Components/AINeedsPlanningComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Core/DA_NPCManagerSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

UAINeedsPlanningComponent::UAINeedsPlanningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentGoalProgress = 0.0f;
}

void UAINeedsPlanningComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeNeeds();

	   if (UWorld* World = GetWorld())
	   {
	       if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
	       {
	           Manager->RegisterComponent(this);
	       }
	   }
}

void UAINeedsPlanningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	   if (UWorld* World = GetWorld())
	   {
	       if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
	       {
	           Manager->UnregisterComponent(this);
	       }
	   }

	   Super::EndPlay(EndPlayReason);
}

void UAINeedsPlanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ALL LOGIC IS NOW HANDLED BY UDA_NPCManagerSubsystem
}

void UAINeedsPlanningComponent::ManagedTick(float DeltaTime)
{
	   // This is the logic that used to be in TickComponent
	   UpdateNeedsWithContext(DeltaTime);

	   GoalExecutionTimer += DeltaTime;
	   if (GoalExecutionTimer >= 3.0f) // Execute every 3 seconds
	   {
	       ExecuteCurrentGoal();
	       GoalExecutionTimer = 0.0f;
	   }
}

void UAINeedsPlanningComponent::UpdateNeedsWithContext(float DeltaTime)
{
	float TimeOfDay = GetTimeOfDay();
	float DangerLevel = EvaluateDangerLevel();
	
	for (auto& Elem : Needs)
	{
		FNeed& Need = Elem.Value;
		float ModifiedDecayRate = Need.DecayRate;
		
		// Contextual decay rate modifications
		switch (Elem.Key)
		{
		case EAINeed::Hunger:
			// Hunger increases faster during work hours
			if (TimeOfDay >= 8.0f && TimeOfDay <= 17.0f)
				ModifiedDecayRate *= 1.5f;
			break;
			
		case EAINeed::Thirst:
			// Thirst increases faster in hot weather or during physical activity
			ModifiedDecayRate *= 1.2f;
			break;
			
		case EAINeed::Rest:
			// Rest need increases faster at night
			if (TimeOfDay >= 20.0f || TimeOfDay <= 6.0f)
				ModifiedDecayRate *= 2.0f;
			break;
			
		case EAINeed::Safety:
			// Safety need increases with danger level
			ModifiedDecayRate *= (1.0f + DangerLevel * 2.0f);
			break;
			
		case EAINeed::Social:
			// Social need increases when isolated
			ModifiedDecayRate *= GetSocialIsolationMultiplier();
			break;
		}
		
		Need.Value -= ModifiedDecayRate * DeltaTime;
		Need.Value = FMath::Clamp(Need.Value, 0.f, 100.f);
	}
}

// Advanced goal execution with contextual decision making
void UAINeedsPlanningComponent::ExecuteCurrentGoal()
{
	FAIGoal Goal = GetHighestPriorityGoal();
	
	// Advanced contextual planning based on environment and social factors
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	float TimeOfDay = GetTimeOfDay();
	bool bIsInDanger = EvaluateDangerLevel() > 0.7f;
	
	switch (Goal.Type)
	{
	case EAIGoal::FindFood:
		ExecuteFoodSearchBehavior(OwnerLocation, TimeOfDay, bIsInDanger);
		break;
	case EAIGoal::FindWater:
		ExecuteWaterSearchBehavior(OwnerLocation, TimeOfDay, bIsInDanger);
		break;
	case EAIGoal::FindShelter:
		ExecuteShelterSearchBehavior(OwnerLocation, TimeOfDay, bIsInDanger);
		break;
	case EAIGoal::IncreaseSafety:
		ExecuteSafetyBehavior(OwnerLocation, TimeOfDay);
		break;
	case EAIGoal::FulfillSocial:
		ExecuteSocialBehavior(OwnerLocation, TimeOfDay, bIsInDanger);
		break;
	case EAIGoal::Work:
		ExecuteWorkBehavior(OwnerLocation, TimeOfDay);
		break;
	case EAIGoal::Trade:
		ExecuteTradeBehavior(OwnerLocation, TimeOfDay);
		break;
	default:
		ExecuteIdleBehavior(OwnerLocation, TimeOfDay);
		break;
	}
	
	// Update goal completion status
	UpdateGoalProgress(Goal);
}

void UAINeedsPlanningComponent::ExecuteFoodSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger)
{
	if (bInDanger)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI postponing food search due to danger - seeking safety first"));
		return;
	}
	
	// Time-based food search strategy
	if (TimeOfDay >= 6.0f && TimeOfDay <= 18.0f) // Daytime
	{
		UE_LOG(LogTemp, Log, TEXT("AI searching for food during daylight - checking berry bushes and hunting small game"));
		AttemptForaging();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI avoiding food search at night - too dangerous"));
		// Defer to safety behavior at night
		SatisfyNeed(EAINeed::Safety, -10.0f); // Increase safety need
	}
}

void UAINeedsPlanningComponent::ExecuteWaterSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger)
{
	if (bInDanger)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI postponing water search due to danger"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("AI searching for clean water source - checking wells and streams"));
	AttemptWaterCollection();
}

void UAINeedsPlanningComponent::ExecuteShelterSearchBehavior(const FVector& Location, float TimeOfDay, bool bInDanger)
{
	if (TimeOfDay >= 20.0f || TimeOfDay <= 6.0f) // Night time
	{
		UE_LOG(LogTemp, Log, TEXT("AI seeking shelter for the night - prioritizing safety"));
		AttemptShelterSeeking(true); // High priority
	}
	else if (bInDanger)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI seeking emergency shelter due to danger"));
		AttemptShelterSeeking(true);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI casually looking for better shelter options"));
		AttemptShelterSeeking(false); // Low priority
	}
}

void UAINeedsPlanningComponent::ExecuteSafetyBehavior(const FVector& Location, float TimeOfDay)
{
	float DangerLevel = EvaluateDangerLevel();
	
	if (DangerLevel > 0.8f)
	{
		UE_LOG(LogTemp, Error, TEXT("AI in extreme danger - fleeing to safety"));
		AttemptFlee();
	}
	else if (DangerLevel > 0.5f)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI sensing moderate danger - seeking defensive position"));
		AttemptDefensivePosition();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI maintaining vigilance - patrolling area"));
		AttemptPatrol();
	}
}

void UAINeedsPlanningComponent::ExecuteSocialBehavior(const FVector& Location, float TimeOfDay, bool bInDanger)
{
	if (bInDanger)
	{
		UE_LOG(LogTemp, Log, TEXT("AI seeking help from others due to danger"));
		AttemptSeekHelp();
		return;
	}
	
	if (TimeOfDay >= 18.0f && TimeOfDay <= 22.0f) // Evening social time
	{
		UE_LOG(LogTemp, Log, TEXT("AI heading to tavern for evening social interaction"));
		AttemptTavernSocialization();
	}
	else if (TimeOfDay >= 12.0f && TimeOfDay <= 14.0f) // Lunch break
	{
		UE_LOG(LogTemp, Log, TEXT("AI taking lunch break - casual social interaction"));
		AttemptCasualSocialization();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI engaging in work-related social interaction"));
		AttemptWorkSocialization();
	}
}

void UAINeedsPlanningComponent::ExecuteWorkBehavior(const FVector& Location, float TimeOfDay)
{
	if (TimeOfDay >= 8.0f && TimeOfDay <= 17.0f) // Work hours
	{
		UE_LOG(LogTemp, Log, TEXT("AI performing daily work tasks"));
		AttemptWork();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI outside work hours - maintaining equipment"));
		AttemptMaintenance();
	}
}

void UAINeedsPlanningComponent::ExecuteTradeBehavior(const FVector& Location, float TimeOfDay)
{
	if (TimeOfDay >= 9.0f && TimeOfDay <= 16.0f) // Market hours
	{
		UE_LOG(LogTemp, Log, TEXT("AI engaging in trade activities"));
		AttemptTrading();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI preparing goods for tomorrow's trade"));
		AttemptTradePreparation();
	}
}

void UAINeedsPlanningComponent::ExecuteIdleBehavior(const FVector& Location, float TimeOfDay)
{
	UE_LOG(LogTemp, Log, TEXT("AI in idle state - performing maintenance activities"));
	// Random idle activities based on personality
	int32 RandomActivity = FMath::RandRange(0, 2);
	switch (RandomActivity)
	{
	case 0:
		UE_LOG(LogTemp, Log, TEXT("AI cleaning and organizing"));
		break;
	case 1:
		UE_LOG(LogTemp, Log, TEXT("AI observing surroundings"));
		break;
	case 2:
		UE_LOG(LogTemp, Log, TEXT("AI resting and contemplating"));
		break;
	}
}

void UAINeedsPlanningComponent::SatisfyNeed(EAINeed NeedType, float Amount)
{
	if (FNeed* Need = Needs.Find(NeedType))
	{
		Need->Value = FMath::Clamp(Need->Value + Amount, 0.f, 100.f);
		
		// Log significant need changes
		if (FMath::Abs(Amount) > 20.0f)
		{
			UE_LOG(LogTemp, Log, TEXT("AI need %d significantly changed by %f, new value: %f"), 
				(int32)NeedType, Amount, Need->Value);
		}
	}
}

float UAINeedsPlanningComponent::GetNeedValue(EAINeed NeedType) const
{
	if (const FNeed* Need = Needs.Find(NeedType))
	{
		return Need->Value;
	}
	return 0.f;
}

bool UAINeedsPlanningComponent::IsNeedUrgent(EAINeed NeedType) const
{
	if (const FNeed* Need = Needs.Find(NeedType))
	{
		return Need->Value < Need->UrgencyThreshold;
	}
	return false;
}

FAIGoal UAINeedsPlanningComponent::GetHighestPriorityGoal() const
{
	FAIGoal HighestPriorityGoal;
	HighestPriorityGoal.Priority = -1.f;
	HighestPriorityGoal.Type = EAIGoal::None;

	for (const auto& Elem : Needs)
	{
		const FNeed& Need = Elem.Value;
		if (Need.Value < Need.UrgencyThreshold)
		{
			// Advanced priority calculation considering multiple factors
			float BasePriority = (100.f - Need.Value) / 100.f;
			float ContextualModifier = GetContextualPriorityModifier(Elem.Key);
			float FinalPriority = BasePriority * ContextualModifier;
			
			if (FinalPriority > HighestPriorityGoal.Priority)
			{
				HighestPriorityGoal.Priority = FinalPriority;
				HighestPriorityGoal.Type = DetermineGoalFromNeed(Elem.Key);
			}
		}
	}

	// If no urgent needs, determine work/social goals based on time
	if (HighestPriorityGoal.Type == EAIGoal::None)
	{
		float TimeOfDay = GetTimeOfDay();
		if (TimeOfDay >= 8.0f && TimeOfDay <= 17.0f)
		{
			HighestPriorityGoal.Type = EAIGoal::Work;
			HighestPriorityGoal.Priority = 0.5f;
		}
		else if (TimeOfDay >= 18.0f && TimeOfDay <= 22.0f)
		{
			HighestPriorityGoal.Type = EAIGoal::FulfillSocial;
			HighestPriorityGoal.Priority = 0.3f;
		}
	}

	return HighestPriorityGoal;
}

float UAINeedsPlanningComponent::GetContextualPriorityModifier(EAINeed NeedType) const
{
	float TimeOfDay = GetTimeOfDay();
	float DangerLevel = EvaluateDangerLevel();
	
	switch (NeedType)
	{
	case EAINeed::Safety:
		return 1.0f + DangerLevel * 2.0f; // Safety becomes more important with danger
		
	case EAINeed::Rest:
		// Rest is more important at night
		if (TimeOfDay >= 20.0f || TimeOfDay <= 6.0f)
			return 1.5f;
		return 1.0f;
		
	case EAINeed::Social:
		// Social needs are more important during social hours
		if ((TimeOfDay >= 12.0f && TimeOfDay <= 14.0f) || (TimeOfDay >= 18.0f && TimeOfDay <= 22.0f))
			return 1.3f;
		return 0.8f;
		
	default:
		return 1.0f;
	}
}

void UAINeedsPlanningComponent::InitializeNeeds()
{
	// Initialize with more sophisticated need configurations
	FNeed HungerNeed;
	HungerNeed.Value = 80.0f;
	HungerNeed.DecayRate = 1.0f; // Loses 1 point per second
	HungerNeed.UrgencyThreshold = 30.0f;
	Needs.Add(EAINeed::Hunger, HungerNeed);
	
	FNeed ThirstNeed;
	ThirstNeed.Value = 75.0f;
	ThirstNeed.DecayRate = 1.5f; // Loses 1.5 points per second
	ThirstNeed.UrgencyThreshold = 25.0f;
	Needs.Add(EAINeed::Thirst, ThirstNeed);
	
	FNeed RestNeed;
	RestNeed.Value = 90.0f;
	RestNeed.DecayRate = 0.5f; // Loses 0.5 points per second
	RestNeed.UrgencyThreshold = 20.0f;
	Needs.Add(EAINeed::Rest, RestNeed);
	
	FNeed SafetyNeed;
	SafetyNeed.Value = 70.0f;
	SafetyNeed.DecayRate = 0.3f; // Loses 0.3 points per second
	SafetyNeed.UrgencyThreshold = 40.0f;
	Needs.Add(EAINeed::Safety, SafetyNeed);
	
	FNeed SocialNeed;
	SocialNeed.Value = 60.0f;
	SocialNeed.DecayRate = 0.2f; // Loses 0.2 points per second
	SocialNeed.UrgencyThreshold = 30.0f;
	Needs.Add(EAINeed::Social, SocialNeed);
}

EAIGoal UAINeedsPlanningComponent::DetermineGoalFromNeed(EAINeed Need) const
{
	switch (Need)
	{
		case EAINeed::Hunger:
			return EAIGoal::FindFood;
		case EAINeed::Thirst:
			return EAIGoal::FindWater;
		case EAINeed::Rest:
			return EAIGoal::FindShelter;
		case EAINeed::Safety:
			return EAIGoal::IncreaseSafety;
		case EAINeed::Social:
			return EAIGoal::FulfillSocial;
		default:
			return EAIGoal::None;
	}
}

float UAINeedsPlanningComponent::GetTimeOfDay() const
{
	if (UWorld* World = GetWorld())
	{
		// Simplified time calculation - in a real implementation, this would use the TimeSystem
		float GameTime = World->GetTimeSeconds();
		return FMath::Fmod(GameTime / 60.0f, 24.0f); // Convert to 24-hour format
	}
	return 12.0f; // Default to noon
}

float UAINeedsPlanningComponent::EvaluateDangerLevel() const
{
	float DangerLevel = 0.0f;

	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			FVector OwnerLocation = GetOwner()->GetActorLocation();

			// Use cached hostile actors from manager
			TArray<AActor*> NearbyHostiles = Manager->GetNearbyHostiles(OwnerLocation, 500.0f);

			for (AActor* Hostile : NearbyHostiles)
			{
				if (Hostile && Hostile != GetOwner())
				{
					float Distance = FVector::Dist(OwnerLocation, Hostile->GetActorLocation());
					DangerLevel += (500.0f - Distance) / 500.0f; // Closer = more dangerous
				}
			}
		}
	}

	return FMath::Clamp(DangerLevel, 0.0f, 1.0f);
}

float UAINeedsPlanningComponent::GetSocialIsolationMultiplier() const
{
	// Check for nearby friendly NPCs using cached data
	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			FVector OwnerLocation = GetOwner()->GetActorLocation();
			TArray<AActor*> NearbyNPCs = Manager->GetNearbyFriendlyNPCs(OwnerLocation, 1000.0f);

			int32 FriendlyNearby = NearbyNPCs.Num();

			// More isolation = higher multiplier
			return FMath::Max(1.0f, 3.0f - FriendlyNearby * 0.5f);
		}
	}

	return 1.5f; // Default moderate isolation
}

void UAINeedsPlanningComponent::UpdateGoalProgress(const FAIGoal& Goal)
{
	// Track goal completion and adjust needs accordingly
	CurrentGoalProgress += 0.1f; // Increment progress
	
	if (CurrentGoalProgress >= 1.0f)
	{
		// Goal completed - satisfy the related need
		switch (Goal.Type)
		{
		case EAIGoal::FindFood:
			SatisfyNeed(EAINeed::Hunger, 30.0f);
			break;
		case EAIGoal::FindWater:
			SatisfyNeed(EAINeed::Thirst, 35.0f);
			break;
		case EAIGoal::FindShelter:
			SatisfyNeed(EAINeed::Rest, 25.0f);
			SatisfyNeed(EAINeed::Safety, 20.0f);
			break;
		case EAIGoal::IncreaseSafety:
			SatisfyNeed(EAINeed::Safety, 40.0f);
			break;
		case EAIGoal::FulfillSocial:
			SatisfyNeed(EAINeed::Social, 30.0f);
			break;
		case EAIGoal::Work:
			SatisfyNeed(EAINeed::Hunger, -5.0f); // Work makes you hungrier
			SatisfyNeed(EAINeed::Social, 10.0f); // But provides some social interaction
			break;
		case EAIGoal::Trade:
			SatisfyNeed(EAINeed::Social, 15.0f); // Trading is social
			break;
		}
		
		CurrentGoalProgress = 0.0f; // Reset progress
		UE_LOG(LogTemp, Log, TEXT("AI completed goal: %d"), (int32)Goal.Type);
	}
}

// Advanced behavior implementations
void UAINeedsPlanningComponent::AttemptForaging()
{
	UE_LOG(LogTemp, Log, TEXT("AI attempting to forage for berries and edible plants"));

	if (!GetOwner()) return;

	// Get nearby resource nodes or edible plants
	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			FVector OwnerLocation = GetOwner()->GetActorLocation();

			// Look for nearby edible resources (this would be expanded with actual resource system)
			// For now, simulate finding food with some probability
			float ForageSuccessChance = 0.6f; // 60% success rate

			if (FMath::FRand() < ForageSuccessChance)
			{
				// Successful foraging
				float FoodFound = FMath::RandRange(10.0f, 25.0f);
				SatisfyNeed(EAINeed::Hunger, FoodFound);
				UE_LOG(LogTemp, Log, TEXT("AI successfully foraged %.1f food units"), FoodFound);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("AI failed to find edible resources"));
				// Could trigger movement to different location or different strategy
			}
		}
	}
}

void UAINeedsPlanningComponent::AttemptWaterCollection()
{
	UE_LOG(LogTemp, Log, TEXT("AI attempting to collect clean water"));

	if (!GetOwner()) return;

	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			FVector OwnerLocation = GetOwner()->GetActorLocation();

			// Look for nearby water sources
			TArray<AActor*> NearbyWater = Manager->GetNearbyWaterSources(OwnerLocation, 500.0f);

			if (NearbyWater.Num() > 0)
			{
				// Found water source
				float WaterCollected = FMath::RandRange(15.0f, 30.0f);
				SatisfyNeed(EAINeed::Thirst, WaterCollected);
				UE_LOG(LogTemp, Log, TEXT("AI collected %.1f water units from nearby source"), WaterCollected);
			}
			else
			{
				// No nearby water - could implement movement to find water
				UE_LOG(LogTemp, Log, TEXT("AI found no nearby water sources"));
				// Could trigger movement behavior to find water
			}
		}
	}
}

void UAINeedsPlanningComponent::AttemptShelterSeeking(bool bHighPriority)
{
	if (!GetOwner()) return;

	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			FVector OwnerLocation = GetOwner()->GetActorLocation();

			// Look for nearby shelters
			TArray<AActor*> NearbyShelters = Manager->GetNearbyShelters(OwnerLocation, 1000.0f);

			if (NearbyShelters.Num() > 0)
			{
				// Found shelter
				float RestGained = bHighPriority ? FMath::RandRange(20.0f, 35.0f) : FMath::RandRange(10.0f, 20.0f);
				SatisfyNeed(EAINeed::Rest, RestGained);

				if (bHighPriority)
				{
					// Also reduce safety need when finding emergency shelter
					SatisfyNeed(EAINeed::Safety, 15.0f);
					UE_LOG(LogTemp, Warning, TEXT("AI found emergency shelter, gained %.1f rest and reduced safety concerns"), RestGained);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("AI found comfortable shelter, gained %.1f rest"), RestGained);
				}
			}
			else
			{
				if (bHighPriority)
				{
					UE_LOG(LogTemp, Warning, TEXT("AI urgently needs shelter but found none nearby"));
					// Could implement emergency behavior like sleeping outdoors with penalties
					SatisfyNeed(EAINeed::Rest, 5.0f); // Minimal rest
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("AI found no better shelter options"));
				}
			}
		}
	}
}

void UAINeedsPlanningComponent::AttemptFlee()
{
	UE_LOG(LogTemp, Error, TEXT("AI fleeing from immediate danger"));
	// Implementation would involve pathfinding away from threats
}

void UAINeedsPlanningComponent::AttemptDefensivePosition()
{
	UE_LOG(LogTemp, Warning, TEXT("AI taking defensive position"));
	// Implementation would involve finding cover or high ground
}

void UAINeedsPlanningComponent::AttemptPatrol()
{
	UE_LOG(LogTemp, Log, TEXT("AI patrolling area for threats"));
	// Implementation would involve moving in a patrol pattern
}

void UAINeedsPlanningComponent::AttemptSeekHelp()
{
	UE_LOG(LogTemp, Warning, TEXT("AI seeking help from nearby allies"));
	// Implementation would involve finding friendly NPCs
}

void UAINeedsPlanningComponent::AttemptTavernSocialization()
{
	UE_LOG(LogTemp, Log, TEXT("AI heading to tavern for evening drinks and conversation"));
	// Implementation would involve pathfinding to tavern
}

void UAINeedsPlanningComponent::AttemptCasualSocialization()
{
	UE_LOG(LogTemp, Log, TEXT("AI engaging in casual conversation"));
	// Implementation would involve finding nearby NPCs for chat
}

void UAINeedsPlanningComponent::AttemptWorkSocialization()
{
	UE_LOG(LogTemp, Log, TEXT("AI discussing work matters with colleagues"));
	// Implementation would involve work-related dialogue
}

void UAINeedsPlanningComponent::AttemptWork()
{
	UE_LOG(LogTemp, Log, TEXT("AI performing primary work duties"));

	if (!GetOwner()) return;

	// Work provides income and social interaction
	float WorkIncome = FMath::RandRange(5.0f, 15.0f);
	float SocialGain = FMath::RandRange(5.0f, 10.0f);

	// Different NPC types have different work behaviors
	FString NPCTag = GetOwner()->Tags.Num() > 0 ? GetOwner()->Tags[0].ToString() : "Generic";

	if (NPCTag.Contains("Miller"))
	{
		UE_LOG(LogTemp, Log, TEXT("Miller grinding grain - earned %.1f gold, gained %.1f social"), WorkIncome, SocialGain);
	}
	else if (NPCTag.Contains("Landowner"))
	{
		WorkIncome *= 1.5f; // Landowners earn more
		UE_LOG(LogTemp, Log, TEXT("Landowner managing estate - earned %.1f gold, gained %.1f social"), WorkIncome, SocialGain);
	}
	else if (NPCTag.Contains("TavernKeeper"))
	{
		SocialGain *= 1.3f; // Tavern keepers get more social interaction
		UE_LOG(LogTemp, Log, TEXT("TavernKeeper serving customers - earned %.1f gold, gained %.1f social"), WorkIncome, SocialGain);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Generic NPC working - earned %.1f gold, gained %.1f social"), WorkIncome, SocialGain);
	}

	// Satisfy social need from work interaction
	SatisfyNeed(EAINeed::Social, SocialGain);

	// Work makes you hungrier
	SatisfyNeed(EAINeed::Hunger, -8.0f);

	// In a full implementation, this would add gold to NPC's inventory
}

void UAINeedsPlanningComponent::AttemptMaintenance()
{
	UE_LOG(LogTemp, Log, TEXT("AI maintaining tools and workspace"));

	if (!GetOwner()) return;

	// Maintenance provides some rest but costs time
	float RestGain = FMath::RandRange(3.0f, 8.0f);
	float TimeCost = FMath::RandRange(10.0f, 20.0f); // In minutes

	// Different maintenance activities based on NPC type
	FString NPCTag = GetOwner()->Tags.Num() > 0 ? GetOwner()->Tags[0].ToString() : "Generic";

	if (NPCTag.Contains("Miller"))
	{
		UE_LOG(LogTemp, Log, TEXT("Miller maintaining mill equipment - gained %.1f rest (%.1f min)"), RestGain, TimeCost);
	}
	else if (NPCTag.Contains("Landowner"))
	{
		UE_LOG(LogTemp, Log, TEXT("Landowner maintaining estate - gained %.1f rest (%.1f min)"), RestGain, TimeCost);
	}
	else if (NPCTag.Contains("TavernKeeper"))
	{
		UE_LOG(LogTemp, Log, TEXT("TavernKeeper cleaning tavern - gained %.1f rest (%.1f min)"), RestGain, TimeCost);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Generic NPC performing maintenance - gained %.1f rest (%.1f min)"), RestGain, TimeCost);
	}

	// Satisfy rest need from maintenance work
	SatisfyNeed(EAINeed::Rest, RestGain);

	// Maintenance can be slightly social if done with others
	SatisfyNeed(EAINeed::Social, 2.0f);
}

void UAINeedsPlanningComponent::AttemptTrading()
{
	UE_LOG(LogTemp, Log, TEXT("AI actively trading goods"));

	if (!GetOwner()) return;

	// Trading provides gold and social interaction
	float TradeProfit = FMath::RandRange(8.0f, 20.0f);
	float SocialGain = FMath::RandRange(8.0f, 15.0f);

	// Success chance for trade
	float TradeSuccessChance = 0.7f; // 70% success rate

	if (FMath::FRand() < TradeSuccessChance)
	{
		UE_LOG(LogTemp, Log, TEXT("AI successful trade - profit: %.1f gold, social gain: %.1f"), TradeProfit, SocialGain);

		// Satisfy social need from trade interaction
		SatisfyNeed(EAINeed::Social, SocialGain);

		// Trading can be tiring
		SatisfyNeed(EAINeed::Rest, -5.0f);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AI trade failed - lost potential profit"));
		TradeProfit *= 0.3f; // Small loss
		SocialGain *= 0.5f; // Less social interaction

		SatisfyNeed(EAINeed::Social, SocialGain);
	}

	// In a full implementation, this would affect NPC's inventory and gold
}

void UAINeedsPlanningComponent::AttemptTradePreparation()
{
	UE_LOG(LogTemp, Log, TEXT("AI preparing goods for trade"));
	// Implementation would involve inventory management
}