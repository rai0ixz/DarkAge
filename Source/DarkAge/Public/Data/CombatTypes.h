#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unarmed     UMETA(DisplayName = "Unarmed"),
    Sword       UMETA(DisplayName = "Sword"),
    Axe         UMETA(DisplayName = "Axe"),
    Spear       UMETA(DisplayName = "Spear"),
    Bow         UMETA(DisplayName = "Bow"),
    Crossbow    UMETA(DisplayName = "Crossbow"),
    Mace        UMETA(DisplayName = "Mace"),
    Dagger      UMETA(DisplayName = "Dagger"),
    Staff       UMETA(DisplayName = "Staff"),
    Shield      UMETA(DisplayName = "Shield"),
    Thrown      UMETA(DisplayName = "Thrown Weapon")
};