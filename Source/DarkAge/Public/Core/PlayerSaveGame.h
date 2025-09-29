#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "GameFramework/SaveGame.h"
#include "Components/StatlineComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/DAQuestLogComponent.h"
#include "Data/QuestData.h"
#include "Data/PlayerSkillData.h"
#include "Data/InventoryData.h"
#include "PlayerSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStat
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName StatName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FPlayerAchievement
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AchievementID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUnlocked = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime UnlockTime;
};

UCLASS()
class DARKAGE_API UPlayerSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PlayerName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerLevel = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FStat> PlayerStats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPlayerAchievement> Achievements;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LastPlayedMap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastSaveTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FInventoryItem> InventoryItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PlayerLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator PlayerRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FQuestLogEntry> QuestLog;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FPlayerSkillInstanceData> PlayerSkills;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<FString> DiscoveredRegions;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WorldTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> RegionWeatherMap;
};
