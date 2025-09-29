#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldEpochSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FEpochData
{
    GENERATED_BODY();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EpochName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Objectives;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEpochAdvanced, const FEpochData&, NewEpoch);

UCLASS()
class DARKAGE_API UWorldEpochSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Epoch")
    void AdvanceAge();
    UFUNCTION(BlueprintCallable, Category = "Epoch")
    FEpochData GetCurrentEpoch() const;

    UPROPERTY(BlueprintAssignable, Category = "Epoch")
    FOnEpochAdvanced OnEpochAdvanced;
protected:
    UPROPERTY()
    TArray<FEpochData> Epochs;
    UPROPERTY()
    int32 CurrentEpochIndex = 0;
};
