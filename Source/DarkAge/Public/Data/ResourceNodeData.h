#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/ItemData.h" // Include for EHarvestToolType
#include "ResourceNodeData.generated.h"

USTRUCT(BlueprintType)
struct FResourceYield
{
    GENERATED_BODY()

    // The ID of the item that is yielded (from the ItemData table)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Yield")
    FName ItemID;

    // The quantity of the item yielded per harvest action
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Yield")
    int32 Quantity;

    FResourceYield()
        : ItemID(NAME_None)
        , Quantity(1)
    {}
};

USTRUCT(BlueprintType)
struct FResourceNodeData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FResourceNodeData()
        : NodeName()
        , RequiredToolType(EHarvestToolType::HTT_None)
        , RequiredHarvestingPower(0.f)
        , NodeHealth(100.f)
        , RespawnTime(300.f) // Time in seconds
    {}

    // The display name of the resource node (e.g., "Iron Vein")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    FText NodeName;

    // The type of tool required to harvest this node
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    EHarvestToolType RequiredToolType;

    // The minimum power the tool needs to have to harvest this node
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    float RequiredHarvestingPower;

    // How many "hits" the node can take before being depleted
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    float NodeHealth;

    // The resources yielded by this node
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    TArray<FResourceYield> ResourceYields;

    // Time in seconds for this node to respawn after being depleted
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node Data")
    float RespawnTime;
};