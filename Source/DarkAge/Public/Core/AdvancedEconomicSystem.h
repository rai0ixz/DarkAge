#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AdvancedEconomicSystem.generated.h"

UENUM(BlueprintType)
enum class EBusinessType : uint8
{
    Blacksmith    UMETA(DisplayName = "Blacksmith"),
    Tavern        UMETA(DisplayName = "Tavern"),
    Farm          UMETA(DisplayName = "Farm"),
    Mine          UMETA(DisplayName = "Mine"),
    Merchant      UMETA(DisplayName = "Merchant"),
    Inn           UMETA(DisplayName = "Inn"),
    Temple        UMETA(DisplayName = "Temple")
};

UENUM(BlueprintType)
enum class EMarketTrend : uint8
{
    Stable        UMETA(DisplayName = "Stable"),
    Rising        UMETA(DisplayName = "Rising"),
    Falling       UMETA(DisplayName = "Falling"),
    Volatile      UMETA(DisplayName = "Volatile"),
    Boom          UMETA(DisplayName = "Boom"),
    Bust          UMETA(DisplayName = "Bust")
};

USTRUCT(BlueprintType)
struct FCommodity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    FString CommodityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    float BasePrice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    float CurrentPrice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    int32 Supply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    int32 Demand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    EMarketTrend Trend;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commodity")
    float Volatility;

    FCommodity()
    {
        BasePrice = 10.0f;
        CurrentPrice = 10.0f;
        Supply = 100;
        Demand = 100;
        Trend = EMarketTrend::Stable;
        Volatility = 0.1f;
    }
};

USTRUCT(BlueprintType)
struct FPlayerBusiness
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    FString BusinessName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    EBusinessType BusinessType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    float Revenue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    float Expenses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    float Profit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    TMap<FString, int32> Inventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    TArray<FString> Employees;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Business")
    bool bIsActive;

    FPlayerBusiness()
    {
        Level = 1;
        Revenue = 0.0f;
        Expenses = 0.0f;
        Profit = 0.0f;
        bIsActive = true;
    }
};

USTRUCT(BlueprintType)
struct FAdvancedTradeRoute
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    FString RouteName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    FVector StartLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    FVector EndLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    TMap<FString, int32> Goods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    float Distance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    float ProfitMargin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    bool bIsActive;

    FAdvancedTradeRoute()
    {
        Distance = 0.0f;
        ProfitMargin = 0.2f;
        bIsActive = true;
    }
};

UCLASS()
class DARKAGE_API UAdvancedEconomicSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UAdvancedEconomicSystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Market system
    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateMarketPrices(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Economy")
    float GetCommodityPrice(const FString& CommodityName) const;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ModifySupply(const FString& CommodityName, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ModifyDemand(const FString& CommodityName, int32 Amount);

    // Business management
    UFUNCTION(BlueprintCallable, Category = "Economy")
    bool CreateBusiness(const FString& BusinessName, EBusinessType BusinessType, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    bool UpgradeBusiness(const FString& BusinessName);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateBusinesses(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Economy")
    const TArray<FPlayerBusiness>& GetPlayerBusinesses() const { return PlayerBusinesses; }

    // Trading system
    UFUNCTION(BlueprintCallable, Category = "Economy")
    bool CreateAdvancedTradeRoute(const FVector& StartLocation, const FVector& EndLocation, const TMap<FString, int32>& Goods);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateAdvancedTradeRoutes(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Economy")
    const TArray<FAdvancedTradeRoute>& GetActiveTradeRoutes() const { return ActiveTradeRoutes; }

    // Economic events
    UFUNCTION(BlueprintCallable, Category = "Economy")
    void TriggerEconomicEvent(const FString& EventType);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ProcessEconomicEvents(float DeltaTime);

    // Player economy
    UFUNCTION(BlueprintPure, Category = "Economy")
    float GetPlayerWealth() const { return PlayerWealth; }

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ModifyPlayerWealth(float Amount);

    UFUNCTION(BlueprintPure, Category = "Economy")
    float GetTotalBusinessRevenue() const;

private:
    // Market data
    UPROPERTY()
    TMap<FString, FCommodity> MarketCommodities;

    // Business data
    UPROPERTY()
    TArray<FPlayerBusiness> PlayerBusinesses;

    // Trade data
    UPROPERTY()
    TArray<FAdvancedTradeRoute> ActiveTradeRoutes;

    // Player data
    UPROPERTY()
    float PlayerWealth;

    // Economic events
    UPROPERTY()
    TArray<FString> ActiveEconomicEvents;

    // Core functions
    void InitializeCommodities();
    void SimulateMarketForces(float DeltaTime);
    void CalculateCommodityPrice(FCommodity& Commodity);
    void UpdateBusinessOperations(FPlayerBusiness& Business, float DeltaTime);
    void ProcessAdvancedTradeRoute(FAdvancedTradeRoute& Route, float DeltaTime);
    void GenerateEconomicEvent();
    FCommodity* FindCommodity(const FString& CommodityName);
    FPlayerBusiness* FindBusiness(const FString& BusinessName);
    float CalculateBusinessRevenue(const FPlayerBusiness& Business);
    float CalculateBusinessExpenses(const FPlayerBusiness& Business);
    float CalculateAdvancedTradeProfit(const FAdvancedTradeRoute& Route);
};

// Inline definition to ensure linkage for UFUNCTION wrapper
inline void UAdvancedEconomicSystem::ModifyPlayerWealth(float Amount)
{
    PlayerWealth += Amount;
}