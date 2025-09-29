#include "Items/DAWeaponItem.h"

ADAWeaponItem::ADAWeaponItem()
{
}

void ADAWeaponItem::Attack()
{
    // For now, we'll just print a debug message.
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack!"));
    }
}