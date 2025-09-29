// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/EconomySystem.h"

UEconomySystem::UEconomySystem()
	: TimeSinceLastUpdate(0.0f)
{
}

void UEconomySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize exchange rates
	TMap<ECurrencyType, float> CrownRates;
	CrownRates.Add(ECurrencyType::Crowns, 1.0f);
	CrownRates.Add(ECurrencyType::TradeBars, 0.5f);
	CrownRates.Add(ECurrencyType::LocalScrip, 0.2f);
	ExchangeRates.Add(ECurrencyType::Crowns, CrownRates);
	
	TMap<ECurrencyType, float> TradeBarRates;
	TradeBarRates.Add(ECurrencyType::Crowns, 2.0f);
	TradeBarRates.Add(ECurrencyType::TradeBars, 1.0f);
	TradeBarRates.Add(ECurrencyType::LocalScrip, 0.4f);
	ExchangeRates.Add(ECurrencyType::TradeBars, TradeBarRates);
	
	TMap<ECurrencyType, float> LocalScripRates;
	LocalScripRates.Add(ECurrencyType::Crowns, 5.0f);
	LocalScripRates.Add(ECurrencyType::TradeBars, 2.5f);
	LocalScripRates.Add(ECurrencyType::LocalScrip, 1.0f);
	ExchangeRates.Add(ECurrencyType::LocalScrip, LocalScripRates);
	
	// Initialize regional markets with some example items
	TArray<FName> Regions = { "Heartlands", "NorthernWastes", "EasternMarshlands", "SouthernDeserts", "WesternCoastal" };
	TArray<FName> Items = { "Bread", "Iron", "Wood", "Cloth", "Herbs", "Weapons", "Tools", "Jewelry", "Spices", "Livestock" };
	
	for (const FName& Region : Regions)
	{
		TMap<FName, FMarketPrice> RegionMarket;
		
		for (const FName& Item : Items)
		{
			FMarketPrice Price;
			
			// Set different base prices and supply/demand for different regions and items
			if (Region == "Heartlands")
			{
				if (Item == "Bread") { Price.BasePrice = 5.0f; Price.Supply = 0.8f; }
				else if (Item == "Iron") { Price.BasePrice = 20.0f; Price.Supply = 0.6f; }
				else if (Item == "Wood") { Price.BasePrice = 10.0f; Price.Supply = 0.7f; }
				// ... other items
			}
			else if (Region == "NorthernWastes")
			{
				if (Item == "Bread") { Price.BasePrice = 8.0f; Price.Supply = 0.4f; }
				else if (Item == "Iron") { Price.BasePrice = 15.0f; Price.Supply = 0.9f; }
				else if (Item == "Wood") { Price.BasePrice = 8.0f; Price.Supply = 0.8f; }
				// ... other items
			}
			// ... other regions
			
			// Set current price equal to base price initially
			Price.CurrentPrice = Price.BasePrice;
			
			// Add to region market
			RegionMarket.Add(Item, Price);
		}
		
		// Add region market to global market
		RegionalMarkets.Add(Region, RegionMarket);
	}
	
	// Initialize some trade routes
	RegisterTradeRoute("Heartlands", "NorthernWastes", { "Bread", "Cloth" }, 0.7f);
	RegisterTradeRoute("NorthernWastes", "Heartlands", { "Iron", "Furs" }, 0.7f);
	RegisterTradeRoute("Heartlands", "EasternMarshlands", { "Tools", "Weapons" }, 0.5f);
	RegisterTradeRoute("EasternMarshlands", "Heartlands", { "Herbs", "Exotic Materials" }, 0.5f);
	// ... other trade routes
}

void UEconomySystem::Deinitialize()
{
	Super::Deinitialize();
	
	// Clean up resources
	RegionalMarkets.Empty();
	ExchangeRates.Empty();
	TradeRoutes.Empty();
	TradeRouteItems.Empty();
	TradeRouteSafety.Empty();
}

void UEconomySystem::UpdateEconomy(float DeltaTime)
{
	// Accumulate time since last update
	TimeSinceLastUpdate += DeltaTime;
	
	// Major economic updates happen less frequently
	const float EconomyUpdateInterval = 300.0f; // 5 minutes real time
	
	if (TimeSinceLastUpdate >= EconomyUpdateInterval)
	{
		// Update prices based on supply and demand
		UpdatePrices();
		
		// Simulate NPC trading activity
		SimulateNPCTrading();
		
		// Update exchange rates
		UpdateExchangeRates();
		
		// Reset timer
		TimeSinceLastUpdate = 0.0f;
	}
}

float UEconomySystem::GetItemPrice(FName ItemID, FName RegionID) const
{
	// Check if region exists
	if (!RegionalMarkets.Contains(RegionID))
	{
		return 0.0f;
	}
	
	// Check if item exists in region
	const TMap<FName, FMarketPrice>* RegionMarket = RegionalMarkets.Find(RegionID);
	if (!RegionMarket || !RegionMarket->Contains(ItemID))
	{
		return 0.0f;
	}
	
	// Return current price
	return RegionMarket->Find(ItemID)->CurrentPrice;
}

float UEconomySystem::GetExchangeRate(ECurrencyType FromCurrency, ECurrencyType ToCurrency) const
{
	// Check if currencies exist
	if (!ExchangeRates.Contains(FromCurrency))
	{
		return 0.0f;
	}
	
	const TMap<ECurrencyType, float>* CurrencyRates = ExchangeRates.Find(FromCurrency);
	if (!CurrencyRates || !CurrencyRates->Contains(ToCurrency))
	{
		return 0.0f;
	}
	
	// Return exchange rate
	const float* Rate = CurrencyRates->Find(ToCurrency);
	return Rate ? *Rate : 0.0f;
}

void UEconomySystem::AdjustSupply(FName ItemID, FName RegionID, float SupplyChange)
{
	// Check if region exists
	if (!RegionalMarkets.Contains(RegionID))
	{
		return;
	}
	
	// Check if item exists in region
	TMap<FName, FMarketPrice>* RegionMarket = RegionalMarkets.Find(RegionID);
	if (!RegionMarket || !RegionMarket->Contains(ItemID))
	{
		return;
	}
	
	// Adjust supply
	FMarketPrice* ItemPrice = RegionMarket->Find(ItemID);
	ItemPrice->Supply = FMath::Clamp(ItemPrice->Supply + SupplyChange, 0.0f, 1.0f);
	
	// Update price based on new supply
	float OldPrice = ItemPrice->CurrentPrice;
	ItemPrice->CurrentPrice = ItemPrice->BasePrice * (2.0f - ItemPrice->Supply) * ItemPrice->Demand;
	
	// Broadcast price change event if significant
	if (FMath::Abs(ItemPrice->CurrentPrice - OldPrice) > 0.1f)
	{
		OnPriceChanged.Broadcast(ItemID, RegionID, ItemPrice->CurrentPrice);
	}
}

void UEconomySystem::AdjustDemand(FName ItemID, FName RegionID, float DemandChange)
{
	// Check if region exists
	if (!RegionalMarkets.Contains(RegionID))
	{
		return;
	}
	
	// Check if item exists in region
	TMap<FName, FMarketPrice>* RegionMarket = RegionalMarkets.Find(RegionID);
	if (!RegionMarket || !RegionMarket->Contains(ItemID))
	{
		return;
	}
	
	// Adjust demand
	FMarketPrice* ItemPrice = RegionMarket->Find(ItemID);
	ItemPrice->Demand = FMath::Clamp(ItemPrice->Demand + DemandChange, 0.0f, 1.0f);
	
	// Update price based on new demand
	float OldPrice = ItemPrice->CurrentPrice;
	ItemPrice->CurrentPrice = ItemPrice->BasePrice * (2.0f - ItemPrice->Supply) * ItemPrice->Demand;
	
	// Broadcast price change event if significant
	if (FMath::Abs(ItemPrice->CurrentPrice - OldPrice) > 0.1f)
	{
		OnPriceChanged.Broadcast(ItemID, RegionID, ItemPrice->CurrentPrice);
	}
}

void UEconomySystem::SimulateTradeTransaction(FName ItemID, FName SourceRegion, FName DestinationRegion, float Quantity)
{
	// Check if regions exist
	if (!RegionalMarkets.Contains(SourceRegion) || !RegionalMarkets.Contains(DestinationRegion))
	{
		return;
	}
	
	// Check if item exists in both regions
	TMap<FName, FMarketPrice>* SourceMarket = RegionalMarkets.Find(SourceRegion);
	TMap<FName, FMarketPrice>* DestMarket = RegionalMarkets.Find(DestinationRegion);
	
	if (!SourceMarket || !SourceMarket->Contains(ItemID) || !DestMarket || !DestMarket->Contains(ItemID))
	{
		return;
	}
	
	// Calculate supply change based on quantity
	// This is a simplified model - in a real system, the impact would depend on market size
	float SupplyImpact = Quantity * 0.01f;
	
	// Reduce supply in source region
	AdjustSupply(ItemID, SourceRegion, -SupplyImpact);
	
	// Increase supply in destination region
	AdjustSupply(ItemID, DestinationRegion, SupplyImpact);
}

void UEconomySystem::RegisterTradeRoute(FName SourceRegion, FName DestinationRegion, TArray<FName> ItemIDs, float SafetyRating)
{
	// Add trade route
	TradeRoutes.Add(TTuple<FName, FName>(SourceRegion, DestinationRegion));
	
	// Generate unique key for this route
	FString RouteKey = GetTradeRouteKey(SourceRegion, DestinationRegion);
	
	// Store items traded on this route
	TradeRouteItems.Add(RouteKey, ItemIDs);
	
	// Store safety rating
	TradeRouteSafety.Add(RouteKey, FMath::Clamp(SafetyRating, 0.0f, 1.0f));
}

void UEconomySystem::TriggerEconomicEvent(FName EventType, FName RegionID, float Intensity)
{
	// Check if region exists
	if (!RegionalMarkets.Contains(RegionID))
	{
		return;
	}
	
	// Apply effects based on event type
	if (EventType == "Drought")
	{
		// Reduce supply of agricultural products
		AdjustSupply("Bread", RegionID, -0.2f * Intensity);
		AdjustSupply("Livestock", RegionID, -0.15f * Intensity);
		AdjustSupply("Herbs", RegionID, -0.1f * Intensity);
		
		// Increase demand for water and food
		AdjustDemand("Water", RegionID, 0.3f * Intensity);
		AdjustDemand("Bread", RegionID, 0.2f * Intensity);
	}
	else if (EventType == "MiningBoom")
	{
		// Increase supply of metals
		AdjustSupply("Iron", RegionID, 0.3f * Intensity);
		AdjustSupply("Copper", RegionID, 0.25f * Intensity);
		AdjustSupply("Silver", RegionID, 0.15f * Intensity);
		
		// Increase demand for mining tools
		AdjustDemand("Tools", RegionID, 0.2f * Intensity);
	}
	else if (EventType == "War")
	{
		// Increase demand for weapons and armor
		AdjustDemand("Weapons", RegionID, 0.4f * Intensity);
		AdjustDemand("Armor", RegionID, 0.35f * Intensity);
		
		// Reduce safety of trade routes
		for (const auto& Route : TradeRoutes)
		{
			if (Route.Key == RegionID || Route.Value == RegionID)
			{
				FString RouteKey = GetTradeRouteKey(Route.Key, Route.Value);
				float CurrentSafety = TradeRouteSafety.Contains(RouteKey) ? TradeRouteSafety[RouteKey] : 0.5f;
				TradeRouteSafety[RouteKey] = FMath::Max(0.1f, CurrentSafety - 0.3f * Intensity);
			}
		}
	}
	
	// Broadcast economic event
	OnEconomicEvent.Broadcast(EventType, RegionID, Intensity);
}

void UEconomySystem::UpdatePrices()
{
	// Update prices for all items in all regions
	for (auto& RegionPair : RegionalMarkets)
	{
		for (auto& ItemPair : RegionPair.Value)
		{
			// Get current price data
			FMarketPrice& Price = ItemPair.Value;
			float OldPrice = Price.CurrentPrice;
			
			// Calculate new price based on supply and demand
			// This is a simplified model - real economies are more complex
			Price.CurrentPrice = Price.BasePrice * (2.0f - Price.Supply) * Price.Demand;
			
			// Add some random fluctuation based on volatility
			float RandomFactor = 1.0f + FMath::RandRange(-Price.Volatility, Price.Volatility);
			Price.CurrentPrice *= RandomFactor;
			
			// Ensure price doesn't go too low or too high
			Price.CurrentPrice = FMath::Clamp(Price.CurrentPrice, Price.BasePrice * 0.5f, Price.BasePrice * 3.0f);
			
			// Broadcast price change event if significant
			if (FMath::Abs(Price.CurrentPrice - OldPrice) > 0.1f)
			{
				OnPriceChanged.Broadcast(ItemPair.Key, RegionPair.Key, Price.CurrentPrice);
			}
		}
	}
}

void UEconomySystem::SimulateNPCTrading()
{
	// Simulate NPC traders moving goods along established trade routes
	for (const auto& Route : TradeRoutes)
	{
		FString RouteKey = GetTradeRouteKey(Route.Key, Route.Value);
		
		// Skip if we don't have items for this route
		if (!TradeRouteItems.Contains(RouteKey))
		{
			continue;
		}
		
		// Get safety rating for this route
		float Safety = TradeRouteSafety.Contains(RouteKey) ? TradeRouteSafety[RouteKey] : 0.5f;
		
		// Chance of successful trade depends on safety
		if (FMath::FRand() < Safety)
		{
			// Trade is successful
			for (const FName& Item : TradeRouteItems[RouteKey])
			{
				// Calculate trade volume based on price difference and safety
				float SourcePrice = GetItemPrice(Item, Route.Key);
				float DestPrice = GetItemPrice(Item, Route.Value);
				
				if (DestPrice > SourcePrice)
				{
					// Profitable to trade from source to destination
					float PriceDifference = DestPrice - SourcePrice;
					float TradeVolume = FMath::Min(1.0f, PriceDifference / SourcePrice) * Safety;
					
					// Execute trade
					SimulateTradeTransaction(Item, Route.Key, Route.Value, TradeVolume);
				}
			}
		}
		else
		{
			// Trade failed - could trigger events like bandit attacks, etc.
			// This would integrate with other systems
		}
	}
}

void UEconomySystem::UpdateExchangeRates()
{
	// Update exchange rates based on regional economic conditions
	// This is a simplified model - real currency exchange is more complex
	
	// For now, just add some random fluctuation
	for (auto& FromCurrency : ExchangeRates)
	{
		for (auto& ToCurrency : FromCurrency.Value)
		{
			// Skip same currency exchanges
			if (FromCurrency.Key == ToCurrency.Key)
			{
				continue;
			}
			
			// Add small random fluctuation
			float Fluctuation = FMath::RandRange(-0.05f, 0.05f);
			ToCurrency.Value = FMath::Max(0.1f, ToCurrency.Value * (1.0f + Fluctuation));
			
			// Ensure reciprocal rates are consistent
			if (ExchangeRates.Contains(ToCurrency.Key) && ExchangeRates[ToCurrency.Key].Contains(FromCurrency.Key))
			{
				ExchangeRates[ToCurrency.Key][FromCurrency.Key] = 1.0f / ToCurrency.Value;
			}
		}
	}
}

FString UEconomySystem::GetTradeRouteKey(FName SourceRegion, FName DestinationRegion) const
{
	return SourceRegion.ToString() + "_to_" + DestinationRegion.ToString();
}