# EconomySubsystem

**Class:** `UEconomySubsystem`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Core/EconomySubsystem.h`  
**Implementation:** `Source/DarkAge/Private/Core/EconomySubsystem.cpp`

## Overview

The `UEconomySubsystem` is a Game Instance Subsystem responsible for managing the dynamic economy simulation in Dark Age. It handles supply and demand mechanics, price fluctuations, trade routes, and merchant behaviors across different settlements.

## Key Features

- **Dynamic Pricing**: Real-time price adjustments based on supply and demand
- **Trade Route Management**: Inter-settlement commerce simulation
- **Market Events**: Economic crises, booms, and special events
- **Vendor Integration**: NPC merchant behavior coordination
- **Regional Economics**: Settlement-specific economic conditions

## Core Functionality

### Supply & Demand Simulation
```cpp
// Update item supply in a settlement
void UpdateItemSupply(EItemType ItemType, int32 Quantity);

// Calculate price based on current market conditions
float CalculatePriceFromSupplyDemand(EItemType ItemType);

// Process player trade actions and market impact
void ProcessPlayerTradeAction(const FTradeTransaction& Transaction);
```

### Trade Route Management
```cpp
// Establish trade routes between settlements
void CreateTradeRoute(const FString& FromSettlement, const FString& ToSettlement);

// Check for trade route opportunities based on price differences
void CheckForTradeRouteGeneration(EItemType ItemType);

// Update all active trade routes
void UpdateTradeRoutes(float DeltaTime);
```

### Market Events
```cpp
// Trigger economic events that affect prices
void TriggerMarketEvent(EMarketEventType EventType, const FString& Settlement);

// Process economic ripple effects from world events
void ProcessEconomicEvent(const FGlobalEvent& Event);
```

## Data Structures

### FTradeTransaction
```cpp
USTRUCT(BlueprintType)
struct FTradeTransaction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PricePerUnit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Settlement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Trader;
};
```

## Integration Points

### Global Event Bus
- Listens for: `MarketPriceChanged`, `TradeCompleted`, `PlayerCommittedCrime`
- Broadcasts: `EconomicCrisisStarted`, `TradeRouteEstablished`, `MarketEventTriggered`

### Related Systems
- **NPCEcosystemSubsystem**: Merchant NPC behavior and needs
- **FactionManagerSubsystem**: Faction-based trade restrictions
- **WorldEcosystemSubsystem**: Resource availability and environmental factors

## Configuration

Economic parameters are configured via Data Tables in `Content/_DA/Data/Economy/`:
- `EconomicItemData` - Base prices, supply/demand curves
- `SettlementEconomicData` - Regional economic modifiers
- `TradeRouteData` - Trade route configurations and costs

## Performance Considerations

- Economic calculations are performed on a tick interval (default: 5 seconds)
- Price updates are batched to avoid excessive UI refreshes
- Trade route pathfinding is cached and updated periodically

## Usage Examples

### Basic Price Query
```cpp
if (UEconomySubsystem* EconomySys = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
{
    float CurrentPrice = EconomySys->GetItemPrice(EItemType::IronOre, TEXT("Millhaven"));
    UE_LOG(LogTemp, Log, TEXT("Iron Ore price in Millhaven: %f"), CurrentPrice);
}
```

### Processing Player Trade
```cpp
FTradeTransaction Transaction;
Transaction.ItemType = EItemType::IronOre;
Transaction.Quantity = 50;
Transaction.PricePerUnit = 5.0f;
Transaction.Settlement = TEXT("Millhaven");
Transaction.Trader = PlayerCharacter;

EconomySys->ProcessPlayerTradeAction(Transaction);
```

## See Also

- [FactionManagerSubsystem](FactionManagerSubsystem.md) - Political trade restrictions
- [NPCEcosystemSubsystem](NPCEcosystemSubsystem.md) - Merchant behavior
- [CraftingComponent](CraftingComponent.md) - Resource consumption