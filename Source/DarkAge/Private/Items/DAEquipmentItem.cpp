#include "Items/DAEquipmentItem.h"

ADAEquipmentItem::ADAEquipmentItem()
{
}

void ADAEquipmentItem::Equip(APawn* InstigatorPawn)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Equipped"));
    }
}

void ADAEquipmentItem::Unequip(APawn* InstigatorPawn)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Unequipped"));
    }
}