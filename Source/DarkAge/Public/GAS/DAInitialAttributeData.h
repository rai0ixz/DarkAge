// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DAInitialAttributeData.generated.h"

USTRUCT(BlueprintType)
struct DARKAGE_API FDAInitialAttributeData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Hunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Thirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Warmth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Sleep = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Strength = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Dexterity = 10.0f;
};