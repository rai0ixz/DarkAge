#include "DarkAge/Public/Core/NextGenCombatSubsystem.h"
#include "Core/NextGenCombatSubsystem.h"

UNextGenCombatSubsystem::UNextGenCombatSubsystem()
{
    // Constructor logic here
}

void UNextGenCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Initialization logic here
}

void UNextGenCombatSubsystem::Deinitialize()
{
    // Deinitialization logic here
    Super::Deinitialize();
}

#include "DarkAge/Public/Components/StatlineComponent.h"
#include "Components/StatlineComponent.h"
#include "GameFramework/Actor.h"

#include "Components/PlayerSkillsComponent.h" // For skill/perk integration

void UNextGenCombatSubsystem::Tick(float DeltaTime)
{
    // Tick logic here
}

FDamageResult UNextGenCombatSubsystem::ProcessDamage(AActor* Attacker, AActor* Victim, float BaseDamage)
{
    FDamageResult Result = { false, 0.f, false, false };

    if (!Attacker || !Victim)
    {
        return Result;
    }

    UStatlineComponent* AttackerStats = Attacker->FindComponentByClass<UStatlineComponent>();
    UStatlineComponent* VictimStats = Victim->FindComponentByClass<UStatlineComponent>();

    // Get the attacker's skills component
    UPlayerSkillsComponent* AttackerSkills = Attacker->FindComponentByClass<UPlayerSkillsComponent>();

    if (!AttackerStats || !VictimStats)
    {
        return Result;
    }

    float AttackerStrength = AttackerStats->GetCurrentStatValue(FName("Strength"));
    float VictimDefense = VictimStats->GetCurrentStatValue(FName("Defense"));
    float AttackerCritChance = AttackerStats->GetCurrentStatValue(FName("CriticalHitChance"));
    float AttackerCritDamage = AttackerStats->GetCurrentStatValue(FName("CriticalHitDamage"));

    // --- Skill/Perk Integration Start ---
    float SkillDamageBonus = 0.f;
    float SkillCritBonus = 0.f;
    float SkillCritDamageBonus = 0.f;
    if (AttackerSkills)
    {
        // Example: Use "OneHanded" skill for melee, "Archery" for ranged, etc.
        // For demonstration, we'll use "OneHanded". Replace with logic as needed.
        int32 OneHandedLevel = AttackerSkills->GetSkillLevel(FName("OneHanded"));
        if (OneHandedLevel > 0)
        {
            SkillDamageBonus += OneHandedLevel * 0.02f * BaseDamage; // +2% per level
            SkillCritBonus += OneHandedLevel * 0.001f; // +0.1% crit chance per level
            SkillCritDamageBonus += OneHandedLevel * 0.01f; // +1% crit damage per level
        }

        // Example: Check for a perk that increases crit chance
        if (AttackerSkills->HasPerk(FName("OneHanded"), FName("OH_CritMaster")))
        {
            SkillCritBonus += 0.05f; // +5% crit chance
        }
        // Example: Check for a perk that increases damage
        if (AttackerSkills->HasPerk(FName("OneHanded"), FName("OH_DamageExpert")))
        {
            SkillDamageBonus += 0.10f * BaseDamage; // +10% base damage
        }
    }
    // --- Skill/Perk Integration End ---

    Result.FinalDamage = FMath::Max(0.f, BaseDamage + (AttackerStrength * 0.5f) - (VictimDefense * 0.25f) + SkillDamageBonus);
    Result.bDidHit = true;

    if (FMath::FRand() < (AttackerCritChance + SkillCritBonus))
    {
        Result.bIsCritical = true;
        Result.FinalDamage *= (AttackerCritDamage + SkillCritDamageBonus);
    }

    VictimStats->UpdateStat(FName("Health"), -Result.FinalDamage);

    Result.bIsFatal = VictimStats->GetCurrentStatValue(FName("Health")) <= 0.f;

    return Result;
}