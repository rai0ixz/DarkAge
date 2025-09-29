#include "Core/DynamicEventSubsystem.h"
#include "Core/WorldManagementSubsystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PlayerSkillsComponent.h"

UDynamicEventSubsystem::UDynamicEventSubsystem()
	: WorldManagementSubsystem(nullptr)
	, EventCheckInterval(30.0f)
	, TimeSinceLastCheck(0.0f)
{
}

void UDynamicEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldManagementSubsystem = Collection.InitializeDependency<UWorldManagementSubsystem>();
	InitializeEventTemplates();
}

void UDynamicEventSubsystem::Deinitialize()
{
	ActiveEvents.Empty();
	EventTemplates.Empty();
	Super::Deinitialize();
}

void UDynamicEventSubsystem::Tick(float DeltaTime)
{
	TimeSinceLastCheck += DeltaTime;
	if (TimeSinceLastCheck >= EventCheckInterval)
	{
		CheckForEvents();
		TimeSinceLastCheck = 0.0f;
	}
}

FDynamicEvent UDynamicEventSubsystem::GenerateRandomEvent(const FString& RegionID)
{
	FDynamicEvent NewEvent;
	NewEvent.EventID = FGuid::NewGuid().ToString();
	NewEvent.EventName = TEXT("Random Event");
	NewEvent.Description = TEXT("Auto-generated dynamic event");
	NewEvent.EventType = static_cast<EDynamicEventType>(FMath::RandHelper(static_cast<int32>(EDynamicEventType::Plague) + 1));
	NewEvent.State = EDynamicEventState::Building;
	NewEvent.RegionID = RegionID;
	NewEvent.Intensity = FMath::FRandRange(0.5f, 2.0f);
	NewEvent.Duration = FMath::FRandRange(30.0f, 120.0f);
	NewEvent.TimeRemaining = NewEvent.Duration;
	return NewEvent;
}

void UDynamicEventSubsystem::TriggerEvent(const FDynamicEvent& Event)
{
	ActiveEvents.Add(Event.EventID, Event);
	ApplyEventEffects(Event);
	CheckEventTriggers(Event);
}

void UDynamicEventSubsystem::UpdateEvent(const FString& EventID, float DeltaTime)
{
	if (FDynamicEvent* Event = ActiveEvents.Find(EventID))
	{
		Event->State = EDynamicEventState::Active;
		Event->TimeRemaining = FMath::Max(0.0f, Event->TimeRemaining - DeltaTime);

		UpdateEventEffects(*Event, DeltaTime);

		if (Event->TimeRemaining <= 0.0f)
		{
			ResolveEvent(EventID);
		}
	}
}

void UDynamicEventSubsystem::ResolveEvent(const FString& EventID)
{
	if (FDynamicEvent* Event = ActiveEvents.Find(EventID))
	{
		Event->State = EDynamicEventState::Resolved;
		RemoveEventEffects(*Event);
		ActiveEvents.Remove(EventID);
	}
}

TArray<FDynamicEvent> UDynamicEventSubsystem::GetActiveEvents() const
{
	TArray<FDynamicEvent> OutEvents;
	OutEvents.Reserve(ActiveEvents.Num());
	for (const TPair<FString, FDynamicEvent>& Pair : ActiveEvents)
	{
		OutEvents.Add(Pair.Value);
	}
	return OutEvents;
}

TArray<FDynamicEvent> UDynamicEventSubsystem::GetEventsInRegion(const FString& RegionID) const
{
	TArray<FDynamicEvent> OutEvents;
	for (const TPair<FString, FDynamicEvent>& Pair : ActiveEvents)
	{
		if (Pair.Value.RegionID == RegionID)
		{
			OutEvents.Add(Pair.Value);
		}
	}
	return OutEvents;
}

void UDynamicEventSubsystem::ApplyEventEffects(const FDynamicEvent& Event)
{
	// Placeholder for integrating with other systems (economy, factions, etc.)
	UE_LOG(LogTemp, Verbose, TEXT("ApplyEventEffects: %s"), *Event.EventName);
}

void UDynamicEventSubsystem::RemoveEventEffects(const FDynamicEvent& Event)
{
	// Placeholder for reversing effects
	UE_LOG(LogTemp, Verbose, TEXT("RemoveEventEffects: %s"), *Event.EventName);
}

void UDynamicEventSubsystem::InitializeEventTemplates()
{
	EventTemplates.Empty();

	if (DynamicEventDataTable)
	{
		TArray<FName> RowNames = DynamicEventDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			if (const FDynamicEventData* Data = DynamicEventDataTable->FindRow<FDynamicEventData>(RowName, TEXT("InitializeEventTemplates")))
			{
				FDynamicEvent Template;
				Template.EventID = RowName.ToString();
				Template.EventName = Data->EventName.ToString();
				Template.Description = TEXT("Template from data table");
				Template.EventType = Data->EventType;
				Template.State = EDynamicEventState::Dormant;
				Template.Intensity = 1.0f;
				Template.Duration = 60.0f;
				Template.TimeRemaining = Template.Duration;
				EventTemplates.Add(Template);
			}
		}
	}
}

void UDynamicEventSubsystem::CheckForEvents()
{
	if (!DynamicEventDataTable)
	{
		return;
	}

	TArray<FName> RowNames = DynamicEventDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		if (FDynamicEventData* EventData = DynamicEventDataTable->FindRow<FDynamicEventData>(RowName, TEXT("")))
		{
			if (AreEventConditionsMet(*EventData))
			{
				if (FMath::FRand() < EventData->BaseChanceToOccur)
				{
					OnDynamicEventTriggered.Broadcast(RowName);
					UE_LOG(LogTemp, Log, TEXT("Dynamic Event Triggered: %s"), *EventData->EventName.ToString());
				}
			}
		}
	}
}

bool UDynamicEventSubsystem::AreEventConditionsMet(const FDynamicEventData& EventData) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	if (!PlayerCharacter)
	{
		return false;
	}

	for (const FDynamicEventCondition& Condition : EventData.Conditions)
	{
		switch (Condition.ConditionType)
		{
		case EDynamicEventConditionType::TimeOfDay:
			if (WorldManagementSubsystem)
			{
				float CurrentTime = WorldManagementSubsystem->GetCurrentWorldTime();
				if (CurrentTime < Condition.MinValue || CurrentTime > Condition.MaxValue)
				{
					return false;
				}
			}
			break;

		case EDynamicEventConditionType::PlayerLevel:
			if (UPlayerSkillsComponent* SkillsComponent = PlayerCharacter->FindComponentByClass<UPlayerSkillsComponent>())
			{
				int32 PlayerLevel = SkillsComponent->GetCharacterLevel();
				if (PlayerLevel < Condition.MinValue || PlayerLevel > Condition.MaxValue)
				{
					return false;
				}
			}
			break;

		case EDynamicEventConditionType::FactionReputation:
			if (UFactionManagerSubsystem* FactionSubsystem = GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>())
			{
				float Reputation = FactionSubsystem->GetPlayerReputation(Condition.TargetID);
				if (Reputation < Condition.MinValue || Reputation > Condition.MaxValue)
				{
					return false;
				}
			}
			break;

		case EDynamicEventConditionType::WorldState:
			if (WorldManagementSubsystem)
			{
				FName CurrentState = WorldManagementSubsystem->GetWorldState(Condition.TargetID);
				if (CurrentState != Condition.TargetID)
				{
					return false;
				}
			}
			break;

		default:
			break;
		}
	}

	return true;
}

void UDynamicEventSubsystem::UpdateEventEffects(FDynamicEvent& Event, float DeltaTime)
{
	// Simple placeholder that decays intensity over time
	Event.Intensity = FMath::Max(0.0f, Event.Intensity - DeltaTime * 0.01f);
}

void UDynamicEventSubsystem::CheckEventTriggers(const FDynamicEvent& Event)
{
	// Placeholder for future notifications or quest hooks
	UE_LOG(LogTemp, Verbose, TEXT("Event Triggered: %s"), *Event.EventName);
}