#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "AdvancedMarketSimulationSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMarketEventTriggered, const FString&, EventType, const FString&, MarketName);

UENUM(BlueprintType)
enum class EMarketType : uint8
{
    Village     UMETA(DisplayName = "Village"),
    Town        UMETA(DisplayName = "Town"),
    City        UMETA(DisplayName = "City"),
    Port        UMETA(DisplayName = "Port")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FCommodityData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    float BasePrice = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    float CurrentPrice = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    int32 Supply = 100;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    int32 Demand = 100;

    FCommodityData()
    {
        BasePrice = 10.0f;
        CurrentPrice = 10.0f;
        Supply = 100;
        Demand = 100;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FMarketData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    FString MarketName;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    FVector Location;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    EMarketType MarketType = EMarketType::Village;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    float Wealth = 1000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    int32 Population = 500;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    float TradingVolume = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    float MarketStability = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Market")
    TMap<FString, FCommodityData> Commodities;

    FMarketData()
    {
        MarketName = TEXT("");
        Location = FVector::ZeroVector;
        MarketType = EMarketType::Village;
        Wealth = 1000.0f;
        Population = 500;
        TradingVolume = 0.0f;
        MarketStability = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FTradeRoute
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    FString OriginMarket;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    FString DestinationMarket;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    float Distance = 1000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    float BaseEfficiency = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    float CurrentEfficiency = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    float TradeVolume = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Trade")
    bool bIsActive = true;

    FTradeRoute()
    {
        OriginMarket = TEXT("");
        DestinationMarket = TEXT("");
        Distance = 1000.0f;
        BaseEfficiency = 1.0f;
        CurrentEfficiency = 1.0f;
        TradeVolume = 0.0f;
        bIsActive = true;
    }
};

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedMarketSimulationSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedMarketSimulationSubsystem() {}

    /** If false, the entire market simulation update will be skipped. Can be toggled with the 'ToggleMarket' console command. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market")
    bool bMarketEnabled = true;

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedMarketSimulationSubsystem, STATGROUP_Tickables); }

    // Market Event Delegate
    UPROPERTY(BlueprintAssignable, Category = "Market Events")
    FOnMarketEventTriggered OnMarketEventTriggered;

    // Query Functions
    UFUNCTION(BlueprintCallable, Category = "Market")
    float GetCommodityPrice(const FString& MarketName, const FString& CommodityName) const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    int32 GetCommoditySupply(const FString& MarketName, const FString& CommodityName) const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    int32 GetCommodityDemand(const FString& MarketName, const FString& CommodityName) const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    TArray<FString> GetAllMarkets() const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    TArray<FString> GetAllCommodities() const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    float GetMarketWealth(const FString& MarketName) const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    float GetTradeRouteEfficiency(const FString& Origin, const FString& Destination) const;

    UFUNCTION(BlueprintCallable, Category = "Market")
    void TriggerMarketEvent(const FString& EventType, const FString& MarketName);

protected:
    // Core market data
    UPROPERTY()
    TMap<FString, FMarketData> Markets;

    UPROPERTY()
    TMap<FString, FTradeRoute> TradeRoutes;

    UPROPERTY()
    TMap<FString, float> CommodityBasePrices;

    UPROPERTY()
    TArray<FString> MarketEventTypes;

    // Timers
    float MarketUpdateTimer = 0.0f;
    float EventCheckTimer = 0.0f;

    // Initialization functions
    void InitializeMarketData();
    void CreateMarket(const FString& MarketName, const FVector& Location, EMarketType Type, float Wealth);
    void InitializeMarketCommodities(FMarketData& Market);
    void SetCommoditySupplyDemand(const FMarketData& Market, const FString& CommodityName, FCommodityData& CommodityData);
    int32 GetPopulationForMarketType(EMarketType Type);

    void InitializeTradingRoutes();
    void CreateTradeRoute(const FString& Origin, const FString& Destination, float Distance, float Efficiency);

    void InitializeMarketEvents();

    // Update functions
    void UpdateAllMarkets();
    void UpdateMarketPrices(FMarketData& Market);
    void UpdateMarketSupplyDemand(FMarketData& Market);
    void ProcessMarketTrade(FMarketData& Market);

    void UpdateTradeRoutes(float DeltaTime);
    void ProcessTradeRoute(FTradeRoute& Route);
    void UpdateRouteEfficiency(FTradeRoute& Route);

    // Event functions
    void ProcessMarketEvents();
    void TriggerRandomMarketEvent();
    void ApplyMarketEvent(const FString& EventType, const FString& MarketName);
};