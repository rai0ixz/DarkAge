#include "Components/FactionReputationComponent.h"
#include "Core/FactionManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

UFactionReputationComponent::UFactionReputationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFactionReputationComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize reputation with all factions from the faction manager
    if(UWorld* World = GetWorld())
    {
        UFactionManagerSubsystem* FactionManager = World->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
        if (FactionManager)
        {
            TArray<FName> AllFactions = FactionManager->GetAllFactions();
            for (const FName& FactionID : AllFactions)
            {
                if (!FactionReputations.Contains(FactionID))
                {
                    FFactionReputation NewReputation(FactionID);
                    FactionReputations.Add(FactionID, NewReputation);
                }
            }
        }
    }
}

void UFactionReputationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UFactionReputationComponent::GetFactionReputation(FName FactionID) const
{
    if (FactionReputations.Contains(FactionID))
    {
        return FactionReputations[FactionID].ReputationValue;
    }

    return 0.0f;
}

int32 UFactionReputationComponent::GetFactionRank(FName FactionID) const
{
    if (FactionReputations.Contains(FactionID))
    {
        return FactionReputations[FactionID].FactionRank;
    }

    return 0;
}

bool UFactionReputationComponent::ModifyFactionReputation(FName FactionID, float Delta)
{
    // Check if the faction exists in the faction manager
    if (UWorld* World = GetWorld())
    {
        UFactionManagerSubsystem* FactionManager = World->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
        if (!FactionManager || !FactionManager->GetAllFactions().Contains(FactionID))
        {
            UE_LOG(LogTemp, Warning, TEXT("FactionReputationComponent: Attempted to modify reputation with non-existent faction"));
            return false;
        }
    } else {
  return false;
 }

    // Get or create the reputation data
    FFactionReputation& ReputationData = FactionReputations.FindOrAdd(FactionID);
    ReputationData.FactionID = FactionID;

    // Store the old values
    float OldReputation = ReputationData.ReputationValue;
    int32 OldRank = ReputationData.FactionRank;

    // Update the reputation value
    ReputationData.ReputationValue += Delta;
    
    // Clamp the reputation value
    ReputationData.ReputationValue = FMath::Clamp(ReputationData.ReputationValue, -100.0f, 100.0f);

    // Update the faction rank
    UpdateFactionRank(FactionID);

    // Broadcast the reputation change if it's different
    if (!FMath::IsNearlyEqual(OldReputation, ReputationData.ReputationValue))
    {
        OnFactionReputationChanged.Broadcast(FactionID, OldReputation, ReputationData.ReputationValue);
    }

    // Broadcast the rank change if it's different
    if (OldRank != ReputationData.FactionRank)
    {
        OnFactionRankChanged.Broadcast(FactionID, OldRank, ReputationData.FactionRank);
    }

    return true;
}

bool UFactionReputationComponent::SetFactionReputation(FName FactionID, float NewReputation)
{
    // Check if the faction exists in the faction manager
    if (UWorld* World = GetWorld())
    {
        UFactionManagerSubsystem* FactionManager = World->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
        if (!FactionManager || !FactionManager->GetAllFactions().Contains(FactionID))
        {
            UE_LOG(LogTemp, Warning, TEXT("FactionReputationComponent: Attempted to set reputation with non-existent faction"));
            return false;
        }
    } else {
  return false;
 }

    // Get or create the reputation data
    FFactionReputation& ReputationData = FactionReputations.FindOrAdd(FactionID);
    ReputationData.FactionID = FactionID;

    // Store the old values
    float OldReputation = ReputationData.ReputationValue;
    int32 OldRank = ReputationData.FactionRank;

    // Update the reputation value
    ReputationData.ReputationValue = FMath::Clamp(NewReputation, -100.0f, 100.0f);

    // Update the faction rank
    UpdateFactionRank(FactionID);

    // Broadcast the reputation change if it's different
    if (!FMath::IsNearlyEqual(OldReputation, ReputationData.ReputationValue))
    {
        OnFactionReputationChanged.Broadcast(FactionID, OldReputation, ReputationData.ReputationValue);
    }

    // Broadcast the rank change if it's different
    if (OldRank != ReputationData.FactionRank)
    {
        OnFactionRankChanged.Broadcast(FactionID, OldRank, ReputationData.FactionRank);
    }

    return true;
}

bool UFactionReputationComponent::SetFactionRank(FName FactionID, int32 NewRank)
{
    // Check if the faction exists in the faction manager
    if (UWorld* World = GetWorld())
    {
        UFactionManagerSubsystem* FactionManager = World->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
        if (!FactionManager || !FactionManager->GetAllFactions().Contains(FactionID))
        {
            UE_LOG(LogTemp, Warning, TEXT("FactionReputationComponent: Attempted to set rank with non-existent faction"));
            return false;
        }
    } else {
  return false;
 }

    // Get or create the reputation data
    FFactionReputation& ReputationData = FactionReputations.FindOrAdd(FactionID);
    ReputationData.FactionID = FactionID;

    // Store the old rank
    int32 OldRank = ReputationData.FactionRank;

    // Update the faction rank
    ReputationData.FactionRank = FMath::Clamp(NewRank, 0, 10);

    // Broadcast the rank change if it's different
    if (OldRank != ReputationData.FactionRank)
    {
        OnFactionRankChanged.Broadcast(FactionID, OldRank, ReputationData.FactionRank);
    }

    return true;
}

TArray<FName> UFactionReputationComponent::GetAllFactions() const
{
    TArray<FName> Factions;
    FactionReputations.GetKeys(Factions);
    return Factions;
}

bool UFactionReputationComponent::GetFactionReputationData(FName FactionID, FFactionReputation& OutReputation) const
{
    if (FactionReputations.Contains(FactionID))
    {
        OutReputation = FactionReputations[FactionID];
        return true;
    }

    return false;
}

bool UFactionReputationComponent::IsAlliedWithFaction(FName FactionID) const
{
    float Reputation = GetFactionReputation(FactionID);
    return Reputation >= 50.0f;
}

bool UFactionReputationComponent::IsHostileToFaction(FName FactionID) const
{
    float Reputation = GetFactionReputation(FactionID);
    return Reputation <= -50.0f;
}

void UFactionReputationComponent::UpdateFactionRank(FName FactionID)
{
    if (!FactionReputations.Contains(FactionID))
    {
        return;
    }

    FFactionReputation& ReputationData = FactionReputations[FactionID];
    float Reputation = ReputationData.ReputationValue;
    int32 OldRank = ReputationData.FactionRank;
    int32 NewRank = 0;

    // Calculate the new rank based on reputation
    if (Reputation >= 90.0f)
    {
        NewRank = 10; // Highest rank
    }
    else if (Reputation >= 80.0f)
    {
        NewRank = 9;
    }
    else if (Reputation >= 70.0f)
    {
        NewRank = 8;
    }
    else if (Reputation >= 60.0f)
    {
        NewRank = 7;
    }
    else if (Reputation >= 50.0f)
    {
        NewRank = 6;
    }
    else if (Reputation >= 40.0f)
    {
        NewRank = 5;
    }
    else if (Reputation >= 30.0f)
    {
        NewRank = 4;
    }
    else if (Reputation >= 20.0f)
    {
        NewRank = 3;
    }
    else if (Reputation >= 10.0f)
    {
        NewRank = 2;
    }
    else if (Reputation >= 0.0f)
    {
        NewRank = 1;
    }
    else
    {
        NewRank = 0; // Outsider or hostile
    }

    // Update the rank if it's different
    if (OldRank != NewRank)
    {
        ReputationData.FactionRank = NewRank;
        OnFactionRankChanged.Broadcast(FactionID, OldRank, NewRank);
    }
}

float UFactionReputationComponent::GetReputationThresholdForRank(int32 Rank) const
{
    switch (Rank)
    {
    case 10:
        return 90.0f;
    case 9:
        return 80.0f;
    case 8:
        return 70.0f;
    case 7:
        return 60.0f;
    case 6:
        return 50.0f;
    case 5:
        return 40.0f;
    case 4:
        return 30.0f;
    case 3:
        return 20.0f;
    case 2:
        return 10.0f;
    case 1:
        return 0.0f;
    default:
        return -100.0f; // Rank 0 or invalid
    }
}

bool UFactionReputationComponent::ModifyReputation(FName FactionID, float Delta)
{
    // ModifyReputation is a simple wrapper around ModifyFactionReputation for debug purposes
    UE_LOG(LogTemp, Log, TEXT("ModifyReputation: Modifying reputation with faction '%s' by %.2f"), *FactionID.ToString(), Delta);
    
    bool bSuccess = ModifyFactionReputation(FactionID, Delta);
    
    if (bSuccess)
    {
        float NewReputation = GetFactionReputation(FactionID);
        int32 NewRank = GetFactionRank(FactionID);
        UE_LOG(LogTemp, Log, TEXT("ModifyReputation: Successfully modified reputation with faction '%s'. New reputation: %.2f, Rank: %d"),
            *FactionID.ToString(), NewReputation, NewRank);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ModifyReputation: Failed to modify reputation with faction '%s'"), *FactionID.ToString());
    }
    
    return bSuccess;
}