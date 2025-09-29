#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldPopulationSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWorldPopulationMode : uint8
{
    Manual     UMETA(DisplayName = "Manual"),
    Procedural UMETA(DisplayName = "Procedural")
};

USTRUCT(BlueprintType)
struct FWorldObjectInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> ActorInstance;

    FWorldObjectInfo()
    {
        Name = TEXT("");
        Location = FVector::ZeroVector;
        ActorClass = nullptr;
        ActorInstance = nullptr;
    }
};

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UWorldPopulationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UWorldPopulationSubsystem();

    /** If false, the entire world population simulation update will be skipped. Can be toggled with the 'ToggleWorldPopulation' console command. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    bool bPopulationEnabled = true;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "World Population")
    void SetPopulationMode(EWorldPopulationMode Mode);

    UFUNCTION(BlueprintCallable, Category = "World Population")
    EWorldPopulationMode GetPopulationMode() const;

    UFUNCTION(BlueprintCallable, Category = "World Population")
    void RegisterManualWorldObject(const FWorldObjectInfo& ObjectInfo);

    UFUNCTION(BlueprintCallable, Category = "World Population")
    void ProceduralGenerateWorldObjects();

    UFUNCTION(BlueprintCallable, Category = "World Population")
    TArray<FWorldObjectInfo> GetWorldObjectsByType(const FString& Type) const;

    UFUNCTION(BlueprintCallable, Category = "World Population")
    TArray<FWorldObjectInfo> GetAllWorldObjects() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    EWorldPopulationMode PopulationMode = EWorldPopulationMode::Manual;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    TArray<FWorldObjectInfo> Towns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    TArray<FWorldObjectInfo> WaterSources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    TArray<FWorldObjectInfo> Housing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    TArray<FWorldObjectInfo> FoodSources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Population")
    TArray<FWorldObjectInfo> AllWorldObjects;

    void ScanForManualWorldObjects();
    void SpawnProceduralWorldObjects();
    void RegisterWorldObject(const FWorldObjectInfo& ObjectInfo);
};
