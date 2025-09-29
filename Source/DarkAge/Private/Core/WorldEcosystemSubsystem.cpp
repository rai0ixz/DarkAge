#include "Core/WorldEcosystemSubsystem.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "Core/GlobalEventBus.h"
#include "Core/WorldStateLock.h"
#include "CoreMinimal.h"
#include "Core/EnvironmentalFactorsComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"
#include "Data/WorldEcosystemData.h"
#include "Engine/GameInstance.h"
#include "Subsystems/SubsystemCollection.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

UWorldEcosystemSubsystem::UWorldEcosystemSubsystem() {}

void UWorldEcosystemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Asynchronously load all data tables
    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
    TArray<FSoftObjectPath> ItemsToStream;

    const FString AnimalSpeciesDataPath = TEXT("/Game/_DA/Data/DT_AnimalSpeciesData.DT_AnimalSpeciesData");
    ItemsToStream.Add(FSoftObjectPath(AnimalSpeciesDataPath));

    const FString SeasonalDataPath = TEXT("/Game/_DA/Data/DT_SeasonalData.DT_SeasonalData");
    ItemsToStream.Add(FSoftObjectPath(SeasonalDataPath));

    StreamableManager.RequestAsyncLoad(ItemsToStream, FStreamableDelegate::CreateUObject(this, &UWorldEcosystemSubsystem::OnDataTablesLoaded));
}

void UWorldEcosystemSubsystem::OnDataTablesLoaded()
{
    UE_LOG(LogTemp, Log, TEXT("All ecosystem data tables loaded successfully."));

    // Safely assign the loaded data tables
    const FString AnimalSpeciesDataPath = TEXT("/Game/_DA/Data/DT_AnimalSpeciesData.DT_AnimalSpeciesData");
    AnimalSpeciesDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *AnimalSpeciesDataPath));
    if (!AnimalSpeciesDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get AnimalSpeciesDataTable after async load."));
    }

    const FString SeasonalDataPath = TEXT("/Game/_DA/Data/DT_SeasonalData.DT_SeasonalData");
    SeasonalDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *SeasonalDataPath));
    if (!SeasonalDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get SeasonalDataTable after async load."));
    }

    // Get references to other subsystems
    WorldStateLock = GetGameInstance()->GetSubsystem<UWorldStateLock>();
    GlobalEventBus = GetGameInstance()->GetSubsystem<UGlobalEventBus>();
    
    // Register with Global Event Bus
    if (GlobalEventBus)
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        TArray<EGlobalEventType> ListenedEvents = {
            EGlobalEventType::SeasonChanged,
            EGlobalEventType::WeatherChanged,
            EGlobalEventType::ResourceDepleted,
            EGlobalEventType::ResourceDiscovered,
            EGlobalEventType::AnimalPopulationChanged,
            EGlobalEventType::EnvironmentalEventTriggered
        };
        GlobalEventBus->RegisterListener(ListenerInterface, ListenedEvents);
        
        UE_LOG(LogTemp, Log, TEXT("WorldEcosystemSubsystem: Registered with Global Event Bus"));
    }

    if (!AnimalSpeciesDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimalSpeciesDataTable not loaded!"));
    }

    UE_LOG(LogTemp, Log, TEXT("WorldEcosystemSubsystem: Initialized"));
}

// --- Admin/Debug Function Implementations ---

void UWorldEcosystemSubsystem::DebugPrintRegionState(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        UE_LOG(LogTemp, Warning, TEXT("=== Region: %s ==="), *RegionName);
        UE_LOG(LogTemp, Warning, TEXT("Season: %s"), *UEnum::GetValueAsString(RegionData->CurrentSeason));
        UE_LOG(LogTemp, Warning, TEXT("Weather: %s (Intensity: %.2f)"), 
            *UEnum::GetValueAsString(RegionData->WeatherData.CurrentWeather), 
            RegionData->WeatherData.WeatherIntensity);
        UE_LOG(LogTemp, Warning, TEXT("Ecosystem Health: %.2f"), RegionData->EcosystemHealth);
        UE_LOG(LogTemp, Warning, TEXT("Animal Species Count: %d"), RegionData->AnimalPopulations.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::DebugListRegions() const
{
    UE_LOG(LogTemp, Warning, TEXT("=== All Registered Regions ==="));
    for (const auto& Elem : RegionalData)
    {
        const FString& RegionName = Elem.Key;
        const FRegionalEcosystemData& RegionData = Elem.Value;
        UE_LOG(LogTemp, Warning, TEXT("- %s (Health: %.2f, Season: %s)"), 
            *RegionName, 
            RegionData.EcosystemHealth,
            *UEnum::GetValueAsString(RegionData.CurrentSeason));
    }
}

void UWorldEcosystemSubsystem::AdminForceSeason(const FString& RegionName, ESeasonType NewSeason)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->CurrentSeason = NewSeason;
        OnSeasonChanged.Broadcast(RegionName, NewSeason);
        UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Forced season to %s in region %s"), 
            *UEnum::GetValueAsString(NewSeason), *RegionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminForceWeather(const FString& RegionName, EWeatherPattern NewWeather, float Intensity)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->WeatherData.CurrentWeather = NewWeather;
        RegionData->WeatherData.WeatherIntensity = Intensity;
        RegionData->WeatherData.TimeToNextChange = FMath::FRandRange(180.0f, 600.0f);
        OnWeatherChanged.Broadcast(RegionName, RegionData->WeatherData);
        UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Forced weather to %s (%.2f) in region %s"), 
            *UEnum::GetValueAsString(NewWeather), Intensity, *RegionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminSetResource(const FString& RegionName, EBasicResourceType ResourceType, float NewValue)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (FResourceData* ResourceData = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == ResourceType; }))
        {
            ResourceData->Availability = FMath::Clamp(NewValue, 0.0f, 1.0f);
            OnResourceChanged.Broadcast(RegionName, ResourceType, ResourceData->Availability);
            UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Set %s resource to %.2f in region %s"), 
                *UEnum::GetValueAsString(ResourceType), NewValue, *RegionName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[ADMIN] Resource type not found in region %s"), *RegionName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminSetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (FAnimalPopulationData* AnimalData = RegionData->AnimalPopulations.FindByPredicate(
            [&](const FAnimalPopulationData& Data) { return Data.AnimalClass->GetName() == SpeciesName; }))
        {
            AnimalData->Population = FMath::Max(0, NewPopulation);
            OnAnimalPopulationChanged.Broadcast(RegionName, SpeciesName, AnimalData->Population);
            UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Set %s population to %d in region %s"), 
                *SpeciesName, NewPopulation, *RegionName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[ADMIN] Species '%s' not found in region %s"), *SpeciesName, *RegionName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminTriggerEvent(const FString& RegionName, EEnvironmentalEventType EventType, float Severity, float Duration)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        FEnvironmentalEventData EventData;
        EventData.EventType = EventType;
        EventData.Severity = FMath::Clamp(Severity, 0.0f, 1.0f);
        EventData.DurationInDays = FMath::Max(0.0f, Duration);
        EventData.TimeRemaining = EventData.DurationInDays;
        
        TriggerEnvironmentalEvent(RegionName, EventData);
        UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Triggered %s event (Severity: %.2f, Duration: %.2f days) in region %s"), 
            *UEnum::GetValueAsString(EventType), Severity, Duration, *RegionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminClearEvent(const FString& RegionName, EEnvironmentalEventType EventType)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        ClearEnvironmentalEvent(RegionName, EventType);
        UE_LOG(LogTemp, Warning, TEXT("[ADMIN] Cleared %s event in region %s"), 
            *UEnum::GetValueAsString(EventType), *RegionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ADMIN] Region '%s' not found"), *RegionName);
    }
}

void UWorldEcosystemSubsystem::AdminSetEcosystemEnabled(const bool bIsEnabled)
{
    bEcosystemEnabled = bIsEnabled;
    UE_LOG(LogTemp, Warning, TEXT("[ADMIN] WorldEcosystemSubsystem enabled set to: %s"), bEcosystemEnabled ? TEXT("true") : TEXT("false"));
}

void UWorldEcosystemSubsystem::Deinitialize()
{
    // Unregister from Global Event Bus
    if (GlobalEventBus)
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        GlobalEventBus->UnregisterListener(ListenerInterface);
    }
    
    Super::Deinitialize();
}

void UWorldEcosystemSubsystem::Tick(float DeltaTime)
{
    // If the entire system is disabled, do nothing.
    if (!bEcosystemEnabled)
    {
        return;
    }

    // Check if it's safe to perform ecosystem updates
    if (WorldStateLock && !WorldStateLock->IsSafeForEcosystemUpdate())
    {
        // Skip ecosystem updates if save/load operations are active
        return;
    }
    
    // Request an ecosystem update lock to prevent conflicts during major changes
    FGuid EcosystemLockID;
    if (WorldStateLock)
    {
        EcosystemLockID = WorldStateLock->RequestSimpleLock(EWorldLockType::EcosystemUpdate,
            TEXT("WorldEcosystemSubsystem"), TEXT("Ecosystem tick update"),
            ELockPriority::Normal, false, 5.0f);
    }

    for (auto& Elem : RegionalData)
    {
        const FString& RegionName = Elem.Key;
        FRegionalEcosystemData& RegionData = Elem.Value;

        RegionData.DaysSinceSeasonStart += DeltaTime / 86400.0f; // Convert seconds to days

        const FSeasonalData* CurrentSeasonData = nullptr;
        if (CurrentSeasonData && RegionData.DaysSinceSeasonStart >= CurrentSeasonData->DurationInDays)
        {
            AdvanceSeason(RegionName);
            RegionData.DaysSinceSeasonStart = 0.0f;
        }
        
        UpdateWeather(RegionName, DeltaTime);
        UpdateResources(RegionName, DeltaTime);
        UpdateAnimalPopulations(RegionName, DeltaTime);
        UpdateEnvironmentalEvents(RegionName, DeltaTime);
        UpdateEcosystemHealth(RegionName);
    }
    
    UpdateRegisteredComponents();
    
    // Release the ecosystem lock
    if (WorldStateLock && EcosystemLockID.IsValid())
    {
        WorldStateLock->ReleaseLock(EcosystemLockID);
    }
}

void UWorldEcosystemSubsystem::RegisterRegion(const FString& RegionName, const FString& ClimateZoneString)
{
    if (!RegionalData.Contains(RegionName))
    {
        FRegionalEcosystemData NewRegionData;
        EClimateZoneType ClimateZone = StringToClimateZone(ClimateZoneString);
        
        InitializeRegionData(NewRegionData, ClimateZone);
        InitializeSeasonalData(NewRegionData, ClimateZone);
        InitializeResourceData(NewRegionData, ClimateZone);
        
        RegionalData.Add(RegionName, NewRegionData);
        
        UE_LOG(LogTemp, Log, TEXT("Region '%s' registered with climate zone '%s'"), *RegionName, *ClimateZoneString);
    }
}

void UWorldEcosystemSubsystem::UnregisterRegion(const FString& RegionName)
{
    if (RegionalData.Contains(RegionName))
    {
        RegionalData.Remove(RegionName);
        UE_LOG(LogTemp, Log, TEXT("Region '%s' unregistered"), *RegionName);
    }
}

bool UWorldEcosystemSubsystem::HasRegion(const FString& RegionName) const
{
    return RegionalData.Contains(RegionName);
}

TArray<FString> UWorldEcosystemSubsystem::GetAllRegions() const
{
    TArray<FString> RegionNames;
    RegionalData.GetKeys(RegionNames);
    return RegionNames;
}

void UWorldEcosystemSubsystem::AdvanceSeason(const FString& RegionName)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        ESeasonType OldSeason = RegionData->CurrentSeason;
        RegionData->CurrentSeason = static_cast<ESeasonType>(((static_cast<uint8>(RegionData->CurrentSeason) + 1) % 4));
        OnSeasonChanged.Broadcast(RegionName, RegionData->CurrentSeason);
        
        // Broadcast global event
        if (GlobalEventBus)
        {
            TMap<FString, FString> EventData;
            EventData.Add(TEXT("RegionID"), RegionName);
            EventData.Add(TEXT("OldSeason"), UEnum::GetValueAsString(OldSeason));
            EventData.Add(TEXT("NewSeason"), UEnum::GetValueAsString(RegionData->CurrentSeason));
            EventData.Add(TEXT("Description"), FString::Printf(TEXT("Season changed from %s to %s in %s"),
                *UEnum::GetValueAsString(OldSeason), *UEnum::GetValueAsString(RegionData->CurrentSeason), *RegionName));
            
            GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::SeasonChanged,
                TEXT("WorldEcosystemSubsystem"), TEXT(""), EventData, EEventPriority::Normal, false);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Season advanced to %s in region %s"), *UEnum::GetValueAsString(RegionData->CurrentSeason), *RegionName);
    }
}

void UWorldEcosystemSubsystem::SetSeasonalConfiguration(const FString& RegionName, const FSeasonalData& SeasonData)
{
}

ESeasonType UWorldEcosystemSubsystem::GetCurrentSeason(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        return RegionData->CurrentSeason;
    }
    return ESeasonType::Spring;
}

FSeasonalData UWorldEcosystemSubsystem::GetSeasonalData(const FString& RegionName, ESeasonType Season) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (const FSeasonalData* SeasonData = nullptr)
        {
            return *SeasonData;
        }
    }
    return FSeasonalData();
}

void UWorldEcosystemSubsystem::UpdateWeather(const FString& RegionName, float DeltaTime)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->WeatherData.TimeToNextChange -= DeltaTime;

        if (RegionData->WeatherData.TimeToNextChange <= 0)
        {
            EWeatherPattern OldWeather = RegionData->WeatherData.CurrentWeather;
            GenerateRandomWeather(*RegionData);
            OnWeatherChanged.Broadcast(RegionName, RegionData->WeatherData);
            
            // Broadcast global event for significant weather changes
            if (OldWeather != RegionData->WeatherData.CurrentWeather && GlobalEventBus)
            {
                TMap<FString, FString> EventData;
                EventData.Add(TEXT("RegionID"), RegionName);
                EventData.Add(TEXT("OldWeather"), UEnum::GetValueAsString(OldWeather));
                EventData.Add(TEXT("NewWeather"), UEnum::GetValueAsString(RegionData->WeatherData.CurrentWeather));
                EventData.Add(TEXT("Intensity"), FString::Printf(TEXT("%.2f"), RegionData->WeatherData.WeatherIntensity));
                EventData.Add(TEXT("Description"), FString::Printf(TEXT("Weather changed from %s to %s in %s"),
                    *UEnum::GetValueAsString(OldWeather), *UEnum::GetValueAsString(RegionData->WeatherData.CurrentWeather), *RegionName));
                
                GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::WeatherChanged,
                    TEXT("WorldEcosystemSubsystem"), TEXT(""), EventData, EEventPriority::Normal, false);
            }
        }
    }
}

void UWorldEcosystemSubsystem::ForceWeatherChange(const FString& RegionName, EWeatherPattern NewWeather, float Intensity)
{
}

FWeatherSystemData UWorldEcosystemSubsystem::GetWeatherData(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        return RegionData->WeatherData;
    }
    return FWeatherSystemData();
}

void UWorldEcosystemSubsystem::UpdateResources(const FString& RegionName, float DeltaTime)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        for (FResourceData& ResourceData : RegionData->Resources)
        {
            ResourceData.Availability = FMath::Min(ResourceData.MaxCapacity, ResourceData.Availability + (ResourceData.RegenerationRate * DeltaTime));
        }
    }
}

void UWorldEcosystemSubsystem::DepleteResource(const FString& RegionName, EBasicResourceType ResourceType, float Amount)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (FResourceData* ResourceData = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == ResourceType; }))
        {
            ResourceData->Availability = FMath::Max(0.0f, ResourceData->Availability - Amount);
            OnResourceChanged.Broadcast(RegionName, ResourceType, ResourceData->Availability);
        }
    }
}

void UWorldEcosystemSubsystem::AddResource(const FString& RegionName, EBasicResourceType ResourceType, float Amount)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (FResourceData* ResourceData = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == ResourceType; }))
        {
            ResourceData->Availability = FMath::Min(ResourceData->MaxCapacity, ResourceData->Availability + Amount);
            OnResourceChanged.Broadcast(RegionName, ResourceType, ResourceData->Availability);
        }
    }
}

float UWorldEcosystemSubsystem::GetResourceAvailability(const FString& RegionName, EBasicResourceType ResourceType) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (const FResourceData* ResourceData = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == ResourceType; }))
        {
            return ResourceData->Availability;
        }
    }
    return 0.0f;
}

float UWorldEcosystemSubsystem::GetResourceQuality(const FString& RegionName, EBasicResourceType ResourceType) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (const FResourceData* ResourceData = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == ResourceType; }))
        {
            return ResourceData->Quality;
        }
    }
    return 0.0f;
}

void UWorldEcosystemSubsystem::UpdateAnimalPopulations(const FString& RegionName, float DeltaTime)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        TMap<FString, int32> PopulationChanges;

        for (FAnimalPopulationData& AnimalData : RegionData->AnimalPopulations)
        {
            float GrowthFactor = AnimalData.GrowthRate;
            float PredationFactor = 0.0f;
            float FoodAvailability = 1.0f;

            // Calculate Food Availability
            if (AnimalData.Diet == EAnimalDiet::Herbivore || AnimalData.Diet == EAnimalDiet::Omnivore)
            {
                if (const FResourceData* Vegetation = RegionData->Resources.FindByPredicate([&](const FResourceData& d) { return d.ResourceType == EBasicResourceType::Food; }))
                {
                    FoodAvailability *= (Vegetation->Availability / Vegetation->MaxCapacity);
                }
            }
            if (AnimalData.Diet == EAnimalDiet::Carnivore || AnimalData.Diet == EAnimalDiet::Omnivore)
            {
                float PreyPopulation = 0;
                // This part is complex and depends on how Prey is defined in FAnimalPopulationData
                // Assuming it's a TArray<TSubclassOf<AActor>>
                // for (TSubclassOf<AActor> PreyClass : AnimalData.Prey)
                // {
                //     if (const FAnimalPopulationData* PreyData = RegionData->AnimalPopulations.FindByPredicate([&](const FAnimalPopulationData& d) { return d.AnimalClass == PreyClass; }))
                //     {
                //         PreyPopulation += PreyData->Population;
                //     }
                // }
                // Simple model: food availability based on total prey numbers
                if (AnimalData.Population > 0)
                {
                    FoodAvailability *= FMath::Clamp(PreyPopulation / (AnimalData.Population * 10.0f), 0.0f, 1.0f);
                }
            }

            // Calculate Predation
            // This part is complex and depends on how Predators is defined in FAnimalPopulationData
            // Assuming it's a TArray<TSubclassOf<AActor>>
            // for (TSubclassOf<AActor> PredatorClass : AnimalData.Predators)
            // {
            //     if (const FAnimalPopulationData* PredatorData = RegionData->AnimalPopulations.FindByPredicate([&](const FAnimalPopulationData& d) { return d.AnimalClass == PredatorClass; }))
            //     {
            //         // Simple model: predation increases with predator population
            //         PredationFactor += PredatorData->Population * 0.001f;
            //     }
            // }

            int32 Change = FMath::RoundToInt((AnimalData.Population * GrowthFactor * FoodAvailability) - (AnimalData.Population * PredationFactor));
            PopulationChanges.Add(AnimalData.AnimalClass->GetName(), Change);
        }

        // Apply changes
        for (auto& Elem : PopulationChanges)
        {
            if (FAnimalPopulationData* AnimalData = RegionData->AnimalPopulations.FindByPredicate([&](const FAnimalPopulationData& d) { return d.AnimalClass->GetName() == Elem.Key; }))
            {
                AnimalData->Population = FMath::Max(0, AnimalData->Population + Elem.Value);
                OnAnimalPopulationChanged.Broadcast(RegionName, AnimalData->AnimalClass->GetName(), AnimalData->Population);
            }
        }
    }
}

void UWorldEcosystemSubsystem::AddAnimalSpecies(const FString& RegionName, const FString& SpeciesName, int32 InitialPopulation)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (AnimalSpeciesDataTable)
        {
            FAnimalPopulationData* Row = AnimalSpeciesDataTable->FindRow<FAnimalPopulationData>(FName(*SpeciesName), TEXT(""));
            if (Row)
            {
                FAnimalPopulationData NewAnimalData = *Row;
                NewAnimalData.Population = InitialPopulation;
                RegionData->AnimalPopulations.Add(NewAnimalData);
            }
        }
    }
}

void UWorldEcosystemSubsystem::RemoveAnimalSpecies(const FString& RegionName, const FString& SpeciesName)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->AnimalPopulations.RemoveAll([&](const FAnimalPopulationData& AnimalData)
        {
            return AnimalData.AnimalClass->GetName() == SpeciesName;
        });
    }
}

void UWorldEcosystemSubsystem::SetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (FAnimalPopulationData* AnimalData = RegionData->AnimalPopulations.FindByPredicate([&](const FAnimalPopulationData& d) { return d.AnimalClass->GetName() == SpeciesName; }))
        {
            AnimalData->Population = NewPopulation;
            OnAnimalPopulationChanged.Broadcast(RegionName, SpeciesName, NewPopulation);
        }
    }
}

int32 UWorldEcosystemSubsystem::GetAnimalPopulation(const FString& RegionName, const FString& SpeciesName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        if (const FAnimalPopulationData* AnimalData = RegionData->AnimalPopulations.FindByPredicate([&](const FAnimalPopulationData& d) { return d.AnimalClass->GetName() == SpeciesName; }))
        {
            return AnimalData->Population;
        }
    }
    return 0;
}

void UWorldEcosystemSubsystem::UpdateEnvironmentalEvents(const FString& RegionName, float DeltaTime)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        for (int32 i = RegionData->ActiveEvents.Num() - 1; i >= 0; --i)
        {
            FEnvironmentalEventData& EventData = RegionData->ActiveEvents[i];
            EventData.TimeRemaining -= DeltaTime;

            if (EventData.TimeRemaining <= 0)
            {
                UE_LOG(LogTemp, Log, TEXT("Environmental event '%s' has ended in region '%s'"), *UEnum::GetValueAsString(EventData.EventType), *RegionName);
                RegionData->ActiveEvents.RemoveAt(i);
            }
        }
    }
}

void UWorldEcosystemSubsystem::TriggerEnvironmentalEvent(const FString& RegionName, const FEnvironmentalEventData& EventData)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->ActiveEvents.Add(EventData);
        OnEnvironmentalEvent.Broadcast(RegionName, EventData);
        
        // Broadcast global event
        if (GlobalEventBus)
        {
            TMap<FString, FString> GlobalEventData;
            GlobalEventData.Add(TEXT("RegionID"), RegionName);
            GlobalEventData.Add(TEXT("EventType"), UEnum::GetValueAsString(EventData.EventType));
            GlobalEventData.Add(TEXT("Severity"), FString::Printf(TEXT("%.2f"), EventData.Severity));
            GlobalEventData.Add(TEXT("Duration"), FString::Printf(TEXT("%.2f"), EventData.DurationInDays));
            GlobalEventData.Add(TEXT("Description"), FString::Printf(TEXT("Environmental event %s triggered in %s"),
                *UEnum::GetValueAsString(EventData.EventType), *RegionName));
            
            GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::EnvironmentalEventTriggered,
                TEXT("WorldEcosystemSubsystem"), TEXT(""), GlobalEventData, EEventPriority::Normal, false);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Environmental event '%s' triggered in region '%s'"), *UEnum::GetValueAsString(EventData.EventType), *RegionName);
    }
}

void UWorldEcosystemSubsystem::ClearEnvironmentalEvent(const FString& RegionName, EEnvironmentalEventType EventType)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        RegionData->ActiveEvents.RemoveAll([&](const FEnvironmentalEventData& EventData)
        {
            return EventData.EventType == EventType;
        });
    }
}

TArray<FEnvironmentalEventData> UWorldEcosystemSubsystem::GetActiveEvents(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        return RegionData->ActiveEvents;
    }
    return TArray<FEnvironmentalEventData>();
}

void UWorldEcosystemSubsystem::UpdateEcosystemHealth(const FString& RegionName)
{
    if (FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        CalculateBiodiversityIndex(*RegionData);

        float ResourceHealth = 0.0f;
        if (RegionData->Resources.Num() > 0)
        {
            for (const auto& ResourceData : RegionData->Resources)
            {
                ResourceHealth += ResourceData.Availability / ResourceData.MaxCapacity;
            }
            ResourceHealth /= RegionData->Resources.Num();
        }

        RegionData->EcosystemHealth = (RegionData->BiodiversityIndex + ResourceHealth) / 2.0f;

        OnEcosystemHealthChanged.Broadcast(RegionName, RegionData->EcosystemHealth);
    }
}

float UWorldEcosystemSubsystem::GetEcosystemHealth(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        return RegionData->EcosystemHealth;
    }
    return 0.0f;
}

float UWorldEcosystemSubsystem::GetBiodiversityIndex(const FString& RegionName) const
{
    if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
    {
        return RegionData->BiodiversityIndex;
    }
    return 0.0f;
}

void UWorldEcosystemSubsystem::RegisterEnvironmentalComponent(UEnvironmentalFactorsComponent* Component, const FString& RegionName)
{
    if (Component)
    {
        RegisteredComponents.Add(Component, RegionName);
    }
}

void UWorldEcosystemSubsystem::UnregisterEnvironmentalComponent(UEnvironmentalFactorsComponent* Component)
{
    if (Component)
    {
        RegisteredComponents.Remove(Component);
    }
}

void UWorldEcosystemSubsystem::InitializeRegionData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone)
{
    RegionData.ClimateZone = ClimateZone;
    RegionData.EcosystemHealth = 1.0f; // Start with a healthy ecosystem
    RegionData.BiodiversityIndex = 1.0f; // Start with high biodiversity
}

void UWorldEcosystemSubsystem::InitializeSeasonalData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone)
{
    if (SeasonalDataTable)
    {
        // FClimateZoneSeasons is not a real struct, so this will not compile
        // FClimateZoneSeasons* Row = SeasonalDataTable->FindRow<FClimateZoneSeasons>(*UEnum::GetValueAsString(ClimateZone), TEXT(""));
        // if (Row)
        // {
        //     for (const FSeasonalData& SeasonData : Row->Seasons)
        //     {
        //         RegionData.SeasonalData.Add(SeasonData.Season, SeasonData);
        //     }
        //     RegionData.CurrentSeason = ESeasonType::Spring; // Default to spring
        // }
    }
}

void UWorldEcosystemSubsystem::InitializeResourceData(FRegionalEcosystemData& RegionData, EClimateZoneType ClimateZone)
{
    RegionData.Resources.Empty(); // Clear any existing resources

    switch (ClimateZone)
    {
        case EClimateZoneType::Temperate:
   RegionData.Resources.Emplace(EBasicResourceType::Water, 0.8f, 0.1f, 1.0f, 0.8f);
   RegionData.Resources.Emplace(EBasicResourceType::Food, 0.9f, 0.15f, 1.0f, 0.7f);
   RegionData.Resources.Emplace(EBasicResourceType::Wood, 0.7f, 0.05f, 1.0f, 0.6f);
   RegionData.Resources.Emplace(EBasicResourceType::Stone, 0.8f, 0.02f, 1.0f, 0.7f);
   RegionData.Resources.Emplace(EBasicResourceType::Iron, 0.5f, 0.005f, 1.0f, 0.5f);
   break;
  case EClimateZoneType::Arid:
   RegionData.Resources.Emplace(EBasicResourceType::Water, 0.2f, 0.01f, 0.5f, 0.4f);
   RegionData.Resources.Emplace(EBasicResourceType::Food, 0.3f, 0.02f, 0.6f, 0.5f);
   RegionData.Resources.Emplace(EBasicResourceType::Stone, 0.4f, 0.03f, 0.8f, 0.7f);
   RegionData.Resources.Emplace(EBasicResourceType::Iron, 0.8f, 0.001f, 1.0f, 0.8f);
   RegionData.Resources.Emplace(EBasicResourceType::Gold, 0.9f, 0.002f, 1.0f, 0.9f);
   break;
  case EClimateZoneType::Tundra:
   RegionData.Resources.Emplace(EBasicResourceType::Water, 0.9f, 0.05f, 1.0f, 0.9f); // Mostly frozen
   RegionData.Resources.Emplace(EBasicResourceType::Food, 0.4f, 0.05f, 0.7f, 0.4f);
   RegionData.Resources.Emplace(EBasicResourceType::Wood, 0.6f, 0.04f, 0.9f, 0.8f);
   RegionData.Resources.Emplace(EBasicResourceType::Stone, 0.5f, 0.01f, 0.8f, 0.6f);
   RegionData.Resources.Emplace(EBasicResourceType::Iron, 0.6f, 0.002f, 1.0f, 0.7f);
   break;
  case EClimateZoneType::Tropical:
   RegionData.Resources.Emplace(EBasicResourceType::Water, 0.7f, 0.2f, 1.0f, 0.6f);
   RegionData.Resources.Emplace(EBasicResourceType::Food, 1.0f, 0.25f, 1.0f, 0.8f);
   RegionData.Resources.Emplace(EBasicResourceType::Wood, 0.9f, 0.1f, 1.0f, 0.7f);
   RegionData.Resources.Emplace(EBasicResourceType::Gold, 1.0f, 0.15f, 1.0f, 0.9f);
   break;
    }
}

void UWorldEcosystemSubsystem::UpdateRegisteredComponents()
{
    for (auto& Elem : RegisteredComponents)
    {
        if (UEnvironmentalFactorsComponent* Component = Elem.Key)
        {
            const FString& RegionName = Elem.Value;
            if (const FRegionalEcosystemData* RegionData = RegionalData.Find(RegionName))
            {
                // If FEnvironmentalFactors is defined and has a suitable constructor, use it directly:
                // Otherwise, just pass a default-constructed FEnvironmentalFactors for now (to avoid build break)
                // Component->UpdateEnvironmentalFactors(FEnvironmentalFactors());
            }
        }
    }
}

EClimateZoneType UWorldEcosystemSubsystem::StringToClimateZone(const FString& ClimateZoneString) const
{
    if (ClimateZoneString.Equals(TEXT("Temperate"), ESearchCase::IgnoreCase))
    {
        return EClimateZoneType::Temperate;
    }
    if (ClimateZoneString.Equals(TEXT("Arid"), ESearchCase::IgnoreCase))
    {
        return EClimateZoneType::Arid;
    }
    // No Tropical or Arctic in EClimateZoneType, map to closest valid values
    if (ClimateZoneString.Equals(TEXT("Tropical"), ESearchCase::IgnoreCase))
    {
        return EClimateZoneType::Tropical;
    }
    if (ClimateZoneString.Equals(TEXT("Arctic"), ESearchCase::IgnoreCase))
    {
        return EClimateZoneType::Tundra;
    }
    return EClimateZoneType::Temperate; // Default value
}

void UWorldEcosystemSubsystem::CalculateBiodiversityIndex(FRegionalEcosystemData& RegionData)
{
    if (AnimalSpeciesDataTable)
    {
        int32 TotalSpecies = AnimalSpeciesDataTable->GetRowMap().Num();
        if (TotalSpecies > 0)
        {
            RegionData.BiodiversityIndex = (float)RegionData.AnimalPopulations.Num() / (float)TotalSpecies;
        }
        else
        {
            RegionData.BiodiversityIndex = 0.0f;
        }
    }
    else
    {
        RegionData.BiodiversityIndex = 0.0f;
    }
}

void UWorldEcosystemSubsystem::ApplyEnvironmentalEventEffects(FRegionalEcosystemData& RegionData, const FEnvironmentalEventData& EventData)
{
}

void UWorldEcosystemSubsystem::GenerateRandomWeather(FRegionalEcosystemData& RegionData)
{
    const FSeasonalData* CurrentSeasonData = nullptr;
    if (CurrentSeasonData && CurrentSeasonData->CommonWeatherPatterns.Num() > 0)
    {
        // Select a random weather pattern from the common patterns for the season
        int32 WeatherIndex = FMath::RandRange(0, CurrentSeasonData->CommonWeatherPatterns.Num() - 1);
        RegionData.WeatherData.CurrentWeather = CurrentSeasonData->CommonWeatherPatterns[WeatherIndex];
    }
    else
    {
        // Fallback to a default weather pattern if no seasonal data is available
        RegionData.WeatherData.CurrentWeather = EWeatherPattern::Clear;
    }

    // Reset the timer for the next weather change
    RegionData.WeatherData.TimeToNextChange = FMath::FRandRange(180.0f, 600.0f); // Change weather every 3-10 minutes

    RegionData.WeatherData.WeatherIntensity = FMath::FRandRange(0.1f, 1.0f);
    float TempMod = 0.0f;
    float HumidMod = 0.0f;
    float WindMod = 0.0f;

    switch(RegionData.WeatherData.CurrentWeather)
    {
        case EWeatherPattern::Rain:
            HumidMod = FMath::FRandRange(0.2f, 0.5f);
            WindMod = FMath::FRandRange(5.0f, 15.0f);
            RegionData.WeatherData.WeatherIntensity = FMath::FRandRange(0.3f, 0.8f);
            break;
        case EWeatherPattern::Storm:
            HumidMod = FMath::FRandRange(0.4f, 0.6f);
            WindMod = FMath::FRandRange(15.0f, 40.0f);
            TempMod = FMath::FRandRange(-5.0f, 0.0f);
            RegionData.WeatherData.WeatherIntensity = FMath::FRandRange(0.7f, 1.0f);
            break;
        case EWeatherPattern::Snow:
            HumidMod = FMath::FRandRange(0.1f, 0.4f);
            TempMod = FMath::FRandRange(-15.0f, -5.0f);
            RegionData.WeatherData.WeatherIntensity = FMath::FRandRange(0.4f, 0.9f);
            break;
        case EWeatherPattern::Clear:
        case EWeatherPattern::Cloudy:
        default:
            break;
    }

    RegionData.WeatherData.CurrentTemperature = (CurrentSeasonData ? CurrentSeasonData->BaseTemperature : 15.0f) + TempMod + FMath::FRandRange(-2.0f, 2.0f);
    RegionData.WeatherData.CurrentHumidity = FMath::Clamp((CurrentSeasonData ? CurrentSeasonData->BaseHumidity : 0.5f) + HumidMod, 0.0f, 1.0f);
    RegionData.WeatherData.WindSpeed = WindMod + FMath::FRandRange(0.0f, 5.0f);
    RegionData.WeatherData.Visibility = FMath::Clamp(1.0f - (RegionData.WeatherData.WeatherIntensity * 0.5f), 0.1f, 1.0f);

    UE_LOG(LogTemp, Log, TEXT("Weather changed to %s in region %s"), *UEnum::GetValueAsString(RegionData.WeatherData.CurrentWeather), *RegionData.RegionID.ToString());
}

bool UWorldEcosystemSubsystem::CheckRandomEventTrigger(const FRegionalEcosystemData& RegionData)
{
    return false;
}

FEnvironmentalEventData UWorldEcosystemSubsystem::GenerateRandomEvent(const FRegionalEcosystemData& RegionData)
{
    return FEnvironmentalEventData();
}
