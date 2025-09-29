#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "CrimeTypes.generated.h"

UENUM(BlueprintType)
enum class ECrimeType : uint8
{
	None		UMETA(DisplayName = "None"),
	Theft		UMETA(DisplayName = "Theft"),
	Assault		UMETA(DisplayName = "Assault"),
	Murder		UMETA(DisplayName = "Murder"),
	Trespassing UMETA(DisplayName = "Trespassing"),
	Arson		UMETA(DisplayName = "Arson"),
    Smuggling   UMETA(DisplayName = "Smuggling")
};