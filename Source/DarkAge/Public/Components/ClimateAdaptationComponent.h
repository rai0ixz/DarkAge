#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ClimateData.h"
#include "ClimateAdaptationComponent.generated.h"

USTRUCT(BlueprintType)
struct FClimateAdaptationEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Climate Adaptation")
    EClimateType Climate = EClimateType::Temperate;

    UPROPERTY(EditAnywhere, Category = "Climate Adaptation")
    float Level = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UClimateAdaptationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClimateAdaptationComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Replicated adaptation levels
	UPROPERTY(EditAnywhere, Category = "Climate Adaptation", ReplicatedUsing=OnRep_Adaptation)
	TArray<FClimateAdaptationEntry> AdaptationLevels;

    // Replicated current climate
    UPROPERTY(VisibleAnywhere, Category = "Climate Adaptation", ReplicatedUsing=OnRep_CurrentClimate)
    EClimateType CurrentClimate;

    // RepNotify for AdaptationLevels
    UFUNCTION()
    void OnRep_Adaptation();

    // RepNotify for CurrentClimate
    UFUNCTION()
    void OnRep_CurrentClimate();

public:
    // Client-side call, routes to server
    UFUNCTION(BlueprintCallable, Category = "Climate Adaptation")
    void UpdateAdaptation(float DeltaTime);
    // Server RPC for updating adaptation
    UFUNCTION(Server, Reliable)
    void ServerUpdateAdaptation(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Climate Adaptation")
    void ApplyClimateEffects();
    // Server RPC for applying climate effects
    UFUNCTION(Server, Reliable)
    void ServerApplyClimateEffects();

    UFUNCTION(BlueprintPure, Category = "Climate Adaptation")
    float GetAdaptationLevel(EClimateType Climate) const;

    UFUNCTION(BlueprintPure, Category = "Climate Adaptation")
    EClimateType GetCurrentClimate() const { return CurrentClimate; }

    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};