// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WorldPersistenceSystem.h"
#include "WorldInteractionComponent.generated.h"

/**
 * Component that allows actors to interact with the world persistence system
 * This component can be attached to player characters to easily record actions
 * that affect the world state.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UWorldInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    // Sets default values for this component's properties
    UWorldInteractionComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Record a combat action (attacking, defending, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordCombatAction(FName TargetID, float Intensity, const FString& Details = "");
    
    // Record a trade action (buying, selling, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordTradeAction(FName TargetID, float Value, const FString& Details = "");
    
    // Record a crafting action (creating items, building, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordCraftingAction(FName ItemID, float Quality, const FString& Details = "");
    
    // Record a conversation action (talking to NPCs, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordConversationAction(FName NPCID, float Significance, const FString& Details = "");
    
    // Record an exploration action (discovering locations, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordExplorationAction(FName LocationID, float Significance, const FString& Details = "");
    
    // Record a crime action (stealing, trespassing, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordCrimeAction(FName TargetID, float Severity, const FString& Details = "");
    
    // Record a faction action (helping a faction, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordFactionAction(FName FactionID, float Impact, const FString& Details = "");
    
    // Record an environmental action (planting, harvesting, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordEnvironmentalAction(FName EnvironmentID, float Impact, const FString& Details = "");
    
    // Record a resource action (gathering, mining, etc.)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void RecordResourceAction(FName ResourceID, float Amount, const FString& Details = "");
    
    // Get the current region the actor is in
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    FName GetCurrentRegion() const;
    
    // Set the current region manually (useful for teleportation or level transitions)
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    void SetCurrentRegion(FName RegionID);
    
    // Get the state of the current region
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    FRegionState GetCurrentRegionState() const;
    
    // Get recent events in the current region
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    TArray<FString> GetRecentRegionalEvents(int32 Count = 5) const;
    
    // Get player's impact level in the current region
    UFUNCTION(BlueprintCallable, Category = "World Interaction")
    float GetPlayerRegionalImpact() const;
    
    // Delegate for when the region changes
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegionChanged, FName, OldRegion, FName, NewRegion);
    UPROPERTY(BlueprintAssignable, Category = "World Interaction")
    FOnRegionChanged OnRegionChanged;

protected:
    // The current region the actor is in
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Interaction")
    FName CurrentRegion;
    
    // Reference to the world persistence system
    UPROPERTY()
    UWorldPersistenceSystem* WorldPersistenceSystem;
    
    // Helper function to record an action
    void RecordAction(EWorldActionType ActionType, FName TargetID, float Magnitude, const FString& Details);
    
    // Helper function to get the world persistence system
    UWorldPersistenceSystem* GetWorldPersistenceSystem() const;
};