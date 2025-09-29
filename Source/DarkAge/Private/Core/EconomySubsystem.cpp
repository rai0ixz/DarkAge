#include "EconomySubsystem.h"
#include "DarkAge.h"
#include "TimerManager.h"
#include "Core/DAGameInstance.h"
#include "Data/ItemData.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/Archive.h"

FArchive& operator<<(FArchive& Ar, FBasicMarketData& Data)
{
    Ar << Data.Supply;
    Ar << Data.Demand;
    Ar << Data.BasePrice;
    Ar << Data.CurrentPrice;
    return Ar;
}

FArchive& operator<<(FArchive& Ar, FRegionMarketData& Data)
{
    Ar << Data.MarketItems;
    return Ar;
}
UEconomySubsystem::UEconomySubsystem()
{
}

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FString ResetFlagPath = FPaths::ProjectSavedDir() + TEXT("ResetEconomy.flag");
    if (IFileManager::Get().FileExists(*ResetFlagPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("ResetEconomy.flag found. Deleting Economy.sav and re-initializing."));
        IFileManager::Get().Delete(*(FPaths::ProjectSavedDir() + TEXT("Economy.sav")));
        IFileManager::Get().Delete(*ResetFlagPath);
    }
    
    LoadEconomy();

    // This check is the most robust place to handle default data creation
    if (RegionalMarkets.Num() == 0)
    {
        InitializeDefaultEconomy();
        SaveEconomy(); // Save the new default data so the file exists next time
    }

    UE_LOG(LogTemp, Log, TEXT("EconomySubsystem Initialized. Loaded %d regional markets."), RegionalMarkets.Num());

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TradeRouteTimerHandle, this, &UEconomySubsystem::ProcessTradeRoutes, 300.0f, true);
    }
}

void UEconomySubsystem::Deinitialize()
{
    SaveEconomy();
    UE_LOG(LogTemp, Log, TEXT("EconomySubsystem Stopped."));
    Super::Deinitialize();
}

void UEconomySubsystem::RegisterItem(const FName& Region, const FName& ItemId, float BasePrice)
{
    FRegionMarketData& RegionMarket = RegionalMarkets.FindOrAdd(Region);
    if (!RegionMarket.MarketItems.Contains(ItemId))
    {
        FBasicMarketData NewItem;
        NewItem.BasePrice = BasePrice;
        NewItem.CurrentPrice = BasePrice;
        RegionMarket.MarketItems.Add(ItemId, NewItem);
    }
}

void UEconomySubsystem::UpdateSupply(const FName& Region, const FName& ItemId, float Change)
{
    if (FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        if (FBasicMarketData* ItemData = RegionMarket->MarketItems.Find(ItemId))
        {
            ItemData->Supply = FMath::Max(0.f, ItemData->Supply + Change);
            UpdatePrice(Region, ItemId);
        }
    }
}

void UEconomySubsystem::UpdateDemand(const FName& Region, const FName& ItemId, float Change)
{
    if (FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        if (FBasicMarketData* ItemData = RegionMarket->MarketItems.Find(ItemId))
        {
            ItemData->Demand = FMath::Max(0.f, ItemData->Demand + Change);
            UpdatePrice(Region, ItemId);
        }
    }
}

float UEconomySubsystem::GetItemPrice(const FName& Region, const FName& ItemId) const
{
    if (const FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        if (const FBasicMarketData* ItemData = RegionMarket->MarketItems.Find(ItemId))
        {
            return ItemData->CurrentPrice;
        }
    }
    return 0.f;
}

TArray<FName> UEconomySubsystem::GetItemsWithHighDemand(const FName& Region) const
{
    TArray<FName> HighDemandItems;
    if (const FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        for (const auto& Elem : RegionMarket->MarketItems)
        {
            if (Elem.Value.Demand > Elem.Value.Supply * 1.5f) // Example threshold for high demand
            {
                HighDemandItems.Add(Elem.Key);
            }
        }
    }
    return HighDemandItems;
}

TArray<FName> UEconomySubsystem::GetItemsWithLowSupply(const FName& Region) const
{
    TArray<FName> LowSupplyItems;
    if (const FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        for (const auto& Elem : RegionMarket->MarketItems)
        {
            if (Elem.Value.Supply < Elem.Value.Demand * 0.5f) // Example threshold for low supply
            {
                LowSupplyItems.Add(Elem.Key);
            }
        }
    }
    return LowSupplyItems;
}

void UEconomySubsystem::ProcessTradeRoutes()
{
    for (const FTradeRouteData& Route : TradeRoutes)
    {
        if (Route.Efficiency <= 0)
        {
            continue;
        }

        FRegionMarketData* StartRegionMarket = RegionalMarkets.Find(Route.StartRegion);
        FRegionMarketData* EndRegionMarket = RegionalMarkets.Find(Route.EndRegion);

        if (StartRegionMarket && EndRegionMarket)
        {
            for (auto& ItemPair : StartRegionMarket->MarketItems)
            {
                FBasicMarketData& StartItem = ItemPair.Value;
                FBasicMarketData* EndItem = EndRegionMarket->MarketItems.Find(ItemPair.Key);

                if (EndItem)
                {
                    float PriceDifference = EndItem->CurrentPrice - StartItem.CurrentPrice;
                    if (PriceDifference > 0)
                    {
                        // Move goods from low price to high price region
                        float AmountToTrade = FMath::Min(StartItem.Supply * 0.1f, PriceDifference * Route.Efficiency);
                        StartItem.Supply -= AmountToTrade;
                        EndItem->Supply += AmountToTrade;

                        UpdatePrice(Route.StartRegion, ItemPair.Key);
                        UpdatePrice(Route.EndRegion, ItemPair.Key);
                    }
                }
            }
        }
    }
}

void UEconomySubsystem::UpdatePrice(const FName& Region, const FName& ItemId)
{
    if (FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        if (FBasicMarketData* ItemData = RegionMarket->MarketItems.Find(ItemId))
        {
            float DemandFactor = FMath::Pow(ItemData->Demand / 1000.f, 0.5f);
            float SupplyFactor = FMath::Pow(ItemData->Supply / 1000.f, 0.5f);
            ItemData->CurrentPrice = ItemData->BasePrice * (1.0f + DemandFactor - SupplyFactor);
            ItemData->CurrentPrice = FMath::Max(1.f, ItemData->CurrentPrice); // Ensure price doesn't drop below a minimum value
        }
    }
}

bool UEconomySubsystem::GetItemData(FName ItemID, FItemData& OutItemData) const
{
    UDAGameInstance* GameInstance = Cast<UDAGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        return GameInstance->GetItemData(ItemID, OutItemData);
    }
    return false;
}

void UEconomySubsystem::TriggerMarketEvent(const FMarketEvent& Event)
{
    ActiveMarketEvents.Add(Event);
    UpdateMarketEvent(Event);

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &UEconomySubsystem::UpdateMarketEvent, Event);
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Event.Duration, false);
    }
}

void UEconomySubsystem::UpdateSeasonalEffects(const FName& Season)
{
    // This is a simplified implementation. A real implementation would use a data table to get seasonal effects.
    if (Season == "Winter")
    {
        UpdateSupply("Heartlands", "Grain", -500);
        UpdateDemand("Heartlands", "Firewood", 500);
    }
    else if (Season == "Summer")
    {
        UpdateSupply("Heartlands", "Grain", 1000);
        UpdateDemand("Heartlands", "Firewood", -500);
    }
}

void UEconomySubsystem::InitializeDefaultEconomy()
{
    UE_LOG(LogTemp, Log, TEXT("No save file found. Initializing default economy."));

    // Clear any existing data just in case
    RegionalMarkets.Empty();

    // Create some default regions and items
    // You can pull this data from a DataTable for better design
    FName Heartlands = "Heartlands";
    RegisterItem(Heartlands, "Grain", 2.0f);
    UpdateSupply(Heartlands, "Grain", 2000.0f);
    UpdateDemand(Heartlands, "Grain", 1500.0f);

    RegisterItem(Heartlands, "IronOre", 5.0f);
    UpdateSupply(Heartlands, "IronOre", 800.0f);
    UpdateDemand(Heartlands, "IronOre", 1000.0f);

    FName Frostspire = "Frostspire";
    RegisterItem(Frostspire, "IronOre", 4.0f);
    UpdateSupply(Frostspire, "IronOre", 3000.0f);
    UpdateDemand(Frostspire, "IronOre", 1200.0f);

    RegisterItem(Frostspire, "Furs", 10.0f);
    UpdateSupply(Frostspire, "Furs", 1500.0f);
    UpdateDemand(Frostspire, "Furs", 700.0f);

    UE_LOG(LogTemp, Log, TEXT("Default economy initialized with %d regions."), RegionalMarkets.Num());
}

void UEconomySubsystem::SaveEconomy() const
{
    TArray<uint8> SaveData;
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Economy.sav");

    {
        FArchiveSaveCompressedProxy Compressor(SaveData, NAME_Zlib);

        int32 SaveVersion = 1;
        Compressor << SaveVersion;

        int32 RegionCount = RegionalMarkets.Num();
        Compressor << RegionCount;

        for (const auto& RegionPair : RegionalMarkets)
        {
            FName RegionName = RegionPair.Key;
            FRegionMarketData RegionData = RegionPair.Value;

            Compressor << RegionName;
            Compressor << RegionData;
        }
    }

    if (SaveData.Num() > 0)
    {
        if (FFileHelper::SaveArrayToFile(SaveData, *SavePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully saved economy data to %s"), *SavePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save economy data to %s"), *SavePath);
        }
    }
}

void UEconomySubsystem::LoadEconomy()
{
    TArray<uint8> CompressedData;
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Economy.sav");

    if (!FFileHelper::LoadFileToArray(CompressedData, *SavePath) || CompressedData.Num() == 0)
    {
        return;
    }

    FArchiveLoadCompressedProxy Decompressor(CompressedData, NAME_Zlib);

    if (Decompressor.IsError())
    {
        UE_LOG(LogTemp, Warning, TEXT("Economy.sav appears to be corrupt (decompression failed). Deleting and re-initializing."));
        IFileManager::Get().Delete(*SavePath);
        return;
    }

    const int32 CurrentVersion = 1;
    int32 SaveVersion = 0;
    Decompressor << SaveVersion;

    if (Decompressor.IsError() || SaveVersion != CurrentVersion)
    {
        UE_LOG(LogTemp, Warning, TEXT("Economy.sav is an old version or corrupt. Version found: %d, expected: %d. Deleting and re-initializing."), SaveVersion, CurrentVersion);
        IFileManager::Get().Delete(*SavePath);
        return;
    }

    int32 RegionCount = 0;
    Decompressor << RegionCount;

    if (Decompressor.IsError() || RegionCount < 0 || RegionCount > 10000) // Sanity check
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid region count in Economy.sav: %d. Deleting and re-initializing."), RegionCount);
        IFileManager::Get().Delete(*SavePath);
        return;
    }

    RegionalMarkets.Empty();
    for (int32 i = 0; i < RegionCount; ++i)
    {
        if (Decompressor.AtEnd())
        {
            UE_LOG(LogTemp, Warning, TEXT("Unexpected end of file in Economy.sav. Deleting and re-initializing."));
            IFileManager::Get().Delete(*SavePath);
            RegionalMarkets.Empty();
            return;
        }

        FName RegionName;
        FRegionMarketData RegionData;
        Decompressor << RegionName;
        Decompressor << RegionData;

        if (Decompressor.IsError())
        {
            UE_LOG(LogTemp, Warning, TEXT("Error during deserialization of Economy.sav. Deleting and re-initializing."));
            IFileManager::Get().Delete(*SavePath);
            RegionalMarkets.Empty();
            return;
        }

        RegionalMarkets.Add(RegionName, RegionData);
    }
}


void UEconomySubsystem::RecordPlayerTransaction(const FName& Region, const FName& ItemId, int32 Quantity, bool bIsPurchase)
{
    //extend functionality
    if (bIsPurchase)
    {
        UpdateDemand(Region, ItemId, Quantity);
    }
    else
    {
        UpdateSupply(Region, ItemId, Quantity);
    }
}

void UEconomySubsystem::UpdateMarketEvent(FMarketEvent Event)
{
    UpdateSupply(Event.Region, Event.ItemId, Event.SupplyChange);
    UpdateDemand(Event.Region, Event.ItemId, Event.DemandChange);
}

// Advanced Economic Simulation Features
void UEconomySubsystem::ProcessAdvancedEconomicSimulation()
{
    ProcessInflationDeflation();
    ProcessEconomicCycles();
    ProcessPlayerEconomicImpact();
    ProcessFactionEconomicEffects();
    ProcessSeasonalEconomicChanges();
    ProcessRandomEconomicEvents();
    
    UE_LOG(LogTemp, Log, TEXT("Advanced economic simulation processed"));
}

void UEconomySubsystem::ProcessInflationDeflation()
{
    // Calculate overall economic health
    float TotalEconomicActivity = CalculateTotalEconomicActivity();
    float InflationRate = CalculateInflationRate(TotalEconomicActivity);
    
    // Apply inflation/deflation to all items across all regions
    for (auto& RegionPair : RegionalMarkets)
    {
        FRegionMarketData& RegionMarket = RegionPair.Value;
        
        for (auto& ItemPair : RegionMarket.MarketItems)
        {
            FBasicMarketData& ItemData = ItemPair.Value;
            
            // Apply inflation to base prices
            ItemData.BasePrice *= (1.0f + InflationRate);
            ItemData.BasePrice = FMath::Max(0.1f, ItemData.BasePrice); // Minimum price floor
            
            // Update current price
            UpdatePrice(RegionPair.Key, ItemPair.Key);
        }
    }
    
    if (FMath::Abs(InflationRate) > 0.01f) // Only log significant inflation changes
    {
        UE_LOG(LogTemp, Log, TEXT("Economic inflation rate: %.2f%% - Total activity: %.2f"),
            InflationRate * 100.0f, TotalEconomicActivity);
    }
}

float UEconomySubsystem::CalculateTotalEconomicActivity()
{
    float TotalActivity = 0.0f;
    int32 ItemCount = 0;
    
    for (const auto& RegionPair : RegionalMarkets)
    {
        const FRegionMarketData& RegionMarket = RegionPair.Value;
        
        for (const auto& ItemPair : RegionMarket.MarketItems)
        {
            const FBasicMarketData& ItemData = ItemPair.Value;
            
            // Economic activity = trade volume (supply + demand) * price
            float ItemActivity = (ItemData.Supply + ItemData.Demand) * ItemData.CurrentPrice;
            TotalActivity += ItemActivity;
            ItemCount++;
        }
    }
    
    return ItemCount > 0 ? TotalActivity / ItemCount : 0.0f;
}

float UEconomySubsystem::CalculateInflationRate(float EconomicActivity)
{
    // Base inflation rate
    float BaseInflation = 0.001f; // 0.1% per cycle
    
    // High economic activity increases inflation
    float ActivityMultiplier = FMath::Clamp(EconomicActivity / 10000.0f, 0.5f, 2.0f);
    
    // Random economic fluctuations
    float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
    
    float InflationRate = BaseInflation * ActivityMultiplier * RandomFactor;
    
    // Clamp inflation to reasonable bounds
    return FMath::Clamp(InflationRate, -0.05f, 0.05f); // -5% to +5% per cycle
}

void UEconomySubsystem::ProcessEconomicCycles()
{
    // Simulate economic boom and bust cycles
    static float EconomicCycleTimer = 0.0f;
    static float CurrentCyclePhase = 0.0f; // 0-1, where 0.5 is peak
    
    EconomicCycleTimer += 1.0f; // Increment each cycle
    
    // Economic cycle period (in simulation cycles)
    float CyclePeriod = 1000.0f; // Adjust for desired cycle length
    CurrentCyclePhase = FMath::Sin((EconomicCycleTimer / CyclePeriod) * PI * 2.0f) * 0.5f + 0.5f;
    
    // Apply cycle effects to different item categories
    ApplyEconomicCycleEffects(CurrentCyclePhase);
    
    // Log significant cycle changes
    if (FMath::Fmod(EconomicCycleTimer, 100.0f) < 1.0f) // Log every 100 cycles
    {
        FString CyclePhaseText = GetEconomicCyclePhaseText(CurrentCyclePhase);
        UE_LOG(LogTemp, Log, TEXT("Economic cycle phase: %s (%.2f)"), *CyclePhaseText, CurrentCyclePhase);
    }
}

void UEconomySubsystem::ApplyEconomicCycleEffects(float CyclePhase)
{
    // Different items are affected differently by economic cycles
    TMap<FString, float> CategoryMultipliers;
    
    if (CyclePhase > 0.7f) // Economic boom
    {
        CategoryMultipliers.Add(TEXT("Luxury"), 1.5f);
        CategoryMultipliers.Add(TEXT("Tools"), 1.3f);
        CategoryMultipliers.Add(TEXT("Weapons"), 1.2f);
        CategoryMultipliers.Add(TEXT("Food"), 1.1f);
    }
    else if (CyclePhase < 0.3f) // Economic bust
    {
        CategoryMultipliers.Add(TEXT("Luxury"), 0.6f);
        CategoryMultipliers.Add(TEXT("Tools"), 0.8f);
        CategoryMultipliers.Add(TEXT("Weapons"), 0.9f);
        CategoryMultipliers.Add(TEXT("Food"), 1.0f); // Food demand stays stable
    }
    else // Normal economic conditions
    {
        CategoryMultipliers.Add(TEXT("Luxury"), 1.0f);
        CategoryMultipliers.Add(TEXT("Tools"), 1.0f);
        CategoryMultipliers.Add(TEXT("Weapons"), 1.0f);
        CategoryMultipliers.Add(TEXT("Food"), 1.0f);
    }
    
    // Apply multipliers to demand
    for (auto& RegionPair : RegionalMarkets)
    {
        FRegionMarketData& RegionMarket = RegionPair.Value;
        
        for (auto& ItemPair : RegionMarket.MarketItems)
        {
            FBasicMarketData& ItemData = ItemPair.Value;
            FString ItemCategory = DetermineItemCategory(ItemPair.Key);
            
            if (float* Multiplier = CategoryMultipliers.Find(ItemCategory))
            {
                // Gradually adjust demand based on economic cycle
                float TargetDemand = ItemData.Demand * (*Multiplier);
                ItemData.Demand = FMath::Lerp(ItemData.Demand, TargetDemand, 0.1f);
                
                UpdatePrice(RegionPair.Key, ItemPair.Key);
            }
        }
    }
}

FString UEconomySubsystem::GetEconomicCyclePhaseText(float CyclePhase)
{
    if (CyclePhase > 0.8f)
        return TEXT("Peak Boom");
    else if (CyclePhase > 0.6f)
        return TEXT("Economic Growth");
    else if (CyclePhase > 0.4f)
        return TEXT("Stable");
    else if (CyclePhase > 0.2f)
        return TEXT("Economic Decline");
    else
        return TEXT("Recession");
}

FString UEconomySubsystem::DetermineItemCategory(const FName& ItemName)
{
    FString ItemString = ItemName.ToString();
    
    // Categorize items based on name patterns
    if (ItemString.Contains(TEXT("Gold")) || ItemString.Contains(TEXT("Jewelry")) ||
        ItemString.Contains(TEXT("Silk")) || ItemString.Contains(TEXT("Spice")))
    {
        return TEXT("Luxury");
    }
    else if (ItemString.Contains(TEXT("Sword")) || ItemString.Contains(TEXT("Bow")) ||
             ItemString.Contains(TEXT("Armor")) || ItemString.Contains(TEXT("Shield")))
    {
        return TEXT("Weapons");
    }
    else if (ItemString.Contains(TEXT("Hammer")) || ItemString.Contains(TEXT("Pickaxe")) ||
             ItemString.Contains(TEXT("Tool")) || ItemString.Contains(TEXT("Anvil")))
    {
        return TEXT("Tools");
    }
    else if (ItemString.Contains(TEXT("Bread")) || ItemString.Contains(TEXT("Meat")) ||
             ItemString.Contains(TEXT("Grain")) || ItemString.Contains(TEXT("Fish")))
    {
        return TEXT("Food");
    }
    else
    {
        return TEXT("General");
    }
}

void UEconomySubsystem::ProcessPlayerEconomicImpact()
{
    // Track player's economic influence and adjust markets accordingly
    float PlayerWealthInfluence = CalculatePlayerWealthInfluence();
    float PlayerTradeInfluence = CalculatePlayerTradeInfluence();
    
    // Player's wealth affects luxury goods demand
    if (PlayerWealthInfluence > 1000.0f)
    {
        for (auto& RegionPair : RegionalMarkets)
        {
            FRegionMarketData& RegionMarket = RegionPair.Value;
            
            for (auto& ItemPair : RegionMarket.MarketItems)
            {
                if (DetermineItemCategory(ItemPair.Key) == TEXT("Luxury"))
                {
                    FBasicMarketData& ItemData = ItemPair.Value;
                    float InfluenceMultiplier = FMath::Min(2.0f, PlayerWealthInfluence / 1000.0f);
                    ItemData.Demand *= (1.0f + (InfluenceMultiplier - 1.0f) * 0.1f);
                    
                    UpdatePrice(RegionPair.Key, ItemPair.Key);
                }
            }
        }
    }
    
    // Player's trading activity affects market stability
    if (PlayerTradeInfluence > 100.0f)
    {
        UE_LOG(LogTemp, Log, TEXT("Player economic influence - Wealth: %.2f, Trade: %.2f"),
            PlayerWealthInfluence, PlayerTradeInfluence);
    }
}

float UEconomySubsystem::CalculatePlayerWealthInfluence()
{
    // This would integrate with player inventory/wealth system
    // For now, simulate based on recent trading activity
    static float SimulatedPlayerWealth = 500.0f;
    
    // Simulate wealth growth over time
    SimulatedPlayerWealth += FMath::FRandRange(-10.0f, 20.0f);
    SimulatedPlayerWealth = FMath::Max(0.0f, SimulatedPlayerWealth);
    
    return SimulatedPlayerWealth;
}

float UEconomySubsystem::CalculatePlayerTradeInfluence()
{
    // Track player's recent trading volume
    static float RecentTradeVolume = 0.0f;
    
    // Simulate trading activity
    RecentTradeVolume += FMath::FRandRange(0.0f, 50.0f);
    RecentTradeVolume *= 0.95f; // Decay over time
    
    return RecentTradeVolume;
}

void UEconomySubsystem::ProcessFactionEconomicEffects()
{
    // Different factions affect different aspects of the economy
    ProcessFactionTradeBonus(TEXT("MerchantGuild"), TEXT("Luxury"), 1.2f);
    ProcessFactionTradeBonus(TEXT("IronBrotherhood"), TEXT("Weapons"), 1.3f);
    ProcessFactionTradeBonus(TEXT("Farmers"), TEXT("Food"), 1.1f);
    
    // Faction conflicts disrupt trade
    ProcessFactionConflictEffects();
}

void UEconomySubsystem::ProcessFactionTradeBonus(const FString& FactionName, const FString& Category, float Bonus)
{
    // This would integrate with faction reputation system
    float FactionInfluence = GetFactionEconomicInfluence(FactionName);
    
    if (FactionInfluence > 0.5f) // Faction has significant influence
    {
        for (auto& RegionPair : RegionalMarkets)
        {
            FRegionMarketData& RegionMarket = RegionPair.Value;
            
            for (auto& ItemPair : RegionMarket.MarketItems)
            {
                if (DetermineItemCategory(ItemPair.Key) == Category)
                {
                    FBasicMarketData& ItemData = ItemPair.Value;
                    float EffectiveBonus = 1.0f + ((Bonus - 1.0f) * FactionInfluence);
                    ItemData.Supply *= EffectiveBonus;
                    
                    UpdatePrice(RegionPair.Key, ItemPair.Key);
                }
            }
        }
    }
}

float UEconomySubsystem::GetFactionEconomicInfluence(const FString& FactionName)
{
    // Simulate faction influence (would integrate with faction system)
    static TMap<FString, float> FactionInfluences;
    
    if (!FactionInfluences.Contains(FactionName))
    {
        FactionInfluences.Add(FactionName, FMath::FRandRange(0.3f, 0.8f));
    }
    
    // Slowly change influence over time
    float& Influence = FactionInfluences[FactionName];
    Influence += FMath::FRandRange(-0.01f, 0.01f);
    Influence = FMath::Clamp(Influence, 0.0f, 1.0f);
    
    return Influence;
}

void UEconomySubsystem::ProcessFactionConflictEffects()
{
    // Simulate faction conflicts disrupting trade routes
    if (FMath::FRand() < 0.05f) // 5% chance of conflict affecting trade
    {
        // Randomly select a trade route to disrupt
        if (TradeRoutes.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, TradeRoutes.Num() - 1);
            FTradeRouteData& Route = TradeRoutes[RandomIndex];
            
            // Reduce efficiency due to conflict
            Route.Efficiency *= 0.8f;
            Route.Efficiency = FMath::Max(0.1f, Route.Efficiency);
            
            UE_LOG(LogTemp, Warning, TEXT("Faction conflict disrupts trade route from %s to %s - Efficiency: %.2f"),
                *Route.StartRegion.ToString(), *Route.EndRegion.ToString(), Route.Efficiency);
        }
    }
}

void UEconomySubsystem::ProcessSeasonalEconomicChanges()
{
    // Get current season (would integrate with weather/time system)
    FString CurrentSeason = GetCurrentSeason();
    
    ApplySeasonalSupplyChanges(CurrentSeason);
    ApplySeasonalDemandChanges(CurrentSeason);
}

FString UEconomySubsystem::GetCurrentSeason()
{
    // Simulate seasonal changes based on game time
    if (UWorld* World = GetWorld())
    {
        float GameTime = World->GetTimeSeconds();
        float SeasonCycle = FMath::Fmod(GameTime / 3600.0f, 4.0f); // 4 seasons per hour for testing
        
        if (SeasonCycle < 1.0f)
            return TEXT("Spring");
        else if (SeasonCycle < 2.0f)
            return TEXT("Summer");
        else if (SeasonCycle < 3.0f)
            return TEXT("Autumn");
        else
            return TEXT("Winter");
    }
    
    return TEXT("Spring");
}

void UEconomySubsystem::ApplySeasonalSupplyChanges(const FString& Season)
{
    TMap<FString, float> SeasonalSupplyMultipliers;
    
    if (Season == TEXT("Spring"))
    {
        SeasonalSupplyMultipliers.Add(TEXT("Food"), 1.2f);
        SeasonalSupplyMultipliers.Add(TEXT("Tools"), 1.1f);
    }
    else if (Season == TEXT("Summer"))
    {
        SeasonalSupplyMultipliers.Add(TEXT("Food"), 1.5f);
        SeasonalSupplyMultipliers.Add(TEXT("Luxury"), 1.1f);
    }
    else if (Season == TEXT("Autumn"))
    {
        SeasonalSupplyMultipliers.Add(TEXT("Food"), 1.3f);
        SeasonalSupplyMultipliers.Add(TEXT("Weapons"), 1.1f);
    }
    else if (Season == TEXT("Winter"))
    {
        SeasonalSupplyMultipliers.Add(TEXT("Food"), 0.7f);
        SeasonalSupplyMultipliers.Add(TEXT("Tools"), 0.9f);
    }
    
    ApplySupplyMultipliers(SeasonalSupplyMultipliers);
}

void UEconomySubsystem::ApplySeasonalDemandChanges(const FString& Season)
{
    TMap<FString, float> SeasonalDemandMultipliers;
    
    if (Season == TEXT("Winter"))
    {
        SeasonalDemandMultipliers.Add(TEXT("Food"), 1.4f);
        SeasonalDemandMultipliers.Add(TEXT("Weapons"), 1.2f); // More conflicts in harsh times
        SeasonalDemandMultipliers.Add(TEXT("Luxury"), 0.8f);
    }
    else if (Season == TEXT("Summer"))
    {
        SeasonalDemandMultipliers.Add(TEXT("Luxury"), 1.3f);
        SeasonalDemandMultipliers.Add(TEXT("Tools"), 1.2f);
    }
    
    ApplyDemandMultipliers(SeasonalDemandMultipliers);
}

void UEconomySubsystem::ApplySupplyMultipliers(const TMap<FString, float>& Multipliers)
{
    for (auto& RegionPair : RegionalMarkets)
    {
        FRegionMarketData& RegionMarket = RegionPair.Value;
        
        for (auto& ItemPair : RegionMarket.MarketItems)
        {
            FString Category = DetermineItemCategory(ItemPair.Key);
            
            if (const float* Multiplier = Multipliers.Find(Category))
            {
                FBasicMarketData& ItemData = ItemPair.Value;
                float TargetSupply = ItemData.Supply * (*Multiplier);
                ItemData.Supply = FMath::Lerp(ItemData.Supply, TargetSupply, 0.05f); // Gradual change
                
                UpdatePrice(RegionPair.Key, ItemPair.Key);
            }
        }
    }
}

void UEconomySubsystem::ApplyDemandMultipliers(const TMap<FString, float>& Multipliers)
{
    for (auto& RegionPair : RegionalMarkets)
    {
        FRegionMarketData& RegionMarket = RegionPair.Value;
        
        for (auto& ItemPair : RegionMarket.MarketItems)
        {
            FString Category = DetermineItemCategory(ItemPair.Key);
            
            if (const float* Multiplier = Multipliers.Find(Category))
            {
                FBasicMarketData& ItemData = ItemPair.Value;
                float TargetDemand = ItemData.Demand * (*Multiplier);
                ItemData.Demand = FMath::Lerp(ItemData.Demand, TargetDemand, 0.05f); // Gradual change
                
                UpdatePrice(RegionPair.Key, ItemPair.Key);
            }
        }
    }
}

void UEconomySubsystem::ProcessRandomEconomicEvents()
{
    // Random economic events that can shake up the market
    if (FMath::FRand() < 0.02f) // 2% chance per cycle
    {
        TriggerRandomEconomicEvent();
    }
}

void UEconomySubsystem::TriggerRandomEconomicEvent()
{
    TArray<FString> EventTypes = {
        TEXT("ResourceDiscovery"),
        TEXT("TradeEmbargo"),
        TEXT("TechnologicalAdvancement"),
        TEXT("NaturalDisaster"),
        TEXT("PirateAttack"),
        TEXT("MerchantFestival"),
        TEXT("CropBlight"),
        TEXT("MineBoom")
    };
    
    FString EventType = EventTypes[FMath::RandRange(0, EventTypes.Num() - 1)];
    ExecuteRandomEconomicEvent(EventType);
}

void UEconomySubsystem::ExecuteRandomEconomicEvent(const FString& EventType)
{
    if (EventType == TEXT("ResourceDiscovery"))
    {
        // New resource discovered - increase supply of random item
        TArray<FName> AllItems = GetAllTradeableItems();
        if (AllItems.Num() > 0)
        {
            FName RandomItem = AllItems[FMath::RandRange(0, AllItems.Num() - 1)];
            TArray<FName> AllRegions = GetAllRegions();
            if (AllRegions.Num() > 0)
            {
                FName RandomRegion = AllRegions[FMath::RandRange(0, AllRegions.Num() - 1)];
                UpdateSupply(RandomRegion, RandomItem, FMath::FRandRange(100.0f, 500.0f));
                
                UE_LOG(LogTemp, Warning, TEXT("Economic Event: Resource discovery increases %s supply in %s"),
                    *RandomItem.ToString(), *RandomRegion.ToString());
            }
        }
    }
    else if (EventType == TEXT("TradeEmbargo"))
    {
        // Trade embargo - reduce trade route efficiency
        if (TradeRoutes.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, TradeRoutes.Num() - 1);
            TradeRoutes[RandomIndex].Efficiency *= 0.5f;
            
            UE_LOG(LogTemp, Warning, TEXT("Economic Event: Trade embargo affects route from %s to %s"),
                *TradeRoutes[RandomIndex].StartRegion.ToString(),
                *TradeRoutes[RandomIndex].EndRegion.ToString());
        }
    }
    else if (EventType == TEXT("MerchantFestival"))
    {
        // Merchant festival - increase demand for luxury goods
        for (auto& RegionPair : RegionalMarkets)
        {
            FRegionMarketData& RegionMarket = RegionPair.Value;
        
            for (auto& ItemPair : RegionMarket.MarketItems)
            {
                if (DetermineItemCategory(ItemPair.Key) == TEXT("Luxury"))
                {
                    ItemPair.Value.Demand *= 1.5f;
                    UpdatePrice(RegionPair.Key, ItemPair.Key);
                }
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("Economic Event: Merchant festival increases luxury goods demand"));
    }
    else if (EventType == TEXT("CropBlight"))
    {
        // Crop blight - reduce food supply
        for (auto& RegionPair : RegionalMarkets)
        {
            FRegionMarketData& RegionMarket = RegionPair.Value;
            
            for (auto& ItemPair : RegionMarket.MarketItems)
            {
                if (DetermineItemCategory(ItemPair.Key) == TEXT("Food"))
                {
                    ItemPair.Value.Supply *= 0.6f;
                    UpdatePrice(RegionPair.Key, ItemPair.Key);
                }
            }
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Economic Event: Crop blight reduces food supply across all regions"));
    }
}

TArray<FName> UEconomySubsystem::GetAllTradeableItems() const
{
    TSet<FName> UniqueItems;
    
    for (const auto& RegionPair : RegionalMarkets)
    {
        for (const auto& ItemPair : RegionPair.Value.MarketItems)
        {
            UniqueItems.Add(ItemPair.Key);
        }
    }
    
    return UniqueItems.Array();
}

TArray<FName> UEconomySubsystem::GetAllRegions() const
{
    TArray<FName> Regions;
    RegionalMarkets.GetKeys(Regions);
    return Regions;
}

// Enhanced market analysis functions
float UEconomySubsystem::GetMarketVolatility(const FName& Region) const
{
    if (const FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        float TotalVolatility = 0.0f;
        int32 ItemCount = 0;
        
        for (const auto& ItemPair : RegionMarket->MarketItems)
        {
            const FBasicMarketData& ItemData = ItemPair.Value;
            
            // Calculate volatility as price deviation from base price
            float PriceDeviation = FMath::Abs(ItemData.CurrentPrice - ItemData.BasePrice) / ItemData.BasePrice;
            TotalVolatility += PriceDeviation;
            ItemCount++;
        }
        
        return ItemCount > 0 ? TotalVolatility / ItemCount : 0.0f;
    }
    
    return 0.0f;
}

float UEconomySubsystem::GetRegionProsperity(const FName& Region) const
{
    if (const FRegionMarketData* RegionMarket = RegionalMarkets.Find(Region))
    {
        float TotalValue = 0.0f;
        int32 ItemCount = 0;
        
        for (const auto& ItemPair : RegionMarket->MarketItems)
        {
            const FBasicMarketData& ItemData = ItemPair.Value;
            
            // Prosperity = total market value
            float ItemValue = (ItemData.Supply + ItemData.Demand) * ItemData.CurrentPrice;
            TotalValue += ItemValue;
            ItemCount++;
        }
        
        return ItemCount > 0 ? TotalValue / ItemCount : 0.0f;
    }
    
    return 0.0f;
}

TArray<FName> UEconomySubsystem::GetMostProfitableTradeRoutes() const
{
    TArray<FName> ProfitableRoutes;
    
    for (const auto& RegionPair1 : RegionalMarkets)
    {
        for (const auto& RegionPair2 : RegionalMarkets)
        {
            if (RegionPair1.Key != RegionPair2.Key)
            {
                float ProfitPotential = CalculateRouteProfitPotential(RegionPair1.Key, RegionPair2.Key);
                
                if (ProfitPotential > 100.0f) // Threshold for profitable routes
                {
                    FString RouteString = FString::Printf(TEXT("%s_to_%s"),
                        *RegionPair1.Key.ToString(), *RegionPair2.Key.ToString());
                    ProfitableRoutes.Add(FName(*RouteString));
                }
            }
        }
    }
    
    return ProfitableRoutes;
}

float UEconomySubsystem::CalculateRouteProfitPotential(const FName& StartRegion, const FName& EndRegion) const
{
    const FRegionMarketData* StartMarket = RegionalMarkets.Find(StartRegion);
    const FRegionMarketData* EndMarket = RegionalMarkets.Find(EndRegion);
    if (!StartMarket || !EndMarket)
        return 0.0f;
    float TotalProfit = 0.0f;
    for (const auto& StartItemPair : StartMarket->MarketItems)
    {
        if (const FBasicMarketData* EndItemData = EndMarket->MarketItems.Find(StartItemPair.Key))
        {
            const FBasicMarketData& StartItemData = StartItemPair.Value;
            float PriceDifference = EndItemData->CurrentPrice - StartItemData.CurrentPrice;
            if (PriceDifference > 0.0f)
            {
                float PotentialProfit = PriceDifference * FMath::Min(StartItemData.Supply, EndItemData->Demand);
                TotalProfit += PotentialProfit;
            }
        }
    }
    return TotalProfit;
}