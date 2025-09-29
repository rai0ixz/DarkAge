#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/AchievementManager.h"
#include "AchievementsMenuWidget.generated.h"

UCLASS()
class DARKAGE_API UAchievementsMenuWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Achievements")
    void RefreshAchievements(const TArray<FPlayerAchievement>& Achievements);
    UPROPERTY(BlueprintReadWrite, Category = "Achievements")
    UAchievementManager* AchievementManager;
};
