// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/WorldInteractionComponent.h"
#include "Core/WorldPersistenceSystem.h"
#include "Core/DAGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWorldInteractionComponent::UWorldInteractionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default region
    CurrentRegion = FName("Heartlands");
}

// Called when the game starts
void UWorldInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the world persistence system
    WorldPersistenceSystem = GetWorldPersistenceSystem();
    
    if (!WorldPersistenceSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UWorldInteractionComponent::BeginPlay: Failed to get WorldPersistenceSystem."));
    }
}

// Called every frame
void UWorldInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // In a full implementation, this could check if the actor has moved to a new region
    // For now, we'll rely on manual region setting or level-specific regions
}

void UWorldInteractionComponent::RecordCombatAction(FName TargetID, float Intensity, const FString& Details)
{
    RecordAction(EWorldActionType::Combat, TargetID, Intensity, Details);
}

void UWorldInteractionComponent::RecordTradeAction(FName TargetID, float Value, const FString& Details)
{
    RecordAction(EWorldActionType::Trade, TargetID, Value, Details);
}

void UWorldInteractionComponent::RecordCraftingAction(FName ItemID, float Quality, const FString& Details)
{
    RecordAction(EWorldActionType::Crafting, ItemID, Quality, Details);
}

void UWorldInteractionComponent::RecordConversationAction(FName NPCID, float Significance, const FString& Details)
{
    RecordAction(EWorldActionType::Conversation, NPCID, Significance, Details);
}

void UWorldInteractionComponent::RecordExplorationAction(FName LocationID, float Significance, const FString& Details)
{
    RecordAction(EWorldActionType::Exploration, LocationID, Significance, Details);
}

void UWorldInteractionComponent::RecordCrimeAction(FName TargetID, float Severity, const FString& Details)
{
    RecordAction(EWorldActionType::Crime, TargetID, Severity, Details);
}

void UWorldInteractionComponent::RecordFactionAction(FName FactionID, float Impact, const FString& Details)
{
    RecordAction(EWorldActionType::Faction, FactionID, Impact, Details);
}

void UWorldInteractionComponent::RecordEnvironmentalAction(FName EnvironmentID, float Impact, const FString& Details)
{
    RecordAction(EWorldActionType::Environment, EnvironmentID, Impact, Details);
}

void UWorldInteractionComponent::RecordResourceAction(FName ResourceID, float Amount, const FString& Details)
{
    RecordAction(EWorldActionType::Resource, ResourceID, Amount, Details);
}

FName UWorldInteractionComponent::GetCurrentRegion() const
{
    return CurrentRegion;
}

void UWorldInteractionComponent::SetCurrentRegion(FName RegionID)
{
    if (CurrentRegion != RegionID)
    {
        FName OldRegion = CurrentRegion;
        CurrentRegion = RegionID;
        
        // Broadcast the region change
        OnRegionChanged.Broadcast(OldRegion, CurrentRegion);
        
        UE_LOG(LogTemp, Log, TEXT("UWorldInteractionComponent::SetCurrentRegion: Region changed from %s to %s"), 
            *OldRegion.ToString(), *CurrentRegion.ToString());
    }
}

FRegionState UWorldInteractionComponent::GetCurrentRegionState() const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetRegionState(CurrentRegion);
    }
    
    return FRegionState();
}

TArray<FString> UWorldInteractionComponent::GetRecentRegionalEvents(int32 Count) const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetRecentRegionalEvents(CurrentRegion, Count);
    }
    
    return TArray<FString>();
}

float UWorldInteractionComponent::GetPlayerRegionalImpact() const
{
    if (WorldPersistenceSystem)
    {
        return WorldPersistenceSystem->GetPlayerRegionalImpact(CurrentRegion);
    }
    
    return 0.0f;
}

void UWorldInteractionComponent::RecordAction(EWorldActionType ActionType, FName TargetID, float Magnitude, const FString& Details)
{
    if (!WorldPersistenceSystem)
    {
        WorldPersistenceSystem = GetWorldPersistenceSystem();
        
        if (!WorldPersistenceSystem)
        {
            UE_LOG(LogTemp, Error, TEXT("UWorldInteractionComponent::RecordAction: Failed to get WorldPersistenceSystem."));
            return;
        }
    }
    
    // Clamp magnitude to valid range
    float ClampedMagnitude = FMath::Clamp(Magnitude, 0.0f, 1.0f);
    
    // Get actor location
    FVector Location = GetOwner()->GetActorLocation();
    
    // Record the action
    WorldPersistenceSystem->RecordPlayerAction(ActionType, Location, CurrentRegion, ClampedMagnitude, TargetID, Details);
    
    UE_LOG(LogTemp, Log, TEXT("UWorldInteractionComponent::RecordAction: Recorded %s action in %s with magnitude %.2f"), 
        *UEnum::GetValueAsString(ActionType), *CurrentRegion.ToString(), ClampedMagnitude);
}

UWorldPersistenceSystem* UWorldInteractionComponent::GetWorldPersistenceSystem() const
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