#pragma once

#include "CoreMinimal.h"
#include "Items/DABaseItem.h"
#include "DAEquipmentItem.generated.h"

UCLASS()
class DARKAGE_API ADAEquipmentItem : public ADABaseItem
{
    GENERATED_BODY()

public:
    ADAEquipmentItem();

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual void Equip(APawn* InstigatorPawn);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual void Unequip(APawn* InstigatorPawn);
};