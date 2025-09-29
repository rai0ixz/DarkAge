#pragma once

#include "CoreMinimal.h"
#include "Data/ItemData.h"
#include "InventoryData.generated.h"

// Canonical runtime inventory item used by InventoryComponent and gameplay
USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FString ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    EItemType ItemType = EItemType::IT_Generic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    EItemRarity Rarity = EItemRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    float Weight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Value = 0; // Gold value

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    TMap<FString, float> Stats; // Item stats (damage, defense, etc.)

    FInventoryItem()
        : ItemID(TEXT(""))
        , ItemName(TEXT(""))
        , Description(TEXT(""))
        , ItemType(EItemType::IT_Generic)
        , Rarity(EItemRarity::Common)
        , Quantity(1)
        , MaxStackSize(1)
        , Weight(1.0f)
        , Value(0)
        , Stats()
    {}

    bool CanStackWith(const FInventoryItem& Other) const
    {
        return ItemID == Other.ItemID && Quantity < MaxStackSize;
    }

    int32 GetStackSpace() const
    {
        return MaxStackSize - Quantity;
    }
};

// Compact save entry to avoid duplication with gameplay struct in SaveGame
USTRUCT(BlueprintType)
struct FInventorySaveEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FString ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    int32 Quantity = 0;
};