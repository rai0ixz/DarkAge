#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvancedPlayerAbilitiesComponent.generated.h"

class ACharacter;
class UStatlineComponent;
class UPlayerSkillsComponent;
class UInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UAdvancedPlayerAbilitiesComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAdvancedPlayerAbilitiesComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Combat Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities|Combat")
    bool ActivatePowerAttack();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Combat")
    bool ActivateDodge();

    // Movement Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    bool ActivateSprint();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    void DeactivateSprint();

    // Stealth Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities|Stealth")
    bool ActivateStealth();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Stealth")
    void DeactivateStealth();

    // Social Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities|Social")
    bool ActivateIntimidate();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Social")
    bool ActivateInspire();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Social")
    void DeactivateInspire();

    // Utility Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities|Utility")
    bool ActivateAnalyze();

    UFUNCTION(BlueprintCallable, Category = "Abilities|Utility")
    bool ActivateAdvancedCrafting();

    // Query Functions
    UFUNCTION(BlueprintPure, Category = "Abilities")
    bool CanUseAbility(const FString& AbilityName, float StaminaCost) const;

    UFUNCTION(BlueprintPure, Category = "Abilities")
    float GetAbilityCooldown(const FString& AbilityName) const;

    UFUNCTION(BlueprintPure, Category = "Abilities")
    bool IsAbilityActive(const FString& AbilityName) const;

    // State Getters
    UFUNCTION(BlueprintPure, Category = "Abilities|State")
    bool IsSprintActive() const;

    UFUNCTION(BlueprintPure, Category = "Abilities|State")
    bool IsStealthActive() const;

    UFUNCTION(BlueprintPure, Category = "Abilities|State")
    float GetStealthRemainingTime() const;

    UFUNCTION(BlueprintPure, Category = "Abilities|State")
    bool IsInspireActive() const;

    UFUNCTION(BlueprintPure, Category = "Abilities|State")
    float GetInspireRemainingTime() const;

protected:
    // Component References
    UPROPERTY()
    ACharacter* OwnerCharacter;

    UPROPERTY()
    UStatlineComponent* StatlineComponent;

    UPROPERTY()
    UPlayerSkillsComponent* SkillsComponent;

    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    // Ability Cooldowns
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TMap<FString, float> AbilityCooldowns;

    // Ability States
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|State")
    bool bIsSprintActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|State")
    bool bIsStealthActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|State")
    bool bIsInspireActive;

    // Ability Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Movement")
    float SprintSpeedMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Movement")
    float SprintStaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Combat")
    float DodgeDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Combat")
    float DodgeStaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stealth")
    float StealthDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stealth")
    float StealthMovementPenalty;

    // Internal State
    float DefaultWalkSpeed;
    float StealthRemainingTime;
    float InspireRemainingTime;

    // Update Functions
    void UpdateAbilityCooldowns(float DeltaTime);
    void UpdateActiveAbilities(float DeltaTime);
    void HandleStaminaRegeneration(float DeltaTime);

    // Effect Implementation Functions
    void ApplyPowerAttackEffects();
    void ApplyDodgeMovement();
    void ApplyStealthEffects();
    void RemoveStealthEffects();
    void ApplyIntimidationEffects();
    void ApplyInspirationEffects();
    void RemoveInspirationEffects();
    void PerformEnvironmentalAnalysis();
    void EnableAdvancedCraftingMode();
};