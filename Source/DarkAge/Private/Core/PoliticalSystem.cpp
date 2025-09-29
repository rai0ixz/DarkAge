#include "Core/PoliticalSystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/TimeSystem.h"
#include "Data/SocialData.h"
#include "Kismet/GameplayStatics.h"

UPoliticalSystem::UPoliticalSystem()
    : FactionManager(nullptr)
{
}

void UPoliticalSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Get references to other subsystems
    if (UWorld* World = GetWorld())
    {
    	FactionManager = World->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
    }
    
    UE_LOG(LogTemp, Display, TEXT("Political System initialized"));
}

void UPoliticalSystem::Deinitialize()
{
    Super::Deinitialize();
}

void UPoliticalSystem::CalculateFactionDiplomaticInfluence(FName FactionName)
{
    // Implementation to be added
}

FGuid UPoliticalSystem::CreateTreaty(ETreatyType TreatyType, const TArray<FName>& InvolvedFactions, const FString& TreatyTerms, int32 DurationDays)
{
    FPoliticalTreaty NewTreaty;
    NewTreaty.TreatyID = FGuid::NewGuid();
    NewTreaty.TreatyType = TreatyType;
    NewTreaty.InvolvedFactions = InvolvedFactions;
    NewTreaty.TreatyTerms = TreatyTerms;
    NewTreaty.EstablishedDate = FDateTime::UtcNow();
    NewTreaty.ExpirationDate = FDateTime::UtcNow() + FTimespan::FromDays(DurationDays);
    ActiveTreaties.Add(NewTreaty);
    OnTreatyChanged.Broadcast(NewTreaty);
    return NewTreaty.TreatyID;
}

FGuid UPoliticalSystem::CreatePoliticalEvent(EPoliticalEventType EventType, FName Faction1ID, FName Faction2ID, FName RegionID, float Intensity, int32 Duration)
{
    FPoliticalEvent NewEvent;
    NewEvent.EventID = FGuid::NewGuid();
    NewEvent.EventType = EventType;
    NewEvent.PrimaryFactionID = Faction1ID;
    NewEvent.SecondaryFactionID = Faction2ID;
    NewEvent.RegionID = RegionID;
    NewEvent.Intensity = Intensity;
    ActivePoliticalEvents.Add(NewEvent);
    OnPoliticalEventOccurred.Broadcast(NewEvent);
    return NewEvent.EventID;
}

float UPoliticalSystem::CalculateRegionalPoliticalStability(FName RegionID)
{
    // Implementation to be added
    return 0.5f;
}

bool UPoliticalSystem::NegotiatePeace(FName Faction1ID, FName Faction2ID, const FString& PeaceTerms)
{
    // Implementation to be added
    return false;
}

TArray<FPoliticalEvent> UPoliticalSystem::GetActivePoliticalEvents()
{
    // Implementation to be added
    return TArray<FPoliticalEvent>();
}

bool UPoliticalSystem::ResolvePoliticalEvent(FGuid EventID, bool bSuccess, const FString& Outcome)
{
    // Implementation to be added
    return false;
}

TArray<FPoliticalTreaty> UPoliticalSystem::GetAllActiveTreaties()
{
    // Implementation to be added
    return TArray<FPoliticalTreaty>();
}

TArray<FPoliticalTreaty> UPoliticalSystem::GetFactionTreaties(FName FactionID)
{
    // Implementation to be added
    return TArray<FPoliticalTreaty>();
}
