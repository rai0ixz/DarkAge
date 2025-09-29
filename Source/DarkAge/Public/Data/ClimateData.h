#pragma once

#include "CoreMinimal.h"
#include "ClimateData.generated.h"

UENUM(BlueprintType)
enum class EClimateType : uint8
{
	None        UMETA(DisplayName = "None"),
	Temperate   UMETA(DisplayName = "Temperate"),
	Arid        UMETA(DisplayName = "Arid"),
	Arctic      UMETA(DisplayName = "Arctic"),
    Tundra      UMETA(DisplayName = "Tundra"),
    Tropical    UMETA(DisplayName = "Tropical")
};