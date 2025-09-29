#include "Components/AIMemoryComponent.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/GlobalEventBus.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

// Query memories by type
TArray<FAIMemory> UAIMemoryComponent::GetMemoriesByType(EMemoryType MemoryType) const
{
	TArray<FAIMemory> Result;
	for (const FAIMemory& Memory : Memories)
	{
		if (Memory.MemoryType == MemoryType)
		{
			Result.Add(Memory);
		}
	}
	return Result;
}

// Query memories by context key/value
TArray<FAIMemory> UAIMemoryComponent::GetMemoriesByContext(FName ContextKey, FString ContextValue) const
{
	TArray<FAIMemory> Result;
	for (const FAIMemory& Memory : Memories)
	{
		if (Memory.Context.Contains(ContextKey) && Memory.Context[ContextKey] == ContextValue)
		{
			Result.Add(Memory);
		}
	}
	return Result;
}

UAIMemoryComponent::UAIMemoryComponent()
	: FactionManager(nullptr)
	, GlobalEventBus(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	MemoryDecayRate = 0.1f;
}

void UAIMemoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Get references to subsystems
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			FactionManager = GameInstance->GetSubsystem<UFactionManagerSubsystem>();
			GlobalEventBus = GameInstance->GetSubsystem<UGlobalEventBus>();
		}
	}
	
	// Register with event bus
	RegisterWithEventBus();
}

void UAIMemoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Decay memory strength over time
	for (FAIMemory& Memory : Memories)
	{
		Memory.MemoryStrength -= MemoryDecayRate * DeltaTime;
	}

	// Remove memories with strength <= 0
	Memories.RemoveAll([](const FAIMemory& Memory)
	{
		return Memory.MemoryStrength <= 0.f;
	});
}

void UAIMemoryComponent::AddMemory(const FString& MemoryId, const TMap<FName, FString>& Context, float EmotionalImpact, EMemoryType MemoryType, float InitialStrength)
{
	FAIMemory* ExistingMemory = Memories.FindByPredicate([&](const FAIMemory& Memory)
	{
		return Memory.MemoryId == MemoryId;
	});

	if (ExistingMemory)
	{
		ExistingMemory->Context = Context;
		ExistingMemory->EmotionalImpact = EmotionalImpact;
		ExistingMemory->MemoryType = MemoryType;
		ExistingMemory->MemoryStrength = FMath::Max(ExistingMemory->MemoryStrength, InitialStrength);
		ExistingMemory->Timestamp = FDateTime::UtcNow();
	}
	else
	{
		FAIMemory NewMemory;
		NewMemory.MemoryId = MemoryId;
		NewMemory.Context = Context;
		NewMemory.EmotionalImpact = EmotionalImpact;
		NewMemory.MemoryType = MemoryType;
		NewMemory.MemoryStrength = InitialStrength;
		NewMemory.Timestamp = FDateTime::UtcNow();
		Memories.Add(NewMemory);
	}
}

void UAIMemoryComponent::UpdateMemoryStrength(const FString& MemoryId, float StrengthChange)
{
	   FAIMemory* MemoryToUpdate = Memories.FindByPredicate([&](const FAIMemory& Memory)
	   {
		   return Memory.MemoryId == MemoryId;
	   });

	   if (MemoryToUpdate)
	   {
		   MemoryToUpdate->MemoryStrength += StrengthChange;
	   }
}

bool UAIMemoryComponent::GetMemory(const FString& MemoryId, FAIMemory& OutMemory) const
{
	const FAIMemory* FoundMemory = Memories.FindByPredicate([&](const FAIMemory& Memory)
	{
		return Memory.MemoryId == MemoryId;
	});

	if (FoundMemory)
	{
		OutMemory = *FoundMemory;
		return true;
	}

	return false;
}

void UAIMemoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromEventBus();
	Super::EndPlay(EndPlayReason);
}

const TArray<FAIMemory>& UAIMemoryComponent::GetMemories() const
{
	return Memories;
}

TArray<EGlobalEventType> UAIMemoryComponent::GetListenedEventTypes() const
{
	return {
		EGlobalEventType::FactionReputationChanged,
		EGlobalEventType::FactionWarDeclared,
		EGlobalEventType::FactionAllianceFormed,
		EGlobalEventType::FactionPeaceNegotiated,
		EGlobalEventType::PlayerCommittedCrime,
		EGlobalEventType::TradeCompleted
	};
}

float UAIMemoryComponent::GetFactionOpinionModifier(FName FactionID) const
{
	float OpinionModifier = 0.0f;
	
	// Get all faction-related memories
	TArray<FAIMemory> FactionMemories = GetFactionMemories(FactionID);
	
	// Calculate opinion modifier based on memories
	float TotalImpact = 0.0f;
	float TotalWeight = 0.0f;
	
	for (const FAIMemory& Memory : FactionMemories)
	{
		// Weight more recent and stronger memories more heavily
		float Weight = Memory.MemoryStrength;
		TotalImpact += Memory.EmotionalImpact * Weight;
		TotalWeight += Weight;
	}
	
	if (TotalWeight > 0.0f)
	{
		OpinionModifier = TotalImpact / TotalWeight;
	}
	
	// Clamp to reasonable range
	return FMath::Clamp(OpinionModifier, -1.0f, 1.0f);
}

float UAIMemoryComponent::GetFactionReputation(FName FactionID) const
{
	// Always query the authoritative source for faction reputation
	if (FactionManager)
	{
		return FactionManager->GetPlayerReputation(FactionID);
	}
	
	return 0.0f; // Default neutral reputation
}

float UAIMemoryComponent::GetEffectiveFactionAttitude(FName FactionID) const
{
	// Combine official reputation with personal memories
	float BaseReputation = GetFactionReputation(FactionID);
	float PersonalModifier = GetFactionOpinionModifier(FactionID);
	
	// Personal memories can modify the base reputation by up to 50%
	float EffectiveAttitude = BaseReputation + (PersonalModifier * 50.0f);
	
	// Clamp to valid reputation range
	return FMath::Clamp(EffectiveAttitude, -100.0f, 100.0f);
}

void UAIMemoryComponent::AddFactionMemory(FName FactionID, const FString& EventDescription, float EmotionalImpact, float MemoryStrength)
{
	FString MemoryID = GenerateFactionMemoryID(FactionID, EventDescription);
	
	TMap<FName, FString> Context;
	Context.Add(TEXT("FactionID"), FactionID.ToString());
	Context.Add(TEXT("EventType"), TEXT("FactionInteraction"));
	Context.Add(TEXT("Description"), EventDescription);
	
	AddMemory(MemoryID, Context, EmotionalImpact, EMemoryType::Faction, MemoryStrength);
	
	UE_LOG(LogTemp, Log, TEXT("AIMemoryComponent: Added faction memory for %s - %s (Impact: %.2f)"),
		*FactionID.ToString(), *EventDescription, EmotionalImpact);
}

TArray<FAIMemory> UAIMemoryComponent::GetFactionMemories(FName FactionID) const
{
	TArray<FAIMemory> FactionMemories;
	
	for (const FAIMemory& Memory : Memories)
	{
		if (Memory.MemoryType == EMemoryType::Faction)
		{
			if (Memory.Context.Contains(TEXT("FactionID")) &&
				Memory.Context[TEXT("FactionID")] == FactionID.ToString())
			{
				FactionMemories.Add(Memory);
			}
		}
	}
	
	// Sort by timestamp (most recent first)
	FactionMemories.Sort([](const FAIMemory& A, const FAIMemory& B)
	{
		return A.Timestamp > B.Timestamp;
	});
	
	return FactionMemories;
}

bool UAIMemoryComponent::RemembersPlayerAction(FName PlayerID, const FString& ActionType, float MaxAge) const
{
	FDateTime CurrentTime = FDateTime::UtcNow();
	
	for (const FAIMemory& Memory : Memories)
	{
		// Check if this memory involves the specified player
		if (Memory.Context.Contains(TEXT("PlayerID")) &&
			Memory.Context[TEXT("PlayerID")] == PlayerID.ToString())
		{
			// Check if it's the right type of action
			if (Memory.Context.Contains(TEXT("ActionType")) &&
				Memory.Context[TEXT("ActionType")] == ActionType)
			{
				// Check age if specified
				if (MaxAge > 0.0f)
				{
					float MemoryAge = (CurrentTime - Memory.Timestamp).GetTotalSeconds();
					if (MemoryAge > MaxAge)
					{
						continue; // Too old
					}
				}
				
				return true;
			}
		}
	}
	
	return false;
}


void UAIMemoryComponent::RegisterWithEventBus()
{
	if (GlobalEventBus)
	{
		TScriptInterface<IGlobalEventListener> ListenerInterface;
		ListenerInterface.SetObject(this);
		ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
		TArray<EGlobalEventType> ListenedEvents = {
			EGlobalEventType::NPCMemoryAdded,
			EGlobalEventType::PlayerCommittedCrime,
			EGlobalEventType::CombatStarted,
			EGlobalEventType::CombatEnded,
			EGlobalEventType::NPCDied,
			EGlobalEventType::PlayerEnteredRegion,
			EGlobalEventType::PlayerLeftRegion
		};
		GlobalEventBus->RegisterListener(ListenerInterface, ListenedEvents);
	}
}

void UAIMemoryComponent::UnregisterFromEventBus()
{
	if (GlobalEventBus)
	{
		TScriptInterface<IGlobalEventListener> ListenerInterface;
		ListenerInterface.SetObject(this);
		ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
		GlobalEventBus->UnregisterListener(ListenerInterface);
	}
}

FString UAIMemoryComponent::GenerateFactionMemoryID(FName FactionID, const FString& EventType) const
{
	return FString::Printf(TEXT("Faction_%s_%s_%d"),
		*FactionID.ToString(),
		*EventType,
		FMath::RandRange(1000, 9999));
}