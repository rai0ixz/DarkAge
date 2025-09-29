#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StatusEffectData.h"
#include "StatlineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, FName, StatName, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UENUM(BlueprintType)
enum class EStatCategory : uint8
{
    Primary,
    Survival,
    Combat,
    Generic
};

USTRUCT(BlueprintType)
struct FStat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float BaseValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
    float CurrentValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    EStatCategory Category;

    FStat() : BaseValue(100.0f), CurrentValue(100.0f), Category(EStatCategory::Generic) {}

    FStat(float InBaseValue, float InCurrentValue, EStatCategory InCategory)
        : BaseValue(InBaseValue), CurrentValue(InCurrentValue), Category(InCategory) {}
};

USTRUCT(BlueprintType)
struct FStatEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    FName StatName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    FStat Stat;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UStatlineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStatlineComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void UpdateStat(FName StatName, float Delta);

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentStatValue(FName StatName) const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetBaseStatValue(FName StatName) const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxStatValue(FName StatName) const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetStatPercentage(FName StatName) const;

    UFUNCTION(BlueprintCallable, Category = "Status Effects")
    void ApplyStatusEffect(const FDataTableRowHandle& EffectDataRowHandle, UObject* Instigator);

    UFUNCTION(BlueprintCallable, Category = "Status Effects")
    void RemoveStatusEffect(FName EffectID);


    UPROPERTY(BlueprintAssignable, Category = "Stats")
    FOnStatChanged OnStatChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stats")
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectChanged OnStatusEffectChanged;

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectTick OnStatusEffectTick;

    UPROPERTY(BlueprintAssignable, Category = "Status Effects")
    FOnStatusEffectExpired OnStatusEffectExpired;


    void RecalculateStat(FName StatName);
    void ProcessStatusEffects(float DeltaTime);

    TArray<FStatEntry>& GetMutableStats() { return Stats; }

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UDADamageTextWidget> DamageTextWidgetClass;

    // Replicated stats
    UPROPERTY(EditAnywhere, Category = "Stats", ReplicatedUsing=OnRep_Stats)
    TArray<FStatEntry> Stats;

    // RepNotify for Stats
    UFUNCTION()
    void OnRep_Stats();
    // Server RPC for updating stat
    UFUNCTION(Server, Reliable)
    void ServerUpdateStat(FName StatName, float Delta);
    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UFUNCTION(BlueprintCallable, Category = "Stats")
    const TArray<FStatEntry>& GetAllStats() const { return Stats; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetStats(const TArray<FStatEntry>& InStats);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void PrintStats();

    // Convenience methods for common stats
    UFUNCTION(BlueprintPure, Category = "Stats|Convenience")
    float GetCurrentStamina() const;
    
    UFUNCTION(BlueprintCallable, Category = "Stats|Convenience")
    void ModifyStamina(float Delta);
    
    UFUNCTION(BlueprintPure, Category = "Stats|Convenience")
    float GetCurrentHealth() const;
    
    UFUNCTION(BlueprintCallable, Category = "Stats|Convenience")
    void ModifyHealth(float Delta);
    
    UFUNCTION(BlueprintPure, Category = "Stats|Convenience")
    float GetCurrentMana() const;

    UFUNCTION(BlueprintCallable, Category = "Stats|Convenience")
    void ModifyMana(float Delta);

    UFUNCTION(BlueprintPure, Category = "Stats|Convenience")
    float GetCurrentHunger() const;

    UFUNCTION(BlueprintCallable, Category = "Stats|Convenience")
    void ModifyHunger(float Delta);

    UFUNCTION(BlueprintPure, Category = "Stats|Convenience")
    float GetCurrentThirst() const;

    UFUNCTION(BlueprintCallable, Category = "Stats|Convenience")
    void ModifyThirst(float Delta);

    UPROPERTY(VisibleAnywhere, Category = "Status Effects")
    TArray<FActiveStatusEffect> ActiveStatusEffects;
};