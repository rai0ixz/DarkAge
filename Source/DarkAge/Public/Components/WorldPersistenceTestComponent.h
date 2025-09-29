// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WorldPersistenceSystem.h"
#include "WorldPersistenceTestComponent.generated.h"

/**
 * Test component for demonstrating the world persistence system
 * This component can be attached to any actor to test the world persistence system
 * and visualize its effects.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UWorldPersistenceTestComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    // Sets default values for this component's properties
    UWorldPersistenceTestComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Generate a random player action for testing
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    void GenerateRandomAction();
    
    // Simulate world evolution for a given amount of game time
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    void SimulateWorldEvolution(int32 GameMinutes);
    
    // Get the state of a specific region
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    FRegionState GetRegionState(FName RegionID) const;
    
    // Get recent events in a region
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    TArray<FString> GetRecentRegionalEvents(FName RegionID, int32 Count = 5) const;
    
    // Get player impact level in a region
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    float GetPlayerRegionalImpact(FName RegionID) const;
    
    // Save world state to disk
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    bool SaveWorldState();
    
    // Load world state from disk
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    bool LoadWorldState();
    
    // Get all available regions
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    TArray<FName> GetAllRegions() const;
    
    // Print region state to log
    UFUNCTION(BlueprintCallable, Category = "World Persistence Test")
    void PrintRegionState(FName RegionID) const;

protected:
    // Reference to the world persistence system
    UPROPERTY()
    UWorldPersistenceSystem* WorldPersistenceSystem;
    
    // Available regions for testing
    UPROPERTY()
    TArray<FName> AvailableRegions;
    
    // Helper function to get the world persistence system
    UWorldPersistenceSystem* GetWorldPersistenceSystem() const;
};