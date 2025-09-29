// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/WorldPersistenceTestComponent.h"
#include "Core/WorldPersistenceSystem.h"
#include "Core/DAGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWorldPersistenceTestComponent::UWorldPersistenceTestComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize available regions
    AvailableRegions.Add(FName("Heartlands"));
    AvailableRegions.Add(FName("NorthernWastes"));
    AvailableRegions.Add(FName("EasternMarshlands"));
    AvailableRegions.Add(FName("SouthernDeserts"));
    AvailableRegions.Add(FName("WesternCoastal"));
}

// Called when the game starts
void UWorldPersistenceTestComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the world persistence system
    WorldPersistenceSystem = GetWorldPersistenceSystem();
    
    if (!WorldPersistenceSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::BeginPlay: Failed to get WorldPersistenceSystem."));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UWorldPersistenceTestComponent::BeginPlay: WorldPersistenceSystem found."));
        
        // Print initial state of all regions
        for (const FName& RegionID : AvailableRegions)
        {
            PrintRegionState(RegionID);
        }
    }
}

// Called every frame
void UWorldPersistenceTestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // No need for tick functionality in this test component
}

void UWorldPersistenceTestComponent::GenerateRandomAction()
{
    if (!WorldPersistenceSystem)
    {
        WorldPersistenceSystem = GetWorldPersistenceSystem();
        
        if (!WorldPersistenceSystem)
        {
            UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::GenerateRandomAction: Failed to get WorldPersistenceSystem."));
            return;
        }
    }
    
    // Generate random action type
    EWorldActionType ActionType = static_cast<EWorldActionType>(FMath::RandRange(0, static_cast<int32>(EWorldActionType::Resource)));
    
    // Generate random region
    FName RegionID = AvailableRegions[FMath::RandRange(0, AvailableRegions.Num() - 1)];
    
    // Generate random location
    FVector Location = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), 0.0f);
    
    // Generate random magnitude
    float Magnitude = FMath::RandRange(0.1f, 1.0f);
    
    // Generate random target ID
    FName TargetID;
    switch (ActionType)
    {
        case EWorldActionType::Combat:
            TargetID = FName(*FString::Printf(TEXT("NPC_%d"), FMath::RandRange(1, 100)));
            break;
        case EWorldActionType::Trade:
            TargetID = FName(*FString::Printf(TEXT("Merchant_%d"), FMath::RandRange(1, 20)));
            break;
        case EWorldActionType::Crafting:
            TargetID = FName(*FString::Printf(TEXT("Item_%d"), FMath::RandRange(1, 50)));
            break;
        case EWorldActionType::Conversation:
            TargetID = FName(*FString::Printf(TEXT("NPC_%d"), FMath::RandRange(1, 100)));
            break;
        case EWorldActionType::Exploration:
            TargetID = FName(*FString::Printf(TEXT("Location_%d"), FMath::RandRange(1, 30)));
            break;
        case EWorldActionType::Crime:
            TargetID = FName(*FString::Printf(TEXT("Victim_%d"), FMath::RandRange(1, 50)));
            break;
        case EWorldActionType::Faction:
            {
                static const TArray<FName> Factions = { FName("IronBrotherhood"), FName("MerchantCoalition"), FName("CircleOfDruids"), FName("ScholarsOfTheSundering") };
                TargetID = Factions[FMath::RandRange(0, Factions.Num() - 1)];
            }
            break;
        case EWorldActionType::Environment:
            TargetID = FName(*FString::Printf(TEXT("Environment_%d"), FMath::RandRange(1, 10)));
            break;
        case EWorldActionType::Resource:
            {
                static const TArray<FName> Resources = { FName("Wood"), FName("Iron"), FName("Food"), FName("Stone") };
                TargetID = Resources[FMath::RandRange(0, Resources.Num() - 1)];
            }
            break;
        default:
            TargetID = FName("Unknown");
            break;
    }
    
    // Generate random additional data
    FString AdditionalData = FString::Printf(TEXT("Random test action %d"), FMath::RandRange(1, 1000));
    
    // Record the action
    WorldPersistenceSystem->RecordPlayerAction(ActionType, Location, RegionID, Magnitude, TargetID, AdditionalData);
    
    UE_LOG(LogTemp, Log, TEXT("UWorldPersistenceTestComponent::GenerateRandomAction: Generated %s action in %s with magnitude %.2f, target %s"), 
        *UEnum::GetValueAsString(ActionType), *RegionID.ToString(), Magnitude, *TargetID.ToString());
    
    // Print updated region state
    PrintRegionState(RegionID);
}

void UWorldPersistenceTestComponent::SimulateWorldEvolution(int32 GameMinutes)
{
    if (!WorldPersistenceSystem)
    {
        WorldPersistenceSystem = GetWorldPersistenceSystem();
        
        if (!WorldPersistenceSystem)
        {
            UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::SimulateWorldEvolution: Failed to get WorldPersistenceSystem."));
            return;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("UWorldPersistenceTestComponent::SimulateWorldEvolution: Simulating %d game minutes..."), GameMinutes);
    
    // Print state before simulation
    UE_LOG(LogTemp, Log, TEXT("--- Region States Before Simulation ---"));
    for (const FName& RegionID : AvailableRegions)
    {
        PrintRegionState(RegionID);
    }
    
    // Simulate world evolution
    WorldPersistenceSystem->SimulateWorldEvolution(GameMinutes);
    
    // Print state after simulation
    UE_LOG(LogTemp, Log, TEXT("--- Region States After Simulation ---"));
    for (const FName& RegionID : AvailableRegions)
    {
        PrintRegionState(RegionID);
    }
}

FRegionState UWorldPersistenceTestComponent::GetRegionState(FName RegionID) const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetRegionState(RegionID);
    }
    
    return FRegionState();
}

TArray<FString> UWorldPersistenceTestComponent::GetRecentRegionalEvents(FName RegionID, int32 Count) const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetRecentRegionalEvents(RegionID, Count);
    }
    
    return TArray<FString>();
}

float UWorldPersistenceTestComponent::GetPlayerRegionalImpact(FName RegionID) const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetPlayerRegionalImpact(RegionID);
    }
    
    return 0.0f;
}

bool UWorldPersistenceTestComponent::SaveWorldState()
{
    if (!WorldPersistenceSystem)
    {
        WorldPersistenceSystem = GetWorldPersistenceSystem();
        
        if (!WorldPersistenceSystem)
        {
            UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::SaveWorldState: Failed to get WorldPersistenceSystem."));
            return false;
        }
    }
    
    bool bSuccess = WorldPersistenceSystem->SaveWorldState();
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("UWorldPersistenceTestComponent::SaveWorldState: World state saved successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::SaveWorldState: Failed to save world state."));
    }
    
    return bSuccess;
}

bool UWorldPersistenceTestComponent::LoadWorldState()
{
    if (!WorldPersistenceSystem)
    {
        WorldPersistenceSystem = GetWorldPersistenceSystem();
        
        if (!WorldPersistenceSystem)
        {
            UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::LoadWorldState: Failed to get WorldPersistenceSystem."));
            return false;
        }
    }
    
    bool bSuccess = WorldPersistenceSystem->LoadWorldState();
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("UWorldPersistenceTestComponent::LoadWorldState: World state loaded successfully."));
        
        // Print state after loading
        UE_LOG(LogTemp, Log, TEXT("--- Region States After Loading ---"));
        for (const FName& RegionID : AvailableRegions)
        {
            PrintRegionState(RegionID);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::LoadWorldState: Failed to load world state."));
    }
    
    return bSuccess;
}

TArray<FName> UWorldPersistenceTestComponent::GetAllRegions() const
{
    return AvailableRegions;
}

void UWorldPersistenceTestComponent::PrintRegionState(FName RegionID) const
{
    if (!WorldPersistenceSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UWorldPersistenceTestComponent::PrintRegionState: WorldPersistenceSystem is NULL."));
        return;
    }
    
    FRegionState RegionState = WorldPersistenceSystem->GetRegionState(RegionID);
    
    UE_LOG(LogTemp, Log, TEXT("=== Region: %s ==="), *RegionID.ToString());
    UE_LOG(LogTemp, Log, TEXT("Discovered: %s"), RegionState.bIsDiscovered ? TEXT("True") : TEXT("False"));
    UE_LOG(LogTemp, Log, TEXT("Player Reputation: %.2f"), RegionState.PlayerReputationLevel);
    UE_LOG(LogTemp, Log, TEXT("Current Weather: %s"), *RegionState.CurrentWeather);
    UE_LOG(LogTemp, Log, TEXT("Last Visit Time: %.2f"), RegionState.LastVisitTime);
    UE_LOG(LogTemp, Log, TEXT("Active Bounty: %.2f"), RegionState.ActiveBountyValue);
    UE_LOG(LogTemp, Log, TEXT("Crime Heat: %.2f"), RegionState.CrimeHeatLevel);
    UE_LOG(LogTemp, Log, TEXT("Supply Chain Disruption: %.2f"), RegionState.SupplyChainDisruption);

    UE_LOG(LogTemp, Log, TEXT("Current Events:"));
    TArray<FString> RecentEvents = WorldPersistenceSystem->GetRecentRegionalEvents(RegionID, 5);
    for (const FString& Event : RecentEvents)
    {
        UE_LOG(LogTemp, Log, TEXT("  %s"), *Event);
    }
    
    float PlayerImpact = WorldPersistenceSystem->GetPlayerRegionalImpact(RegionID);
    UE_LOG(LogTemp, Log, TEXT("Player Impact: %.2f"), PlayerImpact);
}

UWorldPersistenceSystem* UWorldPersistenceTestComponent::GetWorldPersistenceSystem() const
{
    if (UWorld* World = GetWorld())
    {
        UDAGameInstance* GameInstance = Cast<UDAGameInstance>(UGameplayStatics::GetGameInstance(World));
        if (GameInstance)
        {
            return GameInstance->GetSubsystem<UWorldPersistenceSystem>();
        }
    }
    
    return nullptr;
}