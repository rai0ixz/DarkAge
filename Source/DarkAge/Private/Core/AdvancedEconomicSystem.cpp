#include "Core/AdvancedEconomicSystem.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UAdvancedEconomicSystem::UAdvancedEconomicSystem()
	: PlayerWealth(0.0f)
{
}

void UAdvancedEconomicSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeCommodities();
	ActiveTradeRoutes.Reset();
	PlayerBusinesses.Reset();
	ActiveEconomicEvents.Reset();
}

void UAdvancedEconomicSystem::Deinitialize()
{
	ActiveTradeRoutes.Reset();
	PlayerBusinesses.Reset();
	ActiveEconomicEvents.Reset();
	Super::Deinitialize();
}

void UAdvancedEconomicSystem::UpdateMarketPrices(float DeltaTime)
{
	SimulateMarketForces(DeltaTime);

	for (TPair<FString, FCommodity>& Entry : MarketCommodities)
	{
		CalculateCommodityPrice(Entry.Value);
	}
}

float UAdvancedEconomicSystem::GetCommodityPrice(const FString& CommodityName) const
{
	if (const FCommodity* Found = MarketCommodities.Find(CommodityName))
	{
		return Found->CurrentPrice;
	}
	return 0.0f;
}

void UAdvancedEconomicSystem::ModifySupply(const FString& CommodityName, int32 Amount)
{
	if (FCommodity* Found = MarketCommodities.Find(CommodityName))
	{
		Found->Supply = FMath::Max(0, Found->Supply + Amount);
	}
}

void UAdvancedEconomicSystem::ModifyDemand(const FString& CommodityName, int32 Amount)
{
	if (FCommodity* Found = MarketCommodities.Find(CommodityName))
	{
		Found->Demand = FMath::Max(0, Found->Demand + Amount);
	}
}

bool UAdvancedEconomicSystem::CreateBusiness(const FString& BusinessName, EBusinessType BusinessType, const FVector& Location)
{
	if (FindBusiness(BusinessName) != nullptr)
	{
		return false; // already exists
	}

	FPlayerBusiness NewBiz;
	NewBiz.BusinessName = BusinessName;
	NewBiz.BusinessType = BusinessType;
	NewBiz.Location = Location;
	NewBiz.Level = 1;
	NewBiz.Revenue = 0.0f;
	NewBiz.Expenses = 0.0f;
	NewBiz.Profit = 0.0f;
	NewBiz.bIsActive = true;

	PlayerBusinesses.Add(MoveTemp(NewBiz));
	return true;
}

bool UAdvancedEconomicSystem::UpgradeBusiness(const FString& BusinessName)
{
	if (FPlayerBusiness* Biz = FindBusiness(BusinessName))
	{
		Biz->Level = FMath::Clamp(Biz->Level + 1, 1, 100);
		return true;
	}
	return false;
}

void UAdvancedEconomicSystem::UpdateBusinesses(float DeltaTime)
{
	for (FPlayerBusiness& Biz : PlayerBusinesses)
	{
		if (!Biz.bIsActive)
		{
			continue;
		}
		UpdateBusinessOperations(Biz, DeltaTime);
	}
}

bool UAdvancedEconomicSystem::CreateAdvancedTradeRoute(const FVector& StartLocation, const FVector& EndLocation, const TMap<FString, int32>& Goods)
{
	FAdvancedTradeRoute Route;
	Route.RouteName = FString::Printf(TEXT("Route_%d"), ActiveTradeRoutes.Num() + 1);
	Route.StartLocation = StartLocation;
	Route.EndLocation = EndLocation;
	Route.Goods = Goods;
	Route.Distance = FVector::Dist(StartLocation, EndLocation);
	Route.ProfitMargin = 0.2f;
	Route.bIsActive = true;

	ActiveTradeRoutes.Add(MoveTemp(Route));
	return true;
}

void UAdvancedEconomicSystem::UpdateAdvancedTradeRoutes(float DeltaTime)
{
	for (FAdvancedTradeRoute& Route : ActiveTradeRoutes)
	{
		if (!Route.bIsActive)
		{
			continue;
		}
		ProcessAdvancedTradeRoute(Route, DeltaTime);
	}
}

void UAdvancedEconomicSystem::TriggerEconomicEvent(const FString& EventType)
{
	ActiveEconomicEvents.Add(EventType);

	// Simple immediate effects for demonstration
	if (EventType.Contains(TEXT("Drought"), ESearchCase::IgnoreCase))
	{
		ModifySupply(TEXT("Grain"), -50);
		ModifySupply(TEXT("Water"), -40);
	}
	else if (EventType.Contains(TEXT("Festival"), ESearchCase::IgnoreCase))
	{
		ModifyDemand(TEXT("Ale"), 40);
		ModifyDemand(TEXT("Meat"), 25);
	}
}

void UAdvancedEconomicSystem::ProcessEconomicEvents(float DeltaTime)
{
	// Decay events over time (placeholder)
	if (ActiveEconomicEvents.Num() > 0)
	{
		// Randomly remove one event to simulate resolution
		if (FMath::FRand() < 0.1f)
		{
			ActiveEconomicEvents.RemoveAt(0);
		}
	}
}

float UAdvancedEconomicSystem::GetTotalBusinessRevenue() const
{
	float Total = 0.0f;
	for (const FPlayerBusiness& Biz : PlayerBusinesses)
	{
		Total += FMath::Max(0.0f, Biz.Revenue);
	}
	return Total;
}

// ----------------- Internal Helpers -----------------

void UAdvancedEconomicSystem::InitializeCommodities()
{
	MarketCommodities.Reset();

	auto AddCommodity = [&](const FString& Name, float Base, int32 Supply, int32 Demand, EMarketTrend Trend, float Volatility)
	{
		FCommodity C;
		C.CommodityName = Name;
		C.BasePrice = Base;
		C.CurrentPrice = Base;
		C.Supply = Supply;
		C.Demand = Demand;
		C.Trend = Trend;
		C.Volatility = Volatility;
		MarketCommodities.Add(Name, C);
	};

	AddCommodity(TEXT("Grain"), 10.0f, 200, 180, EMarketTrend::Stable, 0.08f);
	AddCommodity(TEXT("Meat"), 18.0f, 120, 140, EMarketTrend::Rising, 0.12f);
	AddCommodity(TEXT("Ale"), 12.0f, 160, 150, EMarketTrend::Stable, 0.10f);
	AddCommodity(TEXT("Iron"), 25.0f, 90, 110, EMarketTrend::Volatile, 0.20f);
	AddCommodity(TEXT("Water"), 5.0f, 300, 280, EMarketTrend::Stable, 0.05f);
}

void UAdvancedEconomicSystem::SimulateMarketForces(float DeltaTime)
{
	for (TPair<FString, FCommodity>& Entry : MarketCommodities)
	{
		FCommodity& C = Entry.Value;

		// Demand/Supply drift
		int32 SupplyDrift = FMath::RoundToInt(FMath::FRandRange(-2.0f, 2.0f) * (1.0f + C.Volatility));
		int32 DemandDrift = FMath::RoundToInt(FMath::FRandRange(-2.0f, 2.0f) * (1.0f + C.Volatility));

		C.Supply = FMath::Max(0, C.Supply + SupplyDrift);
		C.Demand = FMath::Max(0, C.Demand + DemandDrift);

		// Trend influence
		switch (C.Trend)
		{
		case EMarketTrend::Rising:
			C.Demand += 1;
			break;
		case EMarketTrend::Falling:
			C.Supply += 1;
			break;
		case EMarketTrend::Boom:
			C.Demand += 2;
			break;
		case EMarketTrend::Bust:
			C.Supply += 2;
			break;
		default:
			break;
		}
	}
}

void UAdvancedEconomicSystem::CalculateCommodityPrice(FCommodity& Commodity)
{
	const float SupplyFactor = (Commodity.Supply > 0) ? 100.0f / (float)Commodity.Supply : 2.0f;
	const float DemandFactor = (Commodity.Demand > 0) ? (float)Commodity.Demand / 100.0f : 0.5f;

	// Base move by relative supply/demand
	float Target = Commodity.BasePrice * DemandFactor * SupplyFactor;

	// Add volatility noise
	Target += FMath::FRandRange(-Commodity.Volatility, Commodity.Volatility) * Commodity.BasePrice;

	// Smooth change
	Commodity.CurrentPrice = FMath::FInterpTo(Commodity.CurrentPrice, FMath::Max(0.1f, Target), 1.0f, 0.3f);
}

void UAdvancedEconomicSystem::UpdateBusinessOperations(FPlayerBusiness& Business, float DeltaTime)
{
	// Simple production/consumption model based on business type and level
	float ProductionFactor = 1.0f + (Business.Level - 1) * 0.15f;

	// Generate revenue based on type
	float RevenuePerSecond = 0.0f;
	float ExpensePerSecond = 0.0f;

	switch (Business.BusinessType)
	{
	case EBusinessType::Farm:
		RevenuePerSecond = GetCommodityPrice(TEXT("Grain")) * 0.3f * ProductionFactor;
		ExpensePerSecond = 0.8f * ProductionFactor;
		break;
	case EBusinessType::Blacksmith:
		RevenuePerSecond = GetCommodityPrice(TEXT("Iron")) * 0.25f * ProductionFactor + GetCommodityPrice(TEXT("Meat")) * 0.02f;
		ExpensePerSecond = 1.2f * ProductionFactor;
		break;
	case EBusinessType::Tavern:
		RevenuePerSecond = GetCommodityPrice(TEXT("Ale")) * 0.4f * ProductionFactor + GetCommodityPrice(TEXT("Meat")) * 0.1f;
		ExpensePerSecond = 1.5f * ProductionFactor;
		break;
	case EBusinessType::Mine:
		RevenuePerSecond = GetCommodityPrice(TEXT("Iron")) * 0.5f * ProductionFactor;
		ExpensePerSecond = 2.0f * ProductionFactor;
		break;
	case EBusinessType::Merchant:
		RevenuePerSecond = (GetCommodityPrice(TEXT("Grain")) + GetCommodityPrice(TEXT("Ale")) + GetCommodityPrice(TEXT("Meat"))) * 0.1f * ProductionFactor;
		ExpensePerSecond = 1.0f * ProductionFactor;
		break;
	case EBusinessType::Inn:
		RevenuePerSecond = GetCommodityPrice(TEXT("Ale")) * 0.3f * ProductionFactor;
		ExpensePerSecond = 1.1f * ProductionFactor;
		break;
	case EBusinessType::Temple:
		RevenuePerSecond = 0.6f * ProductionFactor; // donations
		ExpensePerSecond = 0.4f * ProductionFactor;
		break;
	default:
		break;
	}

	const float RevenueDelta = RevenuePerSecond * DeltaTime;
	const float ExpenseDelta = ExpensePerSecond * DeltaTime;

	Business.Revenue += RevenueDelta;
	Business.Expenses += ExpenseDelta;
	Business.Profit += (RevenueDelta - ExpenseDelta);

	// Apply a portion of net profit to player wealth
	PlayerWealth = PlayerWealth + FMath::Max(0.0f, (RevenueDelta - ExpenseDelta) * 0.25f);
}

void UAdvancedEconomicSystem::ProcessAdvancedTradeRoute(FAdvancedTradeRoute& Route, float DeltaTime)
{
	// Each tick, realize a small fraction of trade profit
	const float Profit = CalculateAdvancedTradeProfit(Route) * DeltaTime;
	PlayerWealth = PlayerWealth + Profit;

	// Random chance to pause route due to bandits/weather
	if (FMath::FRand() < 0.001f)
	{
		Route.bIsActive = false;
	}
}

void UAdvancedEconomicSystem::GenerateEconomicEvent()
{
	// Example: randomly add an event
	static const TArray<FString> PossibleEvents = {
		TEXT("Harvest Boom"), TEXT("Drought"), TEXT("Trade Fair"), TEXT("Mine Collapse"), TEXT("Bandit Activity"), TEXT("Festival")
	};

	const int32 Index = FMath::RandRange(0, PossibleEvents.Num() - 1);
	TriggerEconomicEvent(PossibleEvents[Index]);
}

FCommodity* UAdvancedEconomicSystem::FindCommodity(const FString& CommodityName)
{
	return MarketCommodities.Find(CommodityName);
}

FPlayerBusiness* UAdvancedEconomicSystem::FindBusiness(const FString& BusinessName)
{
	for (FPlayerBusiness& Biz : PlayerBusinesses)
	{
		if (Biz.BusinessName.Equals(BusinessName, ESearchCase::IgnoreCase))
		{
			return &Biz;
		}
	}
	return nullptr;
}

float UAdvancedEconomicSystem::CalculateBusinessRevenue(const FPlayerBusiness& Business)
{
	// Use last tick revenue approximation
	return FMath::Max(0.0f, Business.Revenue);
}

float UAdvancedEconomicSystem::CalculateBusinessExpenses(const FPlayerBusiness& Business)
{
	return FMath::Max(0.0f, Business.Expenses);
}

float UAdvancedEconomicSystem::CalculateAdvancedTradeProfit(const FAdvancedTradeRoute& Route)
{
	// Profit proportional to value of goods and distance, scaled by margin
	float GoodsValue = 0.0f;
	for (const TPair<FString, int32>& Pair : Route.Goods)
	{
		const float Price = GetCommodityPrice(Pair.Key);
		GoodsValue += Price * Pair.Value;
	}

	const float DistanceFactor = 1.0f + (Route.Distance / 10000.0f);
	return GoodsValue * Route.ProfitMargin * DistanceFactor * 0.01f; // small per-tick realization
}