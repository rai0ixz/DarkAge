#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/RegionState.h"
#include "WorldStateData.generated.h"

/**
 * Enum for different region types in the Medieval Mayhem world
 */
UENUM(BlueprintType)
enum class ERegionType : uint8
{
    Village      UMETA(DisplayName = "Village"),
    Town         UMETA(DisplayName = "Town"),
    City         UMETA(DisplayName = "City"),
    Wilderness   UMETA(DisplayName = "Wilderness"),
    Dungeon      UMETA(DisplayName = "Dungeon"),
    Castle       UMETA(DisplayName = "Castle"),
    Outpost      UMETA(DisplayName = "Outpost")
};

/**
 * Enum for regional wealth levels affecting economy
 */
UENUM(BlueprintType)
enum class ERegionWealth : uint8
{
    Impoverished UMETA(DisplayName = "Impoverished"),
    Poor         UMETA(DisplayName = "Poor"),
    Modest       UMETA(DisplayName = "Modest"),
    Prosperous   UMETA(DisplayName = "Prosperous"),
    Wealthy      UMETA(DisplayName = "Wealthy"),
    Opulent      UMETA(DisplayName = "Opulent")
};

/**
 * Enum for law enforcement presence levels
 */
UENUM(BlueprintType)
enum class ELawEnforcementLevel : uint8
{
    None         UMETA(DisplayName = "None"),
    Minimal      UMETA(DisplayName = "Minimal"),
    Light        UMETA(DisplayName = "Light"),
    Moderate     UMETA(DisplayName = "Moderate"),
    Heavy        UMETA(DisplayName = "Heavy"),
    Oppressive   UMETA(DisplayName = "Oppressive")
};

/**
 * Regional data structure for defining region properties
 * Used for configuring regions in data tables
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FRegionData : public FTableRowBase
{
    GENERATED_BODY()

    FORCEINLINE FRegionData()
        : RegionName(FText::GetEmpty())
        , RegionType(ERegionType::Village)
        , ClimateZone(TEXT(""))
        , WealthLevel(ERegionWealth::Modest)
        , LawEnforcementLevel(ELawEnforcementLevel::Moderate)
        , Population(100)
        , CrimeToleranceModifier(1.0f)
        , EconomicActivityModifier(1.0f)
        , TravelTimeToNearestTown(1.0f)
        , bHasMarket(false)
        , bHasInn(false)
        , bHasBlacksmith(false)
        , bHasAlchemist(false)
        , PoliticalStability(0.0f)
    {
    }

    // Basic region information
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    FText RegionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    ERegionType RegionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    FString ClimateZone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    ERegionWealth WealthLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    ELawEnforcementLevel LawEnforcementLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region Info")
    int32 Population;

    // Crime and reputation modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime & Law", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float CrimeToleranceModifier;

    // Economic factors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float EconomicActivityModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float TravelTimeToNearestTown;

    // Available services
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Services")
    bool bHasMarket;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Services")
    bool bHasInn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Services")
    bool bHasBlacksmith;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Services")
    bool bHasAlchemist;

    // Economic resources
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    TArray<FName> AvailableResources;

    // Connected regions for trade routes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geography")
    TArray<FString> ConnectedRegions;

    // Political stability of the region (0.0 = anarchy, 1.0 = stable)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Politics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PoliticalStability;

    // Points of interest within the region (e.g., landmarks, dungeons)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geography")
    TMap<FName, FVector> PointsOfInterest;

    // Active world events in this region
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World State")
    TArray<FName> ActiveWorldEvents;
};


/**
 * Overall world state data for persistence
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FWorldStateData
{
    GENERATED_BODY()

    FWorldStateData()
        : CurrentWorldTime(6.0f)
        , TimeScale(1.0f)
        , CurrentRegion(TEXT("DefaultRegion"))
        , RegionStates()
        , GlobalEvents()
        , PlayerGlobalReputation(0.0f)
        , WorldEconomicState(1.0f)
    {
    }

    // Time and world progression
    UPROPERTY(BlueprintReadWrite, Category = "World State")
    float CurrentWorldTime;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    float TimeScale;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString CurrentRegion;

    // All discovered and tracked regions
    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, FRegionState> RegionStates;

    // Global events affecting the entire world
    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FName> GlobalEvents;

    // Player's overall reputation across all regions
    UPROPERTY(BlueprintReadWrite, Category = "World State")
    float PlayerGlobalReputation;

    // Overall world economic health (affects all supply chains)
    UPROPERTY(BlueprintReadWrite, Category = "World State")
    float WorldEconomicState;
};

/**
 * Environmental condition data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FEnvironmentalCondition
{
    GENERATED_BODY()

    FEnvironmentalCondition()
        : ConditionName()
        , Description()
        , VisibilityModifier(1.0f)
        , MovementSpeedModifier(1.0f)
        , CrimeDetectionModifier(1.0f)
        , NPCMoraleModifier(0.0f)
        , bAffectsSupplyChains(false)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Condition")
    FText ConditionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environmental Condition")
    FText Description;

    // Gameplay modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers", meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float VisibilityModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers", meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float MovementSpeedModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers", meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float CrimeDetectionModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
    float NPCMoraleModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    bool bAffectsSupplyChains;
};