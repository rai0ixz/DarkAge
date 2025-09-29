#include "Core/AdvancedMarketSimulationSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Core/NPCEcosystemSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedMarketSimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeMarketData();
    InitializeTradingRoutes();
    InitializeMarketEvents();
    
    MarketUpdateTimer = 0.0f;
    EventCheckTimer = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedMarketSimulationSubsystem initialized with %d markets"), Markets.Num());
}

void UAdvancedMarketSimulationSubsystem::Tick(float DeltaTime)
{
    if (!bMarketEnabled)
    {
        return;
    }

    // UGameInstanceSubsystem doesn't have Tick method, remove this call
    
    MarketUpdateTimer += DeltaTime;
    EventCheckTimer += DeltaTime;
    
    // Update markets every 10 seconds
    if (MarketUpdateTimer >= 10.0f)
    {
        UpdateAllMarkets();
        MarketUpdateTimer = 0.0f;
    }
    
    // Check for market events every 30 seconds
    if (EventCheckTimer >= 30.0f)
    {
        ProcessMarketEvents();
        EventCheckTimer = 0.0f;
    }
    
    // Update trade routes continuously
    UpdateTradeRoutes(DeltaTime);
}

void UAdvancedMarketSimulationSubsystem::InitializeMarketData()
{
    // Create major trading hubs
    CreateMarket(TEXT("Millhaven"), FVector(0, 0, 0), EMarketType::Village, 1000.0f);
    CreateMarket(TEXT("Oakstead"), FVector(5000, 0, 0), EMarketType::Town, 2500.0f);
    CreateMarket(TEXT("Ironhold"), FVector(-3000, 4000, 0), EMarketType::City, 5000.0f);
    CreateMarket(TEXT("Riverside"), FVector(2000, -3000, 0), EMarketType::Village, 800.0f);
    CreateMarket(TEXT("Goldport"), FVector(8000, 6000, 0), EMarketType::Port, 4000.0f);
    
    // Initialize commodity base prices
    CommodityBasePrices.Add(TEXT("Grain"), 5.0f);
    CommodityBasePrices.Add(TEXT("Bread"), 8.0f);
    CommodityBasePrices.Add(TEXT("Iron"), 15.0f);
    CommodityBasePrices.Add(TEXT("Wood"), 3.0f);
    CommodityBasePrices.Add(TEXT("Stone"), 4.0f);
    CommodityBasePrices.Add(TEXT("Cloth"), 12.0f);
    CommodityBasePrices.Add(TEXT("Ale"), 6.0f);
    CommodityBasePrices.Add(TEXT("Meat"), 10.0f);
    CommodityBasePrices.Add(TEXT("Fish"), 7.0f);
    CommodityBasePrices.Add(TEXT("Salt"), 20.0f);
    CommodityBasePrices.Add(TEXT("Spices"), 50.0f);
    CommodityBasePrices.Add(TEXT("Weapons"), 100.0f);
    CommodityBasePrices.Add(TEXT("Tools"), 25.0f);
    CommodityBasePrices.Add(TEXT("Jewelry"), 200.0f);
    CommodityBasePrices.Add(TEXT("Books"), 75.0f);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d commodity base prices"), CommodityBasePrices.Num());
}

void UAdvancedMarketSimulationSubsystem::CreateMarket(const FString& MarketName, const FVector& Location, EMarketType Type, float Wealth)
{
    FMarketData NewMarket;
    NewMarket.MarketName = MarketName;
    NewMarket.Location = Location;
    NewMarket.MarketType = Type;
    NewMarket.Wealth = Wealth;
    NewMarket.Population = GetPopulationForMarketType(Type);
    NewMarket.TradingVolume = 0.0f;
    NewMarket.MarketStability = 1.0f;
    
    // Initialize commodity supplies and demands based on market type
    InitializeMarketCommodities(NewMarket);
    
    Markets.Add(MarketName, NewMarket);
    
    UE_LOG(LogTemp, Log, TEXT("Created market: %s (Type: %d, Wealth: %f, Population: %d)"), 
        *MarketName, (int32)Type, Wealth, NewMarket.Population);
}

void UAdvancedMarketSimulationSubsystem::InitializeMarketCommodities(FMarketData& Market)
{
    for (const auto& CommodityPair : CommodityBasePrices)
    {
        const FString& CommodityName = CommodityPair.Key;
        
        FCommodityData CommodityData;
        CommodityData.BasePrice = CommodityPair.Value;
        CommodityData.CurrentPrice = CommodityPair.Value;
        
        // Set supply and demand based on market type and commodity
        SetCommoditySupplyDemand(Market, CommodityName, CommodityData);
        
        Market.Commodities.Add(CommodityName, CommodityData);
    }
}

void UAdvancedMarketSimulationSubsystem::SetCommoditySupplyDemand(const FMarketData& Market, const FString& CommodityName, FCommodityData& CommodityData)
{
    // Base supply and demand
    int32 BaseSupply = 100;
    int32 BaseDemand = 100;
    
    // Modify based on market type
    float MarketMultiplier = 1.0f;
    switch (Market.MarketType)
    {
    case EMarketType::Village:
        MarketMultiplier = 0.5f;
        break;
    case EMarketType::Town:
        MarketMultiplier = 1.0f;
        break;
    case EMarketType::City:
        MarketMultiplier = 2.0f;
        break;
    case EMarketType::Port:
        MarketMultiplier = 1.5f;
        break;
    }
    
    // Modify based on commodity specialization
    if (Market.MarketName == TEXT("Millhaven") && CommodityName == TEXT("Grain"))
    {
        BaseSupply *= 3; // Millhaven produces lots of grain
        BaseDemand *= 0.5f;
    }
    else if (Market.MarketName == TEXT("Ironhold") && CommodityName == TEXT("Iron"))
    {
        BaseSupply *= 4; // Ironhold is a mining town
        BaseDemand *= 0.3f;
    }
    else if (Market.MarketName == TEXT("Goldport") && (CommodityName == TEXT("Spices") || CommodityName == TEXT("Fish")))
    {
        BaseSupply *= 2; // Port has access to exotic goods and fish
        BaseDemand *= 1.5f;
    }
    else if (Market.MarketType == EMarketType::City && (CommodityName == TEXT("Weapons") || CommodityName == TEXT("Tools")))
    {
        BaseDemand *= 2; // Cities have high demand for manufactured goods
    }
    
    CommodityData.Supply = FMath::Max(1, (int32)(BaseSupply * MarketMultiplier));
    CommodityData.Demand = FMath::Max(1, (int32)(BaseDemand * MarketMultiplier));
    
    // Add some randomness
    CommodityData.Supply += FMath::RandRange(-20, 20);
    CommodityData.Demand += FMath::RandRange(-20, 20);
    
    CommodityData.Supply = FMath::Max(1, CommodityData.Supply);
    CommodityData.Demand = FMath::Max(1, CommodityData.Demand);
}

int32 UAdvancedMarketSimulationSubsystem::GetPopulationForMarketType(EMarketType Type)
{
    switch (Type)
    {
    case EMarketType::Village:
        return FMath::RandRange(200, 500);
    case EMarketType::Town:
        return FMath::RandRange(800, 1500);
    case EMarketType::City:
        return FMath::RandRange(2000, 5000);
    case EMarketType::Port:
        return FMath::RandRange(1200, 2500);
    default:
        return 500;
    }
}

void UAdvancedMarketSimulationSubsystem::InitializeTradingRoutes()
{
    // Create major trade routes between markets
    CreateTradeRoute(TEXT("Millhaven"), TEXT("Oakstead"), 500.0f, 0.9f);
    CreateTradeRoute(TEXT("Oakstead"), TEXT("Ironhold"), 800.0f, 0.8f);
    CreateTradeRoute(TEXT("Ironhold"), TEXT("Goldport"), 1200.0f, 0.7f);
    CreateTradeRoute(TEXT("Riverside"), TEXT("Millhaven"), 400.0f, 0.95f);
    CreateTradeRoute(TEXT("Goldport"), TEXT("Oakstead"), 1000.0f, 0.75f);
    CreateTradeRoute(TEXT("Riverside"), TEXT("Goldport"), 900.0f, 0.8f);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d trade routes"), TradeRoutes.Num());
}

void UAdvancedMarketSimulationSubsystem::CreateTradeRoute(const FString& Origin, const FString& Destination, float Distance, float Efficiency)
{
    FTradeRoute NewRoute;
    NewRoute.OriginMarket = Origin;
    NewRoute.DestinationMarket = Destination;
    NewRoute.Distance = Distance;
    NewRoute.BaseEfficiency = Efficiency;
    NewRoute.CurrentEfficiency = Efficiency;
    NewRoute.TradeVolume = 0.0f;
    NewRoute.bIsActive = true;
    
    FString RouteKey = Origin + TEXT("_to_") + Destination;
    TradeRoutes.Add(RouteKey, NewRoute);
    
    // Create reverse route
    FTradeRoute ReverseRoute = NewRoute;
    ReverseRoute.OriginMarket = Destination;
    ReverseRoute.DestinationMarket = Origin;
    
    FString ReverseRouteKey = Destination + TEXT("_to_") + Origin;
    TradeRoutes.Add(ReverseRouteKey, ReverseRoute);
}

void UAdvancedMarketSimulationSubsystem::InitializeMarketEvents()
{
    // Define possible market events
    MarketEventTypes.Add(TEXT("Bumper Harvest"));
    MarketEventTypes.Add(TEXT("Crop Failure"));
    MarketEventTypes.Add(TEXT("Bandit Raids"));
    MarketEventTypes.Add(TEXT("Trade Embargo"));
    MarketEventTypes.Add(TEXT("Festival"));
    MarketEventTypes.Add(TEXT("Plague"));
    MarketEventTypes.Add(TEXT("War"));
    MarketEventTypes.Add(TEXT("Discovery"));
    MarketEventTypes.Add(TEXT("Merchant Caravan"));
    MarketEventTypes.Add(TEXT("Natural Disaster"));
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d market event types"), MarketEventTypes.Num());
}

void UAdvancedMarketSimulationSubsystem::UpdateAllMarkets()
{
    for (auto& MarketPair : Markets)
    {
        UpdateMarketPrices(MarketPair.Value);
        UpdateMarketSupplyDemand(MarketPair.Value);
        ProcessMarketTrade(MarketPair.Value);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Updated all %d markets"), Markets.Num());
}

void UAdvancedMarketSimulationSubsystem::UpdateMarketPrices(FMarketData& Market)
{
    for (auto& CommodityPair : Market.Commodities)
    {
        FCommodityData& Commodity = CommodityPair.Value;
        
        // Calculate price based on supply and demand
        float SupplyDemandRatio = (float)Commodity.Supply / (float)Commodity.Demand;
        float PriceMultiplier = 1.0f / SupplyDemandRatio;
        
        // Apply market stability factor
        PriceMultiplier *= Market.MarketStability;
        
        // Smooth price changes to avoid wild fluctuations
        float TargetPrice = Commodity.BasePrice * PriceMultiplier;
        Commodity.CurrentPrice = FMath::Lerp(Commodity.CurrentPrice, TargetPrice, 0.1f);
        
        // Clamp prices to reasonable bounds
        Commodity.CurrentPrice = FMath::Clamp(Commodity.CurrentPrice, 
            Commodity.BasePrice * 0.2f, Commodity.BasePrice * 5.0f);
    }
}

void UAdvancedMarketSimulationSubsystem::UpdateMarketSupplyDemand(FMarketData& Market)
{
    for (auto& CommodityPair : Market.Commodities)
    {
        FCommodityData& Commodity = CommodityPair.Value;
        
        // Natural supply and demand fluctuations
        int32 SupplyChange = FMath::RandRange(-5, 5);
        int32 DemandChange = FMath::RandRange(-5, 5);
        
        // Population affects demand
        float PopulationFactor = Market.Population / 1000.0f;
        DemandChange += (int32)(PopulationFactor * FMath::RandRange(-2, 2));
        
        // Wealth affects demand for luxury goods
        if (CommodityPair.Key == TEXT("Spices") || CommodityPair.Key == TEXT("Jewelry") || CommodityPair.Key == TEXT("Books"))
        {
            float WealthFactor = Market.Wealth / 1000.0f;
            DemandChange += (int32)(WealthFactor * FMath::RandRange(0, 3));
        }
        
        Commodity.Supply = FMath::Max(1, Commodity.Supply + SupplyChange);
        Commodity.Demand = FMath::Max(1, Commodity.Demand + DemandChange);
    }
}

void UAdvancedMarketSimulationSubsystem::ProcessMarketTrade(FMarketData& Market)
{
    float TotalTradeValue = 0.0f;
    
    for (const auto& CommodityPair : Market.Commodities)
    {
        const FCommodityData& Commodity = CommodityPair.Value;
        
        // Calculate trade volume based on supply, demand, and price
        int32 TradeVolume = FMath::Min(Commodity.Supply, Commodity.Demand) / 4;
        float TradeValue = TradeVolume * Commodity.CurrentPrice;
        
        TotalTradeValue += TradeValue;
    }
    
    Market.TradingVolume = TotalTradeValue;
    
    // Trading volume affects market wealth
    Market.Wealth += TotalTradeValue * 0.01f;
    
    UE_LOG(LogTemp, Log, TEXT("Market %s processed %f gold in trade"), *Market.MarketName, TotalTradeValue);
}

void UAdvancedMarketSimulationSubsystem::UpdateTradeRoutes(float DeltaTime)
{
    for (auto& RoutePair : TradeRoutes)
    {
        FTradeRoute& Route = RoutePair.Value;
        
        if (!Route.bIsActive)
            continue;
            
        // Process trade between markets
        ProcessTradeRoute(Route);
        
        // Update route efficiency based on various factors
        UpdateRouteEfficiency(Route);
    }
}

void UAdvancedMarketSimulationSubsystem::ProcessTradeRoute(FTradeRoute& Route)
{
    FMarketData* OriginMarket = Markets.Find(Route.OriginMarket);
    FMarketData* DestinationMarket = Markets.Find(Route.DestinationMarket);
    
    if (!OriginMarket || !DestinationMarket)
        return;
        
    // Find profitable trade opportunities
    for (const auto& CommodityPair : OriginMarket->Commodities)
    {
        const FString& CommodityName = CommodityPair.Key;
        const FCommodityData& OriginCommodity = CommodityPair.Value;
        
        if (FCommodityData* DestCommodity = DestinationMarket->Commodities.Find(CommodityName))
        {
            // Check if it's profitable to trade
            float PriceDifference = DestCommodity->CurrentPrice - OriginCommodity.CurrentPrice;
            float TransportCost = Route.Distance * 0.01f; // Simple transport cost model
            
            if (PriceDifference > TransportCost && OriginCommodity.Supply > 10)
            {
                // Execute trade
                int32 TradeAmount = FMath::Min(5, OriginCommodity.Supply / 4);
                
                // Update supplies
                OriginMarket->Commodities[CommodityName].Supply -= TradeAmount;
                DestinationMarket->Commodities[CommodityName].Supply += TradeAmount;
                
                // Update trade volume
                float TradeValue = TradeAmount * OriginCommodity.CurrentPrice;
                Route.TradeVolume += TradeValue;
                
                UE_LOG(LogTemp, Log, TEXT("Trade: %d %s from %s to %s (Value: %f)"), 
                    TradeAmount, *CommodityName, *Route.OriginMarket, *Route.DestinationMarket, TradeValue);
            }
        }
    }
}

void UAdvancedMarketSimulationSubsystem::UpdateRouteEfficiency(FTradeRoute& Route)
{
    // Base efficiency decay over time
    Route.CurrentEfficiency = FMath::Max(0.1f, Route.CurrentEfficiency - 0.001f);
    
    // Efficiency improves with trade volume
    if (Route.TradeVolume > 100.0f)
    {
        Route.CurrentEfficiency = FMath::Min(Route.BaseEfficiency, Route.CurrentEfficiency + 0.01f);
    }
    
    // Random events can affect efficiency
    if (FMath::RandRange(0, 1000) < 1) // 0.1% chance per update
    {
        if (FMath::RandBool())
        {
            Route.CurrentEfficiency *= 0.8f; // Bandit attack or bad weather
            UE_LOG(LogTemp, Warning, TEXT("Trade route %s to %s disrupted!"), *Route.OriginMarket, *Route.DestinationMarket);
        }
        else
        {
            Route.CurrentEfficiency = FMath::Min(Route.BaseEfficiency, Route.CurrentEfficiency * 1.2f); // Good conditions
            UE_LOG(LogTemp, Log, TEXT("Trade route %s to %s improved!"), *Route.OriginMarket, *Route.DestinationMarket);
        }
    }
}

void UAdvancedMarketSimulationSubsystem::ProcessMarketEvents()
{
    // Random chance for market events
    if (FMath::RandRange(0, 100) < 10) // 10% chance per check
    {
        TriggerRandomMarketEvent();
    }
}

void UAdvancedMarketSimulationSubsystem::TriggerRandomMarketEvent()
{
    if (MarketEventTypes.Num() == 0 || Markets.Num() == 0)
        return;
        
    // Select random event and market
    FString EventType = MarketEventTypes[FMath::RandRange(0, MarketEventTypes.Num() - 1)];
    
    TArray<FString> MarketNames;
    Markets.GetKeys(MarketNames);
    FString TargetMarket = MarketNames[FMath::RandRange(0, MarketNames.Num() - 1)];
    
    ApplyMarketEvent(EventType, TargetMarket);
}

void UAdvancedMarketSimulationSubsystem::ApplyMarketEvent(const FString& EventType, const FString& MarketName)
{
    FMarketData* Market = Markets.Find(MarketName);
    if (!Market)
        return;
        
    UE_LOG(LogTemp, Warning, TEXT("Market Event: %s affects %s"), *EventType, *MarketName);
    
    if (EventType == TEXT("Bumper Harvest"))
    {
        // Increase food supply, decrease food prices
        if (FCommodityData* Grain = Market->Commodities.Find(TEXT("Grain")))
        {
            Grain->Supply += 50;
        }
        if (FCommodityData* Bread = Market->Commodities.Find(TEXT("Bread")))
        {
            Bread->Supply += 30;
        }
        Market->MarketStability += 0.1f;
    }
    else if (EventType == TEXT("Crop Failure"))
    {
        // Decrease food supply, increase food prices
        if (FCommodityData* Grain = Market->Commodities.Find(TEXT("Grain")))
        {
            Grain->Supply = FMath::Max(1, Grain->Supply - 40);
        }
        if (FCommodityData* Bread = Market->Commodities.Find(TEXT("Bread")))
        {
            Bread->Supply = FMath::Max(1, Bread->Supply - 25);
        }
        Market->MarketStability -= 0.2f;
    }
    else if (EventType == TEXT("Bandit Raids"))
    {
        // Disrupt trade routes, increase weapon demand
        for (auto& RoutePair : TradeRoutes)
        {
            FTradeRoute& Route = RoutePair.Value;
            if (Route.OriginMarket == MarketName || Route.DestinationMarket == MarketName)
            {
                Route.CurrentEfficiency *= 0.6f;
            }
        }
        if (FCommodityData* Weapons = Market->Commodities.Find(TEXT("Weapons")))
        {
            Weapons->Demand += 20;
        }
        Market->MarketStability -= 0.15f;
    }
    else if (EventType == TEXT("Festival"))
    {
        // Increase demand for luxury goods and ale
        if (FCommodityData* Ale = Market->Commodities.Find(TEXT("Ale")))
        {
            Ale->Demand += 25;
        }
        if (FCommodityData* Spices = Market->Commodities.Find(TEXT("Spices")))
        {
            Spices->Demand += 15;
        }
        Market->Wealth += 100.0f;
        Market->MarketStability += 0.05f;
    }
    else if (EventType == TEXT("Discovery"))
    {
        // New resource discovered, increase supply of random commodity
        TArray<FString> CommodityNames;
        Market->Commodities.GetKeys(CommodityNames);
        if (CommodityNames.Num() > 0)
        {
            FString RandomCommodity = CommodityNames[FMath::RandRange(0, CommodityNames.Num() - 1)];
            Market->Commodities[RandomCommodity].Supply += 40;
            UE_LOG(LogTemp, Log, TEXT("Discovery increases %s supply in %s"), *RandomCommodity, *MarketName);
        }
        Market->Wealth += 200.0f;
    }
    
    // Clamp market stability
    Market->MarketStability = FMath::Clamp(Market->MarketStability, 0.1f, 2.0f);
    
    // Broadcast event
    OnMarketEventTriggered.Broadcast(EventType, MarketName);
}

// Query Functions
float UAdvancedMarketSimulationSubsystem::GetCommodityPrice(const FString& MarketName, const FString& CommodityName) const
{
    if (const FMarketData* Market = Markets.Find(MarketName))
    {
        if (const FCommodityData* Commodity = Market->Commodities.Find(CommodityName))
        {
            return Commodity->CurrentPrice;
        }
    }
    
    // Return base price if market/commodity not found
    if (const float* BasePrice = CommodityBasePrices.Find(CommodityName))
    {
        return *BasePrice;
    }
    
    return 10.0f; // Default price
}

int32 UAdvancedMarketSimulationSubsystem::GetCommoditySupply(const FString& MarketName, const FString& CommodityName) const
{
    if (const FMarketData* Market = Markets.Find(MarketName))
    {
        if (const FCommodityData* Commodity = Market->Commodities.Find(CommodityName))
        {
            return Commodity->Supply;
        }
    }
    return 0;
}

int32 UAdvancedMarketSimulationSubsystem::GetCommodityDemand(const FString& MarketName, const FString& CommodityName) const
{
    if (const FMarketData* Market = Markets.Find(MarketName))
    {
        if (const FCommodityData* Commodity = Market->Commodities.Find(CommodityName))
        {
            return Commodity->Demand;
        }
    }
    return 0;
}

TArray<FString> UAdvancedMarketSimulationSubsystem::GetAllMarkets() const
{
    TArray<FString> MarketNames;
    Markets.GetKeys(MarketNames);
    return MarketNames;
}

TArray<FString> UAdvancedMarketSimulationSubsystem::GetAllCommodities() const
{
    TArray<FString> CommodityNames;
    CommodityBasePrices.GetKeys(CommodityNames);
    return CommodityNames;
}

float UAdvancedMarketSimulationSubsystem::GetMarketWealth(const FString& MarketName) const
{
    if (const FMarketData* Market = Markets.Find(MarketName))
    {
        return Market->Wealth;
    }
    return 0.0f;
}

float UAdvancedMarketSimulationSubsystem::GetTradeRouteEfficiency(const FString& Origin, const FString& Destination) const
{
    FString RouteKey = Origin + TEXT("_to_") + Destination;
    if (const FTradeRoute* Route = TradeRoutes.Find(RouteKey))
    {
        return Route->CurrentEfficiency;
    }
    return 0.0f;
}

void UAdvancedMarketSimulationSubsystem::TriggerMarketEvent(const FString& EventType, const FString& MarketName)
{
    ApplyMarketEvent(EventType, MarketName);
}