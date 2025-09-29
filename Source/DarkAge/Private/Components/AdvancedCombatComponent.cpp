#include "Components/AdvancedCombatComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UAdvancedCombatComponent::UAdvancedCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentComboState = EAdvancedComboState::Idle;
    ComboTimer = 0.0f;
    ComboWindow = 1.5f; // 1.5 seconds to continue combo
    CombatLevel = 1;
    ExperiencePoints = 0.0f;
}

void UAdvancedCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeWeaponSkills();
}

void UAdvancedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateComboTimer(DeltaTime);
    UpdateTacticalAwareness();
}

void UAdvancedCombatComponent::ExecuteComboAttack()
{
    if (!CanExecuteCombo())
    {
        return;
    }

    ExecuteComboAttackInternal();
    AdvanceCombo();
}

void UAdvancedCombatComponent::ExecuteSpecialMove(const FString& MoveName)
{
    // Execute special moves based on weapon skills
    FWeaponSkill* Skill = FindWeaponSkill(MoveName);
    if (Skill && Skill->bUnlocked)
    {
        // Calculate effective damage with skill modifiers
        float BaseDamage = 20.0f; // Base damage for special moves
        float EffectiveDamage = CalculateEffectiveDamage(BaseDamage, *Skill);

        // Apply tactical and environmental bonuses
        EffectiveDamage *= CalculateTacticalBonus();
        EffectiveDamage *= CalculateEnvironmentalBonus();

        // Execute the special move
        UE_LOG(LogTemp, Log, TEXT("Executed special move: %s for %.1f damage"), *MoveName, EffectiveDamage);

        OnWeaponSkillUsed.Broadcast(*Skill);
        GainCombatExperience(EffectiveDamage * 0.1f);
    }
}

void UAdvancedCombatComponent::ChangeTacticalPosition(ETacticalPosition NewPosition)
{
    if (CurrentTactics.CurrentPosition != NewPosition)
    {
        CurrentTactics.CurrentPosition = NewPosition;
        CalculateTacticalBonuses();

        OnTacticalPositionChanged.Broadcast(NewPosition);

        UE_LOG(LogTemp, Log, TEXT("Changed tactical position to: %s"),
            *UEnum::GetValueAsString(NewPosition));
    }
}

void UAdvancedCombatComponent::UseWeaponSkill(const FString& SkillName)
{
    FWeaponSkill* Skill = FindWeaponSkill(SkillName);
    if (Skill && Skill->bUnlocked && CombatLevel >= Skill->RequiredLevel)
    {
        ExecuteSpecialMove(SkillName);
    }
}

void UAdvancedCombatComponent::AdvanceCombo()
{
    switch (CurrentComboState)
    {
    case EAdvancedComboState::Idle:
        SetComboState(EAdvancedComboState::Combo1);
        break;
    case EAdvancedComboState::Combo1:
        SetComboState(EAdvancedComboState::Combo2);
        break;
    case EAdvancedComboState::Combo2:
        SetComboState(EAdvancedComboState::Combo3);
        break;
    case EAdvancedComboState::Combo3:
        SetComboState(EAdvancedComboState::Finishing);
        HandleComboFinisher();
        break;
    default:
        ResetCombo();
        break;
    }
}

void UAdvancedCombatComponent::ResetCombo()
{
    SetComboState(EAdvancedComboState::Idle);
}

void UAdvancedCombatComponent::UpdateTacticalAwareness()
{
    // Update nearby allies and enemies
    FVector Location = GetOwner()->GetActorLocation();

    // Find nearby actors (simplified - in practice would use more sophisticated queries)
    TArray<AActor*> NearbyActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), NearbyActors);

    CurrentTactics.NearbyAllies.Empty();
    CurrentTactics.NearbyEnemies.Empty();

    for (AActor* Actor : NearbyActors)
    {
        if (Actor == GetOwner()) continue;

        float Distance = FVector::Dist(Location, Actor->GetActorLocation());
        if (Distance <= 1000.0f) // Within 10 meters
        {
            // Simplified classification - would use team/faction system in practice
            if (Actor->GetName().Contains("Ally") || Actor->GetName().Contains("Friend"))
            {
                CurrentTactics.NearbyAllies.Add(Actor);
            }
            else if (Actor->GetName().Contains("Enemy") || Actor->GetName().Contains("Bandit"))
            {
                CurrentTactics.NearbyEnemies.Add(Actor);
            }
        }
    }

    CalculateTacticalBonuses();
}

void UAdvancedCombatComponent::UnlockWeaponSkill(const FString& SkillName)
{
    FWeaponSkill* Skill = FindWeaponSkill(SkillName);
    if (Skill && CombatLevel >= Skill->RequiredLevel)
    {
        Skill->bUnlocked = true;
        UE_LOG(LogTemp, Log, TEXT("Unlocked weapon skill: %s"), *SkillName);
    }
}

bool UAdvancedCombatComponent::IsWeaponSkillUnlocked(const FString& SkillName) const
{
    const FWeaponSkill* Skill = nullptr;
    for (const FWeaponSkill& WeaponSkill : WeaponSkills)
    {
        if (WeaponSkill.SkillName == SkillName)
        {
            Skill = &WeaponSkill;
            break;
        }
    }
    return Skill ? Skill->bUnlocked : false;
}

float UAdvancedCombatComponent::CalculateEffectiveDamage(float BaseDamage, const FWeaponSkill& Skill)
{
    float EffectiveDamage = BaseDamage * Skill.DamageMultiplier;

    // Apply combo bonuses
    switch (CurrentComboState)
    {
    case EAdvancedComboState::Combo2:
        EffectiveDamage *= 1.2f;
        break;
    case EAdvancedComboState::Combo3:
        EffectiveDamage *= 1.4f;
        break;
    case EAdvancedComboState::Finishing:
        EffectiveDamage *= 2.0f;
        break;
    }

    return EffectiveDamage;
}

float UAdvancedCombatComponent::CalculateTacticalBonus()
{
    float Bonus = 1.0f;

    switch (CurrentTactics.CurrentPosition)
    {
    case ETacticalPosition::Aggressive:
        Bonus = 1.3f; // Higher damage, lower defense
        break;
    case ETacticalPosition::Defensive:
        Bonus = 0.8f; // Lower damage, higher defense
        break;
    case ETacticalPosition::Flanking:
        Bonus = 1.5f; // High damage from side/rear
        break;
    case ETacticalPosition::Retreating:
        Bonus = 0.6f; // Very low damage, high mobility
        break;
    }

    // Ally support bonus
    if (CurrentTactics.NearbyAllies.Num() > 0)
    {
        Bonus *= (1.0f + CurrentTactics.NearbyAllies.Num() * 0.1f);
    }

    return Bonus;
}

float UAdvancedCombatComponent::CalculateEnvironmentalBonus()
{
    // Calculate environmental factors
    FVector Location = GetOwner()->GetActorLocation();

    // Height advantage
    if (Location.Z > 500.0f) // High ground
    {
        return 1.2f;
    }

    // Cover bonus (simplified)
    // In practice, would raycast to detect cover

    return 1.0f;
}

void UAdvancedCombatComponent::InitializeWeaponSkills()
{
    // Initialize basic weapon skills
    FWeaponSkill BasicSlash;
    BasicSlash.SkillName = "Basic Slash";
    BasicSlash.Description = "A fundamental sword technique";
    BasicSlash.RequiredLevel = 1;
    BasicSlash.DamageMultiplier = 1.0f;
    BasicSlash.StaminaCost = 10.0f;
    BasicSlash.bUnlocked = true;
    WeaponSkills.Add(BasicSlash);

    FWeaponSkill PowerStrike;
    PowerStrike.SkillName = "Power Strike";
    PowerStrike.Description = "A powerful overhead strike";
    PowerStrike.RequiredLevel = 3;
    PowerStrike.DamageMultiplier = 1.5f;
    PowerStrike.StaminaCost = 25.0f;
    WeaponSkills.Add(PowerStrike);

    FWeaponSkill Whirlwind;
    Whirlwind.SkillName = "Whirlwind";
    Whirlwind.Description = "Spin attack hitting multiple enemies";
    Whirlwind.RequiredLevel = 5;
    Whirlwind.DamageMultiplier = 0.8f; // Lower per-target but hits multiple
    Whirlwind.StaminaCost = 30.0f;
    WeaponSkills.Add(Whirlwind);

    FWeaponSkill Riposte;
    Riposte.SkillName = "Riposte";
    Riposte.Description = "Counter-attack after successful defense";
    Riposte.RequiredLevel = 4;
    Riposte.DamageMultiplier = 2.0f;
    Riposte.StaminaCost = 15.0f;
    WeaponSkills.Add(Riposte);
}

void UAdvancedCombatComponent::UpdateComboTimer(float DeltaTime)
{
    if (CurrentComboState != EAdvancedComboState::Idle && CurrentComboState != EAdvancedComboState::Stunned)
    {
        ComboTimer += DeltaTime;
        if (ComboTimer >= ComboWindow)
        {
            ResetCombo();
        }
    }
}

void UAdvancedCombatComponent::ExecuteComboAttackInternal()
{
    float BaseDamage = 15.0f; // Base combo damage
    float EffectiveDamage = CalculateEffectiveDamage(BaseDamage, WeaponSkills[0]); // Use basic skill
    EffectiveDamage *= CalculateTacticalBonus();
    EffectiveDamage *= CalculateEnvironmentalBonus();

    UE_LOG(LogTemp, Log, TEXT("Executed combo attack for %.1f damage (State: %s)"),
        EffectiveDamage, *UEnum::GetValueAsString(CurrentComboState));

    GainCombatExperience(EffectiveDamage * 0.05f);
}

void UAdvancedCombatComponent::CalculateTacticalBonuses()
{
    CurrentTactics.PositionBonus = CalculateTacticalBonus();
    CurrentTactics.EnvironmentalBonus = CalculateEnvironmentalBonus();
}

void UAdvancedCombatComponent::ApplyEnvironmentalEffects()
{
    // Apply ongoing environmental effects
    // This could include weather, terrain, time of day effects
}

FWeaponSkill* UAdvancedCombatComponent::FindWeaponSkill(const FString& SkillName)
{
    for (FWeaponSkill& Skill : WeaponSkills)
    {
        if (Skill.SkillName == SkillName)
        {
            return &Skill;
        }
    }
    return nullptr;
}

void UAdvancedCombatComponent::GainCombatExperience(float XP)
{
    ExperiencePoints += XP;

    // Level up check (simplified)
    int32 XPForNextLevel = CombatLevel * 100;
    if (ExperiencePoints >= XPForNextLevel)
    {
        CombatLevel++;
        ExperiencePoints -= XPForNextLevel;

        // Unlock new skills based on level
        if (CombatLevel == 3)
        {
            UnlockWeaponSkill("Power Strike");
        }
        else if (CombatLevel == 4)
        {
            UnlockWeaponSkill("Riposte");
        }
        else if (CombatLevel == 5)
        {
            UnlockWeaponSkill("Whirlwind");
        }

        UE_LOG(LogTemp, Log, TEXT("Combat level increased to %d!"), CombatLevel);
    }
}

bool UAdvancedCombatComponent::CanExecuteCombo() const
{
    // Check stamina, weapon equipped, not stunned, etc.
    return CurrentComboState != EAdvancedComboState::Stunned;
}

void UAdvancedCombatComponent::HandleComboFinisher()
{
    // Execute finishing move with bonus effects
    UE_LOG(LogTemp, Log, TEXT("COMBO FINISHER EXECUTED!"));

    // Special effects, bonus damage, etc.
    ResetCombo();
}

void UAdvancedCombatComponent::SetComboState(EAdvancedComboState NewState)
{
    if (CurrentComboState != NewState)
    {
        CurrentComboState = NewState;
        ComboTimer = 0.0f;

        OnComboStateChanged.Broadcast(NewState);
    }
}