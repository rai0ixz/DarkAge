// Copyright Epic Games, Inc. All Rights Reserved.
// Note: This can be extended in Blueprints for designer tweaks, but here's a C++ base.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DADiseaseGameplayEffect.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DARKAGE_API UDADiseaseGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UDADiseaseGameplayEffect();

    // Properties for stages (can be set in BP child classes)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Disease")
    TArray<float> StageDuration = {24.f * 3600.f, 72.f * 3600.f, 48.f * 3600.f};  // Seconds (real-time hours)

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Disease")
    TArray<float> DexterityModifier = {0.f, -5.f, -10.f};  // Stage effects

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Disease")
    TArray<float> StrengthModifier = {0.f, 0.f, -5.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Disease")
    TArray<float> SpreadChance = {0.f, 0.05f, 0.15f};  // Probability to spread
};