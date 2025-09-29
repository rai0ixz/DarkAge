// Copyright (c) 2025 RaioCore
// Helper UObject for StatlineComponent test delegate binding
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StatlineComponentTestListener.generated.h"

UCLASS()
class UStatlineComponentTestListener : public UObject
{
    GENERATED_BODY()
public:
    bool bDeathFired = false;

    UFUNCTION()
    void OnDeathHandler() { bDeathFired = true; }
};
