// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EconomySystem.generated.h"

/**
 * Currency type enum
 */
UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
	Crowns      UMETA(DisplayName = "Crowns"),
	TradeBars   UMETA(DisplayName = "Trade Bars"),
	LocalScrip  UMETA(DisplayName = "Local Scrip")
};

/**
 * Structure to represent a market price for an item
 */
USTRUCT(BlueprintType)
struct FMarketPrice
{
	GENERATED_BODY()
	
	// Base price in crowns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float BasePrice;
	
	// Current price in crowns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float CurrentPrice;
	
	// Price volatility (how much the price can fluctuate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float Volatility;
	
	// Supply level (0.0 = none, 1.0 = abundant)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float Supply;
	
	// Demand level (0.0 = none, 1.0 = high)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float Demand;
	
	FMarketPrice()
		: BasePrice(10.0f)
		, CurrentPrice(10.0f)
		, Volatility(0.1f)
		, Supply(0.5f)
		, Demand(0.5f)
	{
	}
};

/**
 * Economy System for Dark Age
 * 
 * Manages the dynamic economy, including prices, supply and demand,
 * trade routes, and currency exchange rates.
 */
UCLASS()
class DARKAGE_API UEconomySystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UEconomySystem();
	
	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Deinitialize the subsystem
	virtual void Deinitialize() override;
	
	// Update economy based on time elapsed
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void UpdateEconomy(float DeltaTime);
	
	// Get current price for an item in a specific region
	UFUNCTION(BlueprintCallable, Category = "Economy")
	float GetItemPrice(FName ItemID, FName RegionID) const;
	
	// Get current exchange rate between currencies
	UFUNCTION(BlueprintCallable, Category = "Economy")
	float GetExchangeRate(ECurrencyType FromCurrency, ECurrencyType ToCurrency) const;
	
	// Adjust supply of an item in a region
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AdjustSupply(FName ItemID, FName RegionID, float SupplyChange);
	
	// Adjust demand for an item in a region
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AdjustDemand(FName ItemID, FName RegionID, float DemandChange);
	
	// Simulate a trade transaction between regions
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void SimulateTradeTransaction(FName ItemID, FName SourceRegion, FName DestinationRegion, float Quantity);
	
	// Register a new trade route between regions
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void RegisterTradeRoute(FName SourceRegion, FName DestinationRegion, TArray<FName> ItemIDs, float SafetyRating);
	
	// Trigger an economic event (disaster, boom, etc.)
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void TriggerEconomicEvent(FName EventType, FName RegionID, float Intensity);
	
	// Delegate for price change events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPriceChanged, FName, ItemID, FName, RegionID, float, NewPrice);
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnPriceChanged OnPriceChanged;
	
	// Delegate for significant economic events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEconomicEvent, FName, EventType, FName, RegionID, float, Intensity);
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnEconomicEvent OnEconomicEvent;
	
private:
	// Market prices for items in different regions
	TMap<FName, TMap<FName, FMarketPrice>> RegionalMarkets;
	
	// Exchange rates between currencies
	TMap<ECurrencyType, TMap<ECurrencyType, float>> ExchangeRates;
	
	// Trade routes between regions
	TArray<TTuple<FName, FName>> TradeRoutes;
	
	// Items traded on each route
	TMap<FString, TArray<FName>> TradeRouteItems;
	
	// Safety rating for each trade route (affects prices and availability)
	UPROPERTY()
	TMap<FString, float> TradeRouteSafety;
	
	// Time since last major economic update
	float TimeSinceLastUpdate;
	
	// Update prices based on supply and demand
	void UpdatePrices();
	
	// Simulate NPC trading activity
	void SimulateNPCTrading();
	
	// Update exchange rates between currencies
	void UpdateExchangeRates();
	
	// Generate a unique key for a trade route
	FString GetTradeRouteKey(FName SourceRegion, FName DestinationRegion) const;
};