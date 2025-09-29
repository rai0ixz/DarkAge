#pragma once

#include "CoreMinimal.h"
#include "Items/DAEquipmentItem.h"
#include "DAWeaponItem.generated.h"

UCLASS()
class DARKAGE_API ADAWeaponItem : public ADAEquipmentItem
{
    GENERATED_BODY()

public:
    ADAWeaponItem();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Attack();
};