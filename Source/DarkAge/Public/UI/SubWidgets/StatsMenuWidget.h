#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/StatManager.h"
#include "StatsMenuWidget.generated.h"

UCLASS()
class DARKAGE_API UStatsMenuWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Stats")
    void RefreshStats(const TArray<FTrackedStat>& Stats);
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    UStatManager* StatManager;
};
