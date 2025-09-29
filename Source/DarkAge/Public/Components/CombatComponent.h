#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CombatTypes.h"
#include "CombatComponent.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Defending   UMETA(DisplayName = "Defending"),
    Dodging     UMETA(DisplayName = "Dodging"),
    Stunned     UMETA(DisplayName = "Stunned"),
    Dead        UMETA(DisplayName = "Dead")
};


USTRUCT(BlueprintType)
struct FCombatStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackPower = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DefensePower = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CriticalChance = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DodgeChance = 0.1f;

    // Default constructor uses in-class member initializers above
    FCombatStats() = default;

    // Parameterized constructor to support brace initialization at call sites
    FCombatStats(float InAttackPower, float InDefensePower, float InAttackSpeed, float InCriticalChance, float InDodgeChance)
        : AttackPower(InAttackPower)
        , DefensePower(InDefensePower)
        , AttackSpeed(InAttackSpeed)
        , CriticalChance(InCriticalChance)
        , DodgeChance(InDodgeChance)
    {}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Combat actions
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Attack(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Defend();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Dodge();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeDamage(float Damage, AActor* Instigator);

    // Combat state
    UFUNCTION(BlueprintPure, Category = "Combat")
    ECombatState GetCombatState() const { return CurrentCombatState; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCombatState(ECombatState NewState);

    // Weapon management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EquipWeapon(EWeaponType WeaponType);

    UFUNCTION(BlueprintPure, Category = "Combat")
    EWeaponType GetEquippedWeapon() const { return EquippedWeapon; }

    // Combat stats
    UFUNCTION(BlueprintPure, Category = "Combat")
    const FCombatStats& GetCombatStats() const { return CombatStats; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ModifyCombatStats(const FCombatStats& Modifier);

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, ECombatState, NewState);
    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnCombatStateChanged OnCombatStateChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageTaken, float, Damage, AActor*, Instigator);
    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnDamageTaken OnDamageTaken;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackPerformed, AActor*, Target);
    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnAttackPerformed OnAttackPerformed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnDeath OnDeath;

protected:
    // Combat state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    ECombatState CurrentCombatState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    EWeaponType EquippedWeapon;

    // Combat stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FCombatStats CombatStats;

    // Combat timing
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackCooldown;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float StateTimer;

    // Weapon stats modifiers
    TMap<EWeaponType, FCombatStats> WeaponModifiers;

    // Core combat functions
    void PerformAttack(AActor* Target);
    float CalculateDamage(AActor* Target) const;
    bool CheckHit(AActor* Target) const;
    bool CheckCritical() const;
    bool CheckDodge() const;

    void UpdateCombatState(float DeltaTime);
    void ResetAttackCooldown();

    // Weapon setup
    void InitializeWeaponModifiers();
    FCombatStats GetWeaponModifier(EWeaponType WeaponType) const;
};