#include "Components/CombatComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Components/StatlineComponent.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentCombatState = ECombatState::Idle;
    EquippedWeapon = EWeaponType::Unarmed;
    AttackCooldown = 0.0f;
    StateTimer = 0.0f;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeWeaponModifiers();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update combat timers
    if (AttackCooldown > 0.0f)
    {
        AttackCooldown -= DeltaTime;
    }

    if (StateTimer > 0.0f)
    {
        StateTimer -= DeltaTime;
        if (StateTimer <= 0.0f)
        {
            // State timer expired, return to idle
            SetCombatState(ECombatState::Idle);
        }
    }

    UpdateCombatState(DeltaTime);
}

void UCombatComponent::Attack(AActor* Target)
{
    if (!Target || CurrentCombatState != ECombatState::Idle || AttackCooldown > 0.0f)
    {
        return;
    }

    PerformAttack(Target);
    OnAttackPerformed.Broadcast(Target);
}

void UCombatComponent::Defend()
{
    if (CurrentCombatState == ECombatState::Idle)
    {
        SetCombatState(ECombatState::Defending);
        StateTimer = 3.0f; // Defend for 3 seconds
    }
}

void UCombatComponent::Dodge()
{
    if (CurrentCombatState == ECombatState::Idle)
    {
        SetCombatState(ECombatState::Dodging);
        StateTimer = 0.5f; // Dodge for 0.5 seconds
    }
}

void UCombatComponent::TakeDamage(float Damage, AActor* Instigator)
{
    if (CurrentCombatState == ECombatState::Dead)
    {
        return;
    }

    // Apply defense modifiers
    float ActualDamage = Damage;

    if (CurrentCombatState == ECombatState::Defending)
    {
        ActualDamage *= 0.5f; // 50% damage reduction when defending
    }
    else if (CurrentCombatState == ECombatState::Dodging)
    {
        if (CheckDodge())
        {
            ActualDamage = 0.0f; // Complete dodge
        }
    }

    // Apply to health if we have a statline component
    AActor* Owner = GetOwner();
    if (Owner)
    {
        ADAPlayerCharacter* PlayerChar = Cast<ADAPlayerCharacter>(Owner);
        if (PlayerChar && PlayerChar->GetStatlineComponent())
        {
            PlayerChar->GetStatlineComponent()->UpdateStat(FName("Health"), -ActualDamage);
            float CurrentHealth = PlayerChar->GetStatlineComponent()->GetCurrentStatValue(FName("Health"));

            if (CurrentHealth <= 0.0f)
            {
                SetCombatState(ECombatState::Dead);
                OnDeath.Broadcast();
            }
        }
    }

    OnDamageTaken.Broadcast(ActualDamage, Instigator);

    UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage from %s"),
        *GetOwner()->GetName(), ActualDamage, Instigator ? *Instigator->GetName() : TEXT("Unknown"));
}

void UCombatComponent::SetCombatState(ECombatState NewState)
{
    if (CurrentCombatState != NewState)
    {
        CurrentCombatState = NewState;
        OnCombatStateChanged.Broadcast(NewState);

        // Reset state timer for temporary states
        if (NewState == ECombatState::Attacking || NewState == ECombatState::Defending ||
            NewState == ECombatState::Dodging || NewState == ECombatState::Stunned)
        {
            StateTimer = 1.0f; // Default 1 second for most states
        }
    }
}

void UCombatComponent::EquipWeapon(EWeaponType WeaponType)
{
    EquippedWeapon = WeaponType;

    // Apply weapon modifiers to combat stats
    FCombatStats WeaponMod = GetWeaponModifier(WeaponType);
    CombatStats = WeaponMod; // This should be additive, not replacement

    UE_LOG(LogTemp, Log, TEXT("%s equipped %s"),
        *GetOwner()->GetName(), *UEnum::GetValueAsString(WeaponType));
}

void UCombatComponent::ModifyCombatStats(const FCombatStats& Modifier)
{
    CombatStats.AttackPower += Modifier.AttackPower;
    CombatStats.DefensePower += Modifier.DefensePower;
    CombatStats.AttackSpeed += Modifier.AttackSpeed;
    CombatStats.CriticalChance += Modifier.CriticalChance;
    CombatStats.DodgeChance += Modifier.DodgeChance;
}

void UCombatComponent::PerformAttack(AActor* Target)
{
    SetCombatState(ECombatState::Attacking);

    if (CheckHit(Target))
    {
        float Damage = CalculateDamage(Target);
        UCombatComponent* TargetCombat = Target->FindComponentByClass<UCombatComponent>();
        if (TargetCombat)
        {
            TargetCombat->TakeDamage(Damage, GetOwner());
        }
    }

    // Set attack cooldown based on weapon and attack speed
    AttackCooldown = 1.0f / CombatStats.AttackSpeed;
}

float UCombatComponent::CalculateDamage(AActor* Target) const
{
    float BaseDamage = CombatStats.AttackPower;

    // Apply weapon modifier
    FCombatStats WeaponMod = GetWeaponModifier(EquippedWeapon);
    BaseDamage += WeaponMod.AttackPower;

    // Critical hit
    if (CheckCritical())
    {
        BaseDamage *= 2.0f;
        UE_LOG(LogTemp, Log, TEXT("Critical hit!"));
    }

    // Random variance (±20%)
    BaseDamage *= FMath::RandRange(0.8f, 1.2f);

    return BaseDamage;
}

bool UCombatComponent::CheckHit(AActor* Target) const
{
    // Simple hit chance calculation
    float HitChance = 0.8f; // 80% base hit chance

    // Distance affects hit chance
    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Target->GetActorLocation());
    if (Distance > 200.0f)
    {
        HitChance *= 0.7f; // 30% penalty at long range
    }

    return FMath::FRand() < HitChance;
}

bool UCombatComponent::CheckCritical() const
{
    return FMath::FRand() < CombatStats.CriticalChance;
}

bool UCombatComponent::CheckDodge() const
{
    return FMath::FRand() < CombatStats.DodgeChance;
}

void UCombatComponent::UpdateCombatState(float DeltaTime)
{
    // Handle state-specific updates
    switch (CurrentCombatState)
    {
    case ECombatState::Attacking:
        // Attack animation/logic would go here
        break;

    case ECombatState::Defending:
        // Defense bonus active
        break;

    case ECombatState::Dodging:
        // Dodge movement would go here
        break;

    case ECombatState::Stunned:
        // Cannot act while stunned
        break;

    case ECombatState::Dead:
        // Handle death state
        break;

    default:
        break;
    }
}

void UCombatComponent::ResetAttackCooldown()
{
    AttackCooldown = 0.0f;
}

void UCombatComponent::InitializeWeaponModifiers()
{
    // Initialize weapon stat modifiers
    WeaponModifiers.Add(EWeaponType::Unarmed, FCombatStats{5.0f, 2.0f, 1.0f, 0.05f, 0.1f});
    WeaponModifiers.Add(EWeaponType::Sword, FCombatStats{15.0f, 8.0f, 1.2f, 0.08f, 0.12f});
    WeaponModifiers.Add(EWeaponType::Axe, FCombatStats{20.0f, 5.0f, 0.8f, 0.1f, 0.08f});
    WeaponModifiers.Add(EWeaponType::Spear, FCombatStats{12.0f, 10.0f, 1.0f, 0.06f, 0.15f});
    WeaponModifiers.Add(EWeaponType::Bow, FCombatStats{18.0f, 3.0f, 0.9f, 0.12f, 0.05f});
    WeaponModifiers.Add(EWeaponType::Shield, FCombatStats{8.0f, 15.0f, 0.7f, 0.03f, 0.2f});
}

FCombatStats UCombatComponent::GetWeaponModifier(EWeaponType WeaponType) const
{
    const FCombatStats* Modifier = WeaponModifiers.Find(WeaponType);
    return Modifier ? *Modifier : FCombatStats();
}