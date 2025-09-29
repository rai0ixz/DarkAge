#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/PlayerSaveGame.h"
#include "AchievementManager.generated.h"

UCLASS()
class DARKAGE_API UAchievementManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void UnlockAchievement(FName AchievementID);
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    bool IsAchievementUnlocked(FName AchievementID) const;
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    const TArray<FPlayerAchievement>& GetAllAchievements() const;
private:
    UPROPERTY()
    TArray<FPlayerAchievement> Achievements;
};
