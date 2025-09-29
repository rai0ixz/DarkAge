#pragma once

#include "CoreMinimal.h"
#include "Items/DABaseItem.h"
#include "DAConsumableItem.generated.h"

UCLASS()
class DARKAGE_API ADAConsumableItem : public ADABaseItem
{
    GENERATED_BODY()

public:
    ADAConsumableItem();

    // This function will be called when the item is consumed.
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    virtual void Consume(APawn* InstigatorPawn);
};