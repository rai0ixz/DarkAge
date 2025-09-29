#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EconomySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FBasicMarketData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Market Data")
    float Supply = 1000.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Market Data")
    float Demand = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Data")
    float BasePrice = 10.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Market Data")
    float CurrentPrice = 10.f;

    friend FArchive& operator<<(FArchive& Ar, FBasicMarketData& Data);
};

USTRUCT(BlueprintType)
struct FRegionMarketData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Market Data")
    TMap<FName, FBasicMarketData> MarketItems;

    friend FArchive& operator<<(FArchive& Ar, FRegionMarketData& Data);
};

USTRUCT(BlueprintType)
struct FTradeRouteData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade Route")
    FName StartRegion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade Route")
    FName EndRegion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade Route")
    float Efficiency = 1.0f; // 0.0 = closed, 1.0 = fully open
};

USTRUCT(BlueprintType)
struct FMarketEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    FName EventID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    FName Region;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    float SupplyChange = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    float DemandChange = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Market Event")
    float Duration = 0.0f;
};

UCLASS()
class DARKAGE_API UEconomySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UEconomySubsystem();

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void RegisterItem(const FName& Region, const FName& ItemId, float BasePrice);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateSupply(const FName& Region, const FName& ItemId, float Change);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateDemand(const FName& Region, const FName& ItemId, float Change);

    UFUNCTION(BlueprintPure, Category = "Economy")
    float GetItemPrice(const FName& Region, const FName& ItemId) const;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    bool GetItemData(FName ItemID, FItemData& OutItemData) const;

    UFUNCTION(BlueprintPure, Category = "Economy")
    TArray<FName> GetItemsWithHighDemand(const FName& Region) const;

    UFUNCTION(BlueprintPure, Category = "Economy")
    TArray<FName> GetItemsWithLowSupply(const FName& Region) const;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ProcessTradeRoutes();

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void TriggerMarketEvent(const FMarketEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateSeasonalEffects(const FName& Season);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void RecordPlayerTransaction(const FName& Region, const FName& ItemId, int32 Quantity, bool bIsPurchase);

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    // Advanced Economic Simulation Features
    UFUNCTION(BlueprintCallable, Category = "Economy|Advanced")
    void ProcessAdvancedEconomicSimulation();
    
    UFUNCTION(BlueprintPure, Category = "Economy|Analysis")
    float GetMarketVolatility(const FName& Region) const;
    
    UFUNCTION(BlueprintPure, Category = "Economy|Analysis")
    float GetRegionProsperity(const FName& Region) const;
    
    UFUNCTION(BlueprintPure, Category = "Economy|Analysis")
    TArray<FName> GetMostProfitableTradeRoutes() const;

private:
    void UpdatePrice(const FName& Region, const FName& ItemId);
    void UpdateMarketEvent(FMarketEvent Event);
    void RegisterDebugCommands();
    void InitializeDefaultEconomy();
    void SaveEconomy() const;
    void LoadEconomy();
    
    // Advanced Economic Methods
    void ProcessInflationDeflation();
    void ProcessEconomicCycles();
    void ProcessPlayerEconomicImpact();
    void ProcessFactionEconomicEffects();
    void ProcessSeasonalEconomicChanges();
    void ProcessRandomEconomicEvents();
    
    float CalculateTotalEconomicActivity();
    float CalculateInflationRate(float EconomicActivity);
    void ApplyEconomicCycleEffects(float CyclePhase);
    FString GetEconomicCyclePhaseText(float CyclePhase);
    FString DetermineItemCategory(const FName& ItemName);
    
    float CalculatePlayerWealthInfluence();
    float CalculatePlayerTradeInfluence();
    
    void ProcessFactionTradeBonus(const FString& FactionName, const FString& Category, float Bonus);
    float GetFactionEconomicInfluence(const FString& FactionName);
    void ProcessFactionConflictEffects();
    
    FString GetCurrentSeason();
    void ApplySeasonalSupplyChanges(const FString& Season);
    void ApplySeasonalDemandChanges(const FString& Season);
    void ApplySupplyMultipliers(const TMap<FString, float>& Multipliers);
    void ApplyDemandMultipliers(const TMap<FString, float>& Multipliers);
    
    void TriggerRandomEconomicEvent();
    void ExecuteRandomEconomicEvent(const FString& EventType);
    TArray<FName> GetAllTradeableItems() const;
    TArray<FName> GetAllRegions() const;
    
    float CalculateRouteProfitPotential(const FName& StartRegion, const FName& EndRegion) const;

    UPROPERTY()
    TMap<FName, FRegionMarketData> RegionalMarkets;

    UPROPERTY()
    TArray<FTradeRouteData> TradeRoutes;

    UPROPERTY()
    TArray<FMarketEvent> ActiveMarketEvents;

    FTimerHandle TradeRouteTimerHandle;
};