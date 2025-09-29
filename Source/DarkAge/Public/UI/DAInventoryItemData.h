#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/InventoryComponent.h"
#include "Data/InventoryData.h"
#include "DAInventoryItemData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DARKAGE_API UDAInventoryItemData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FInventoryItem ItemData;
};