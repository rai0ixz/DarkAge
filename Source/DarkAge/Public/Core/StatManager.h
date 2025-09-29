#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/PlayerSaveGame.h"
#include "Engine/DataTable.h"
#include "StatManager.generated.h"

USTRUCT(BlueprintType)
struct FTrackedStat
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StatName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;
};

UCLASS()
class DARKAGE_API UStatManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	   virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddStat(FName StatName, int32 Amount = 1);
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetStat(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	const TArray<FTrackedStat>& GetAllStats() const;
private:
    UPROPERTY()
    UDataTable* SkillDefinitionsDataTable;

	UPROPERTY()
	TArray<FTrackedStat> Stats;
};
