#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Data/WorldEcosystemData.h"
#include "Core/GlobalEventBus.h"
#include "WorldEcosystemSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEcosystem, Log, All);

class UEnvironmentalFactorsComponent;

// Delegates for ecosystem events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSeasonChanged, const FString&, RegionName, ESeasonType, NewSeason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged, const FString&, RegionName, const FWeatherSystemData&, NewWeather);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnvironmentalEvent, const FString&, RegionName, const FEnvironmentalEventData&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnResourceChanged, const FString&, RegionName, EBasicResourceType, ResourceType, float, NewAvailability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAnimalPopulationChanged, const FString&, RegionName, const FString&, SpeciesName, int32, NewPopulation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEcosystemHealthChanged, const FString&, RegionName, float, NewHealth);

/**
 * World Ecosystem Subsystem - Manages dynamic weather, seasons, resources, and environmental events
 */
UCLASS()
class DARKAGE_API UWorldEcosystemSubsystem : public UGameInstanceSubsystem, public IGlobalEventListener
{
	GENERATED_BODY()

public:
    UWorldEcosystemSubsystem();

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    // Tick function - called from game instance
    void Tick(float DeltaTime);

    /** If false, the entire ecosystem simulation update will be skipped. Can be toggled with the 'AdminSetEcosystemEnabled' console command. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ecosystem|Admin")
    bool bEcosystemEnabled = true;

    // --- Admin/Debug Commands ---
    void DebugPrintRegionState(const FString& RegionName) const;

    void DebugListRegions() const;

    void AdminForceSeason(const FString& RegionName, ESeasonType NewSeason);

    void AdminForceWeather(const FString& RegionName, EWeatherPattern NewWeather, float Intensity);

    void AdminSetResource(const FString& RegionName, EBasicResourceType ResourceType, float NewValue);

    void AdminSetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation);

    void AdminTriggerEvent(const FString& RegionName, EEnvironmentalEventType EventType, float Severity, float Duration);

    void AdminClearEvent(const FString& RegionName, EEnvironmentalEventType EventType);

    void AdminSetEcosystemEnabled(const bool bIsEnabled);
    // Region Management
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Region")
    void RegisterRegion(const FString& RegionName, const FString& ClimateZoneString);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Region")
    void UnregisterRegion(const FString& RegionName);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Region")
    bool HasRegion(const FString& RegionName) const;

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Region")
    TArray<FString> GetAllRegions() const;

    // Season Management
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Seasons")
    void AdvanceSeason(const FString& RegionName);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Seasons")
    void SetSeasonalConfiguration(const FString& RegionName, const FSeasonalData& SeasonData);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Seasons")
    ESeasonType GetCurrentSeason(const FString& RegionName) const;

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Seasons")
    FSeasonalData GetSeasonalData(const FString& RegionName, ESeasonType Season) const;

    // Weather Management
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Weather")
    void UpdateWeather(const FString& RegionName, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Weather")
    void ForceWeatherChange(const FString& RegionName, EWeatherPattern NewWeather, float Intensity);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Weather")
    FWeatherSystemData GetWeatherData(const FString& RegionName) const;

    // Resource Management
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Resources")
    void UpdateResources(const FString& RegionName, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Resources")
    void DepleteResource(const FString& RegionName, EBasicResourceType ResourceType, float Amount);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Resources")
    void AddResource(const FString& RegionName, EBasicResourceType ResourceType, float Amount);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Resources")
    float GetResourceAvailability(const FString& RegionName, EBasicResourceType ResourceType) const;

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Resources")
    float GetResourceQuality(const FString& RegionName, EBasicResourceType ResourceType) const;

    // Animal Population Management
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Animals")
    void UpdateAnimalPopulations(const FString& RegionName, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Animals")
    void AddAnimalSpecies(const FString& RegionName, const FString& SpeciesName, int32 InitialPopulation);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Animals")
    void RemoveAnimalSpecies(const FString& RegionName, const FString& SpeciesName);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Animals")
    void SetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Animals")
    int32 GetAnimalPopulation(const FString& RegionName, const FString& SpeciesName) const;

    // Environmental Events
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Events")
    void UpdateEnvironmentalEvents(const FString& RegionName, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Events")
    void TriggerEnvironmentalEvent(const FString& RegionName, const FEnvironmentalEventData& EventData);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Events")
    void ClearEnvironmentalEvent(const FString& RegionName, EEnvironmentalEventType EventType);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Events")
    TArray<FEnvironmentalEventData> GetActiveEvents(const FString& RegionName) const;

    // Ecosystem Health
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Health")
    void UpdateEcosystemHealth(const FString& RegionName);

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Health")
    float GetEcosystemHealth(const FString& RegionName) const;

    UFUNCTION(BlueprintPure, Category = "Ecosystem|Health")
    float GetBiodiversityIndex(const FString& RegionName) const;

    // Component Registration
    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Integration")
    void RegisterEnvironmentalComponent(UEnvironmentalFactorsComponent* Component, const FString& RegionName);

    UFUNCTION(BlueprintCallable, Category = "Ecosystem|Integration")
    void UnregisterEnvironmentalComponent(UEnvironmentalFactorsComponent* Component);

    // Event Delegates
    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnSeasonChanged OnSeasonChanged;

    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnWeatherChanged OnWeatherChanged;

    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnEnvironmentalEvent OnEnvironmentalEvent;

    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnResourceChanged OnResourceChanged;

    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnAnimalPopulationChanged OnAnimalPopulationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Ecosystem|Events")
    FOnEcosystemHealthChanged OnEcosystemHealthChanged;

public:
    // Debug/Admin registration
    void RegisterDebugCommands();
    
    // Handle global events
    UFUNCTION(BlueprintImplementableEvent, Category = "Global Events")
    void OnGlobalEventReceived(const FGlobalEvent& Event);

private:
    // Data storage
    TMap<FString, FRegionalEcosystemData> RegionalData;

    UPROPERTY()
    UDataTable* AnimalSpeciesDataTable;

    UPROPERTY()
    UDataTable* SeasonalDataTable;
    
    // References to other subsystems
    UPROPERTY()
    class UWorldStateLock* WorldStateLock;
    
    UPROPERTY()
    class UGlobalEventBus* GlobalEventBus;
    
    // Registered components
    TMap<UEnvironmentalFactorsComponent*, FString> RegisteredComponents;
    
    // Update timers
    float SeasonUpdateTimer;
    float WeatherUpdateTimer;
    float ResourceUpdateTimer;
    float AnimalUpdateTimer;
    float EventUpdateTimer;
    
    // Update intervals (in seconds)
    const float SeasonUpdateInterval = 300.0f; // 5 minutes
    const float WeatherUpdateInterval = 30.0f; // 30 seconds
    const float ResourceUpdateInterval = 60.0f; // 1 minute
    const float AnimalUpdateInterval = 120.0f; // 2 minutes
    const float EventUpdateInterval = 180.0f; // 3 minutes
    
    // Helper functions
    void InitializeRegionData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone);
    void InitializeSeasonalData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone);
    void InitializeResourceData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone);
    void UpdateRegisteredComponents();
    EClimateZoneType StringToClimateZone(const FString& ClimateZoneString) const;
    void CalculateBiodiversityIndex(FRegionalEcosystemData& RegionData);
    void ApplyEnvironmentalEventEffects(FRegionalEcosystemData& RegionData, const FEnvironmentalEventData& EventData);
    void GenerateRandomWeather(FRegionalEcosystemData& RegionData);
    bool CheckRandomEventTrigger(const FRegionalEcosystemData& RegionData);
    FEnvironmentalEventData GenerateRandomEvent(const FRegionalEcosystemData& RegionData);

    void OnDataTablesLoaded();
};
