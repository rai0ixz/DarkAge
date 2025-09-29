#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/WorldStateData.h"
#include "WorldManagementSubsystem.generated.h"

// Forward declarations
struct FRegionData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionDiscovered, const FString&, RegionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegionChanged, const FString&, OldRegionID, const FString&, NewRegionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeUpdated, int32, Day, float, Hour);

/**
 * World Management Subsystem
 *
 * Handles the open-world management as outlined in the Medieval Mayhem GDD.
 * Manages regions, world state persistence, time progression, and environmental factors.
 * 
 * Key Responsibilities:
 * - Region discovery and state tracking
 * - World time and weather simulation
 * - World state persistence and loading
 * - Environmental factor management
 * 
 * TODO: Implement full open-world streaming system
 * TODO: Add dynamic weather patterns based on regions
 * TODO: Implement day/night cycle impact on gameplay
 * TODO: Add seasonal changes affecting supply chains
 */
UCLASS()
class DARKAGE_API UWorldManagementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UWorldManagementSubsystem();
    
    // UGameInstanceSubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Region Management Functions
     */
    UFUNCTION(BlueprintCallable, Category = "World Management|Regions")
    void DiscoverRegion(const FString& RegionID);

    UFUNCTION(BlueprintCallable, Category = "World Management|Regions")
    bool IsRegionDiscovered(const FString& RegionID) const;

    UFUNCTION(BlueprintCallable, Category = "World Management|Regions")
    void SetCurrentRegion(const FString& RegionID);

    UFUNCTION(BlueprintPure, Category = "World Management|Regions")
    FString GetCurrentRegion() const { return CurrentRegionID; }

    UFUNCTION(BlueprintCallable, Category = "World Management|Regions")
    TArray<FString> GetDiscoveredRegions() const;

    UFUNCTION(BlueprintPure, Category = "World Management|Regions")
    TArray<FName> GetAllRegionIDs() const;

    UFUNCTION(BlueprintPure, Category = "World Management|Regions")
    FRegionData GetRegionData(const FString& RegionID) const;

    UFUNCTION(BlueprintPure, Category = "World Management|Regions")
    TMap<FName, FVector> GetPointsOfInterest(const FString& RegionID) const;

    UFUNCTION(BlueprintPure, Category = "World Management|Regions")
    float GetPoliticalStability(const FString& RegionID) const;

    UPROPERTY(BlueprintAssignable, Category = "World Management|Regions")
    FOnRegionDiscovered OnRegionDiscovered;

    UPROPERTY(BlueprintAssignable, Category = "World Management|Regions")
    FOnRegionChanged OnRegionChanged;

    UPROPERTY(BlueprintAssignable, Category = "World Management|Time")
    FOnTimeUpdated OnTimeUpdated;

    /**
     * World Time Management
     */
    UFUNCTION(BlueprintCallable, Category = "World Management|Time")
    void SetTimeScale(float NewTimeScale);

    UFUNCTION(BlueprintPure, Category = "World Management|Time")
    float GetCurrentWorldTime() const { return WorldTime; }

    UFUNCTION(BlueprintPure, Category = "World Management|Time")
    int32 GetCurrentDay() const;

    UFUNCTION(BlueprintPure, Category = "World Management|Time")
    bool IsNightTime() const;


    /**
     * World State Persistence
     */
    UFUNCTION(BlueprintCallable, Category = "World Management|Persistence")
    void SaveWorldState();

    UFUNCTION(BlueprintCallable, Category = "World Management|Persistence")
    void LoadWorldState();
   
      UFUNCTION(BlueprintPure, Category = "World Management|World State")
      FName GetWorldState(FName StateID) const;
   
   protected:
    // Data table for region definitions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World Data")
    UDataTable* RegionDataTable;

    // Current region tracking
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World State")
    FString CurrentRegionID;

    // Discovered regions
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World State")
    TSet<FString> DiscoveredRegions;

    // World time progression (in hours, starting at 0 = dawn of day 1)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World State")
    float WorldTime;

    // Time progression multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Settings")
    float TimeScale;

    // Region-specific weather states
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World State")
    TMap<FString, FString> RegionWeatherMap;
   
      UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World State")
      TMap<FName, FName> WorldStateMap;
   
    // Day/night cycle configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Settings")
    float DayLength; // Hours in a full day cycle

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Settings")
    float NightStartHour; // Hour when night begins

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Settings")
    float DayStartHour; // Hour when day begins

    /**
     * Enhanced Environmental System State
     */

    // Base temperatures for each region
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enhanced Environment")
    TMap<FString, float> RegionBaseTemperatures;

    // Registered environmental components for automatic updates
    UPROPERTY()
    TArray<TWeakObjectPtr<class UEnvironmentalFactorsComponent>> RegisteredEnvironmentalComponents;

    // Current season (for future seasonal system)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enhanced Environment")
    FString CurrentSeason;

    // Environmental update interval
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Environment")
    float EnvironmentalUpdateInterval;

    // Timer for environmental updates
    FTimerHandle EnvironmentalUpdateTimer;

    /**
     * Integration with World Ecosystem System
     */

    // Register ecosystem subsystem for time and region sync
    UFUNCTION(BlueprintCallable, Category = "World Management|Integration")
    void RegisterEcosystemSubsystem(class UWorldEcosystemSubsystem* EcosystemSubsystem);

    // Get world time in days since game start
    UFUNCTION(BlueprintPure, Category = "World Management|Time")
    float GetWorldTimeInDays() const;

    // Notify ecosystem of time progression events
    UFUNCTION(BlueprintCallable, Category = "World Management|Integration")
    void NotifyTimeAdvancement(float DeltaTimeInDays);

    // Sync regional data with ecosystem
    UFUNCTION(BlueprintCallable, Category = "World Management|Integration")
    void SyncRegionsWithEcosystem();

private:
    // Cached reference to ecosystem subsystem
    UPROPERTY()
    class UWorldEcosystemSubsystem* CachedEcosystemSubsystem;

    // Internal time update
    void UpdateWorldTime(float DeltaTime);

    // Update environmental conditions for all registered components
    void UpdateEnvironmentalConditions();

    void UpdateWeatherForRegion(const FString& RegionID);

    // Timer handle for world time updates
    FTimerHandle WorldTimeUpdateTimer;

    /**
     * TODO: Add these systems in future iterations:
     * - Season management and impact on supply chains
     * - Dynamic event system for world events
     * - Region population and influence tracking
     * - Trade route status and safety
     * - Political stability per region
     * - Resource availability fluctuation
     */
};