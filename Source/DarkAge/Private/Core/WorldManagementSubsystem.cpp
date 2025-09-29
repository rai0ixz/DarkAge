#include "Core/WorldManagementSubsystem.h"
#include "Core/PlayerSaveGame.h"
#include "Data/EnvironmentalFactorsData.h"
#include "Core/WorldEcosystemSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Core/CrimeManagerSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Core/QuestManagementSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

UWorldManagementSubsystem::UWorldManagementSubsystem()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> RegionDataTableAsset(TEXT("DataTable'/Game/_DA/Data/DT_Regions.DT_Regions'"));
    if (RegionDataTableAsset.Succeeded())
    {
        RegionDataTable = RegionDataTableAsset.Object;
    }
}

void UWorldManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Initialize default values
    CurrentRegionID = TEXT("DefaultRegion");
    WorldTime = 6.0f; // Start at dawn (6 AM)
    TimeScale = 1.0f;
    DayLength = 24.0f; // 24 hour day cycle
    NightStartHour = 20.0f; // 8 PM
    DayStartHour = 6.0f; // 6 AM
    CachedEcosystemSubsystem = nullptr;

    // Safety check for DefaultRegion row in DT_Regions
    if (RegionDataTable)
    {
        FRegionData* RegionData = RegionDataTable->FindRow<FRegionData>(FName("DefaultRegion"), TEXT(""));
        if (!RegionData)
        {
            UE_LOG(LogTemp, Fatal, TEXT("Failed to find the required 'DefaultRegion' row in DT_Regions. The game cannot start. Please ensure this row exists in the Data Table at /Game/_DA/Data/DT_Regions.DT_Regions."));
            return; // The fatal log will stop execution, but a return is good practice.
        }
    }

    // Discover the starting region
    DiscoverRegion(CurrentRegionID);

    UE_LOG(LogTemp, Log, TEXT("WorldManagementSubsystem initialized. Starting in region: %s"), *CurrentRegionID);

    // Start world time updates
    if (UWorld* World = GetWorld())
    {
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &UWorldManagementSubsystem::UpdateWorldTime, 0.1f);
        World->GetTimerManager().SetTimer(
            WorldTimeUpdateTimer,
            TimerDelegate,
            0.1f,
            true
        );
    }
}

void UWorldManagementSubsystem::Deinitialize()
{
    // Clean up timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(WorldTimeUpdateTimer);
    }

    UE_LOG(LogTemp, Log, TEXT("WorldManagementSubsystem deinitialized"));
    Super::Deinitialize();
}

void UWorldManagementSubsystem::DiscoverRegion(const FString& RegionID)
{
    if (!RegionID.IsEmpty() && !DiscoveredRegions.Contains(RegionID))
    {
        DiscoveredRegions.Add(RegionID);
        
        // Initialize region weather to "Clear" by default
        RegionWeatherMap.Add(RegionID, TEXT("Clear"));
        
        UE_LOG(LogTemp, Log, TEXT("Region discovered: %s"), *RegionID);
        
        // Load region-specific data from data tables
        FRegionData RegionData = GetRegionData(RegionID);
        // FRegionData is a struct, always valid, so no pointer check needed
        // Initialize region economic state
        UEconomySubsystem* EconomySubsystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
        if (EconomySubsystem)
        {
            // EconomySubsystem->InitializeRegionEconomy(RegionID, RegionData.WealthLevel);
            UE_LOG(LogTemp, Log, TEXT("Initialized economy for region %s with wealth level %d"), *RegionID, static_cast<int32>(RegionData.WealthLevel));
        }

        // Trigger region discovery events
        OnRegionDiscovered.Broadcast(RegionID);
    }
}

bool UWorldManagementSubsystem::IsRegionDiscovered(const FString& RegionID) const
{
    return DiscoveredRegions.Contains(RegionID);
}

void UWorldManagementSubsystem::SetCurrentRegion(const FString& RegionID)
{
    if (!RegionID.IsEmpty())
    {
        // Ensure region is discovered
        if (!IsRegionDiscovered(RegionID))
        {
            DiscoverRegion(RegionID);
        }

        FString PreviousRegion = CurrentRegionID;
        CurrentRegionID = RegionID;

        UE_LOG(LogTemp, Log, TEXT("Region changed from %s to %s"), *PreviousRegion, *CurrentRegionID);

        // Trigger region change events
        OnRegionChanged.Broadcast(PreviousRegion, CurrentRegionID);

        UCrimeManagerSubsystem* CrimeSubsystem = GetGameInstance()->GetSubsystem<UCrimeManagerSubsystem>();
        if (CrimeSubsystem)
        {
            // CrimeSubsystem->UpdateAwarenessInRegion(CurrentRegionID);
            UE_LOG(LogTemp, Log, TEXT("Updated law enforcement awareness for region %s"), *CurrentRegionID);
        }

        UEconomySubsystem* EconomySubsystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
        if (EconomySubsystem)
        {
            // EconomySubsystem->UpdateSupplyChainForRegion(CurrentRegionID);
            UE_LOG(LogTemp, Log, TEXT("Updated supply chain for region %s"), *CurrentRegionID);
        }
        
        // Update weather and environmental factors
        UpdateWeatherForRegion(CurrentRegionID);
    }
}

TArray<FString> UWorldManagementSubsystem::GetDiscoveredRegions() const
{
    return DiscoveredRegions.Array();
}

TArray<FName> UWorldManagementSubsystem::GetAllRegionIDs() const
{
    if (!RegionDataTable)
    {
        return {};
    }
    return RegionDataTable->GetRowNames();
}

FRegionData UWorldManagementSubsystem::GetRegionData(const FString& RegionID) const
{
    if (!RegionDataTable)
    {
        return FRegionData();
    }

    FRegionData* Row = RegionDataTable->FindRow<FRegionData>(FName(*RegionID), TEXT(""));
    return Row ? *Row : FRegionData();
}

TMap<FName, FVector> UWorldManagementSubsystem::GetPointsOfInterest(const FString& RegionID) const
{
    FRegionData RegionData = GetRegionData(RegionID);
    return RegionData.PointsOfInterest;
}

float UWorldManagementSubsystem::GetPoliticalStability(const FString& RegionID) const
{
    FRegionData RegionData = GetRegionData(RegionID);
    return RegionData.PoliticalStability;
}

void UWorldManagementSubsystem::SetTimeScale(float NewTimeScale)
{
    TimeScale = FMath::Clamp(NewTimeScale, 0.0f, 100.0f);
    UE_LOG(LogTemp, Log, TEXT("World time scale set to: %f"), TimeScale);
}

int32 UWorldManagementSubsystem::GetCurrentDay() const
{
    return FMath::FloorToInt(WorldTime / DayLength) + 1;
}

bool UWorldManagementSubsystem::IsNightTime() const
{
    float HourOfDay = FMath::Fmod(WorldTime, DayLength);
    return (HourOfDay >= NightStartHour || HourOfDay < DayStartHour);
}


void UWorldManagementSubsystem::SaveWorldState()
{
    if (UPlayerSaveGame* SaveGameInstance = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass())))
    {
        // Save world data
        SaveGameInstance->DiscoveredRegions = this->DiscoveredRegions;
        SaveGameInstance->WorldTime = this->WorldTime;
        SaveGameInstance->RegionWeatherMap = this->RegionWeatherMap;

        // The other subsystems would save their own data to the save game instance
        // For example:
        // GetGameInstance()->GetSubsystem<UEconomySubsystem>()->SaveState(SaveGameInstance);
        // GetGameInstance()->GetSubsystem<UCrimeManagerSubsystem>()->SaveState(SaveGameInstance);

        UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("WorldState"), 0);
        UE_LOG(LogTemp, Log, TEXT("World state saved - Day %d, Time: %.2f, Region: %s"),
               GetCurrentDay(), WorldTime, *CurrentRegionID);
    }
}

void UWorldManagementSubsystem::LoadWorldState()
{
    if (UPlayerSaveGame* LoadedGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("WorldState"), 0)))
    {
        this->DiscoveredRegions = LoadedGame->DiscoveredRegions;
        this->WorldTime = LoadedGame->WorldTime;
        this->RegionWeatherMap = LoadedGame->RegionWeatherMap;

        // The other subsystems would load their own data
        // GetGameInstance()->GetSubsystem<UEconomySubsystem>()->LoadState(LoadedGame);
        // GetGameInstance()->GetSubsystem<UCrimeManagerSubsystem>()->LoadState(LoadedGame);

        UE_LOG(LogTemp, Log, TEXT("World state loaded"));
       }
      }
      
      FName UWorldManagementSubsystem::GetWorldState(FName StateID) const
      {
         const FName* State = WorldStateMap.Find(StateID);
         return State ? *State : NAME_None;
      }
      
      void UWorldManagementSubsystem::UpdateWorldTime(float DeltaTime)
{
    WorldTime += DeltaTime * TimeScale;
    
    // Broadcast time update event
    OnTimeUpdated.Broadcast(GetCurrentDay(), FMath::Fmod(WorldTime, DayLength));

    // Trigger time-based events
    UQuestManagementSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestManagementSubsystem>();
    if (QuestSubsystem)
    {
        // QuestSubsystem->OnWorldTimeUpdate(WorldTime);
    }

    UEconomySubsystem* EconomySubsystem = GetGameInstance()->GetSubsystem<UEconomySubsystem>();
    if (EconomySubsystem)
    {
        // EconomySubsystem->UpdateMarketPrices(WorldTime);
    }

    // This is where NPC schedules would be updated.
    // For example: GetGameInstance()->GetSubsystem<UNPCScheduleSubsystem>()->UpdateSchedules(WorldTime);
    
    // Notify ecosystem of time advancement
    if (CachedEcosystemSubsystem)
    {
        float DeltaTimeInDays = DeltaTime * TimeScale / 24.0f; // Convert hours to days
        NotifyTimeAdvancement(DeltaTimeInDays);
    }
}

// Integration with World Ecosystem System

void UWorldManagementSubsystem::RegisterEcosystemSubsystem(UWorldEcosystemSubsystem* EcosystemSubsystem)
{
    CachedEcosystemSubsystem = EcosystemSubsystem;
    
    if (CachedEcosystemSubsystem)
    {
        // Sync existing regions with ecosystem
        SyncRegionsWithEcosystem();
        UE_LOG(LogTemp, Log, TEXT("WorldManagementSubsystem registered with WorldEcosystemSubsystem"));
    }
}

float UWorldManagementSubsystem::GetWorldTimeInDays() const
{
    // Assuming WorldTime is in hours, convert to days
    return WorldTime / 24.0f;
}

void UWorldManagementSubsystem::NotifyTimeAdvancement(float DeltaTimeInDays)
{
    if (CachedEcosystemSubsystem)
    {
        // Broadcast time advancement event to ecosystem
        // The ecosystem subsystem will handle season progression, weather updates, etc.
        UE_LOG(LogTemp, VeryVerbose, TEXT("Notified ecosystem of time advancement: %.6f days"), DeltaTimeInDays);
    }
}

void UWorldManagementSubsystem::SyncRegionsWithEcosystem()
{
    if (!CachedEcosystemSubsystem)
    {
        return;
    }
    
    // Register all discovered regions with the ecosystem
    for (const FString& RegionID : DiscoveredRegions)
    {
        // Determine climate zone based on region data
        FRegionData RegionData = GetRegionData(RegionID);
        FString ClimateZone = RegionData.ClimateZone;
        
        CachedEcosystemSubsystem->RegisterRegion(RegionID, ClimateZone);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Synced %d regions with ecosystem subsystem"), DiscoveredRegions.Num());
}

void UWorldManagementSubsystem::UpdateWeatherForRegion(const FString& RegionID)
{
    if (CachedEcosystemSubsystem)
    {
        FWeatherSystemData WeatherData = CachedEcosystemSubsystem->GetWeatherData(RegionID);
        RegionWeatherMap.Add(RegionID, UEnum::GetValueAsString(WeatherData.CurrentWeather));
        UE_LOG(LogTemp, Log, TEXT("Weather in region %s is %s"), *RegionID, *UEnum::GetValueAsString(WeatherData.CurrentWeather));
    }
    else
    {
        const FString& Weather = RegionWeatherMap.FindOrAdd(RegionID, TEXT("Clear"));
        UE_LOG(LogTemp, Log, TEXT("Weather in region %s is %s (Ecosystem not available)"), *RegionID, *Weather);
    }
}
