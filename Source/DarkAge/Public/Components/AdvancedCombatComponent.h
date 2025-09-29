#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvancedCombatComponent.generated.h"

UENUM(BlueprintType)
enum class EAdvancedComboState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Combo1      UMETA(DisplayName = "Combo 1"),
    Combo2      UMETA(DisplayName = "Combo 2"),
    Combo3      UMETA(DisplayName = "Combo 3"),
    Finishing   UMETA(DisplayName = "Finishing Move"),
    Stunned     UMETA(DisplayName = "Stunned")
};

UENUM(BlueprintType)
enum class ETacticalPosition : uint8
{
    Neutral     UMETA(DisplayName = "Neutral"),
    Aggressive  UMETA(DisplayName = "Aggressive"),
    Defensive   UMETA(DisplayName = "Defensive"),
    Flanking    UMETA(DisplayName = "Flanking"),
    Retreating  UMETA(DisplayName = "Retreating")
};

USTRUCT(BlueprintType)
struct FWeaponSkill
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float DamageMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float StaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    TArray<FString> ComboSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    bool bUnlocked;

    FWeaponSkill()
    {
        RequiredLevel = 1;
        DamageMultiplier = 1.0f;
        StaminaCost = 10.0f;
        bUnlocked = false;
    }
};

USTRUCT(BlueprintType)
struct FCombatTactics
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tactics")
    ETacticalPosition CurrentPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tactics")
    float PositionBonus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tactics")
    TArray<AActor*> NearbyAllies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tactics")
    TArray<AActor*> NearbyEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tactics")
    float EnvironmentalBonus;

    FCombatTactics()
    {
        CurrentPosition = ETacticalPosition::Neutral;
        PositionBonus = 1.0f;
        EnvironmentalBonus = 1.0f;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UAdvancedCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAdvancedCombatComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Advanced combat actions
    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void ExecuteComboAttack();

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void ExecuteSpecialMove(const FString& MoveName);

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void ChangeTacticalPosition(ETacticalPosition NewPosition);

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void UseWeaponSkill(const FString& SkillName);

    // Combo system
    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    EAdvancedComboState GetCurrentComboState() const { return CurrentComboState; }

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void AdvanceCombo();

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void ResetCombo();

    // Tactical system
    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    const FCombatTactics& GetCurrentTactics() const { return CurrentTactics; }

    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void UpdateTacticalAwareness();

    // Weapon skills
    UFUNCTION(BlueprintCallable, Category = "Advanced Combat")
    void UnlockWeaponSkill(const FString& SkillName);

    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    bool IsWeaponSkillUnlocked(const FString& SkillName) const;

    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    const TArray<FWeaponSkill>& GetAvailableSkills() const { return WeaponSkills; }

    // Combat calculations
    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    float CalculateEffectiveDamage(float BaseDamage, const FWeaponSkill& Skill);

    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    float CalculateTacticalBonus();

    UFUNCTION(BlueprintPure, Category = "Advanced Combat")
    float CalculateEnvironmentalBonus();

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboStateChanged, EAdvancedComboState, NewState);
    UPROPERTY(BlueprintAssignable, Category = "Advanced Combat Events")
    FOnComboStateChanged OnComboStateChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTacticalPositionChanged, ETacticalPosition, NewPosition);
    UPROPERTY(BlueprintAssignable, Category = "Advanced Combat Events")
    FOnTacticalPositionChanged OnTacticalPositionChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSkillUsed, const FWeaponSkill&, Skill);
    UPROPERTY(BlueprintAssignable, Category = "Advanced Combat Events")
    FOnWeaponSkillUsed OnWeaponSkillUsed;

protected:
    // Combo system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Combat")
    EAdvancedComboState CurrentComboState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Combat")
    float ComboTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Combat")
    float ComboWindow;

    // Tactical system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Combat")
    FCombatTactics CurrentTactics;

    // Weapon skills
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Combat")
    TArray<FWeaponSkill> WeaponSkills;

    // Combat stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Combat")
    int32 CombatLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Combat")
    float ExperiencePoints;

    // Core functions
    void InitializeWeaponSkills();
    void UpdateComboTimer(float DeltaTime);
    void ExecuteComboAttackInternal();
    void CalculateTacticalBonuses();
    void ApplyEnvironmentalEffects();
    FWeaponSkill* FindWeaponSkill(const FString& SkillName);
    void GainCombatExperience(float XP);
    bool CanExecuteCombo() const;
    void HandleComboFinisher();
    void SetComboState(EAdvancedComboState NewState);
};