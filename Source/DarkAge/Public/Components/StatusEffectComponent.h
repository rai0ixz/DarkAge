#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StatusEffectData.h"
#include "StatusEffectComponent.generated.h"

// Forward declaration
class UStatlineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UStatusEffectComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStatusEffectComponent();

    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
    virtual void BeginPlay() override;
    //~ End UActorComponent Interface

public:
    UFUNCTION(BlueprintCallable, Category = "Status Effects")
    void ApplyEffect(const FDataTableRowHandle& EffectDataRowHandle);

    UFUNCTION(BlueprintCallable, Category = "Status Effects")
    void RemoveEffect(FName EffectID);

    UFUNCTION(BlueprintPure, Category = "Status Effects")
    bool HasEffect(FName EffectID) const;

    UFUNCTION(BlueprintPure, Category = "Status Effects")
    const TArray<FActiveStatusEffect>& GetActiveEffects() const { return ActiveEffects; }

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectChanged OnStatusEffectAdded;

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectChanged OnStatusEffectRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectTick OnStatusEffectTicked;

private:
    void HandleEffectTick(FActiveStatusEffect& Effect, float DeltaTime);
    void ApplyStatModifiers(const FStatusEffectData* EffectData, bool bIsApplying);

    UPROPERTY(VisibleAnywhere, Category = "Status Effects")
    TArray<FActiveStatusEffect> ActiveEffects;

    UPROPERTY()
    TWeakObjectPtr<UStatlineComponent> StatlineComponent;
};