#include "Core/MagicSystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Components/StatlineComponent.h"

UMagicSystem::UMagicSystem()
{
    CurrentMana = 100.0f;
    MaxMana = 100.0f;
    ManaRegenRate = 5.0f; // 5 mana per second
    MagicLevel = 1;
    MagicExperience = 0.0f;
}

void UMagicSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeSpellbook();
    InitializeCurseLibrary();
}

void UMagicSystem::Deinitialize()
{
    Super::Deinitialize();
    ActiveEvents.Empty();
    ActiveCurses.Empty();
}

bool UMagicSystem::CastSpell(const FString& SpellName, AActor* Caster, AActor* Target)
{
    FMagicSpell* Spell = FindSpell(SpellName);
    if (!Spell || !Spell->bUnlocked || !CanCastSpell(*Spell))
    {
        return false;
    }

    // Consume mana
    CurrentMana -= Spell->ManaCost;

    // Apply spell effects
    ApplySpellEffects(*Spell, Caster, Target);

    // Gain experience
    GainMagicExperience(Spell->ManaCost * 0.1f);

    UE_LOG(LogTemp, Log, TEXT("Cast spell: %s (Cost: %d mana)"), *SpellName, Spell->ManaCost);
    return true;
}

bool UMagicSystem::LearnSpell(const FString& SpellName)
{
    FMagicSpell* Spell = FindSpell(SpellName);
    if (!Spell || Spell->bUnlocked || MagicLevel < Spell->RequiredLevel)
    {
        return false;
    }

    Spell->bUnlocked = true;
    KnownSpells.Add(*Spell);

    UE_LOG(LogTemp, Log, TEXT("Learned spell: %s"), *SpellName);
    return true;
}

bool UMagicSystem::HasSpell(const FString& SpellName) const
{
    FMagicSpell* Spell = nullptr;
    for (const FMagicSpell& KnownSpell : KnownSpells)
    {
        if (KnownSpell.SpellName == SpellName)
        {
            Spell = const_cast<FMagicSpell*>(&KnownSpell);
            break;
        }
    }
    return Spell && Spell->bUnlocked;
}

bool UMagicSystem::ApplyCurse(const FString& CurseName, AActor* Target)
{
    FMagicCurse* Curse = FindCurse(CurseName);
    if (!Curse || !Target)
    {
        return false;
    }

        // Add curse to target
        if (!ActiveCurses.Contains(Target))
        {
            ActiveCurses.Add(Target, FActiveCurses());
        }
        ActiveCurses[Target].Curses.Add(*Curse);

    // Apply curse effects
    ApplyCurseEffects(*Curse, Target);

    UE_LOG(LogTemp, Log, TEXT("Applied curse: %s to %s"), *CurseName, *Target->GetName());
    return true;
}

bool UMagicSystem::RemoveCurse(const FString& CurseName, AActor* Target)
{
    if (!ActiveCurses.Contains(Target))
    {
        return false;
    }

    FActiveCurses& Active = ActiveCurses[Target];
    TArray<FMagicCurse>& Curses = Active.Curses;
    for (int32 i = 0; i < Curses.Num(); ++i)
    {
        if (Curses[i].CurseName == CurseName)
        {
            Curses.RemoveAt(i);
            UE_LOG(LogTemp, Log, TEXT("Removed curse: %s from %s"), *CurseName, *Target->GetName());
            return true;
        }
    }

    return false;
}

bool UMagicSystem::HasCurse(AActor* Target, const FString& CurseName) const
{
    if (!ActiveCurses.Contains(Target))
    {
        return false;
    }

    const FActiveCurses& Active = ActiveCurses[Target];
    const TArray<FMagicCurse>& Curses = Active.Curses;
    for (const FMagicCurse& Curse : Curses)
    {
        if (Curse.CurseName == CurseName)
        {
            return true;
        }
    }

    return false;
}

void UMagicSystem::ModifyMana(float Amount)
{
    CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);
}

void UMagicSystem::TriggerSupernaturalEvent(const FString& EventType, const FVector& Location)
{
    CreateSupernaturalEvent(EventType, Location);
}

void UMagicSystem::UpdateSupernaturalEvents(float DeltaTime)
{
    ProcessSupernaturalEffects(DeltaTime);
    UpdateMana(DeltaTime);
    UpdateCurses(DeltaTime);
}

void UMagicSystem::GainMagicExperience(float XP)
{
    MagicExperience += XP;

    // Level up check
    int32 XPForNextLevel = MagicLevel * 200; // Magic leveling is harder
    if (MagicExperience >= XPForNextLevel)
    {
        MagicLevel++;
        MagicExperience -= XPForNextLevel;
        MaxMana += 25.0f; // Increase max mana
        CurrentMana = MaxMana; // Full restore on level up

        UE_LOG(LogTemp, Log, TEXT("Magic level increased to %d! Max mana: %.1f"), MagicLevel, MaxMana);
    }
}

void UMagicSystem::InitializeSpellbook()
{
    // Fire spells
    FMagicSpell Fireball;
    Fireball.SpellName = "Fireball";
    Fireball.Description = "Launch a ball of fire at enemies";
    Fireball.School = EMagicSchool::Elemental;
    Fireball.Type = ESpellType::Offensive;
    Fireball.ManaCost = 15;
    Fireball.RequiredLevel = 1;
    Fireball.Effects.Add("Damage", 25.0f);
    Fireball.Effects.Add("BurnChance", 0.3f);
    AvailableSpells.Add(Fireball);

    // Healing spells
    FMagicSpell Heal;
    Heal.SpellName = "Heal";
    Heal.Description = "Restore health to target";
    Heal.School = EMagicSchool::Transmutation;
    Heal.Type = ESpellType::Healing;
    Heal.ManaCost = 20;
    Heal.RequiredLevel = 2;
    Heal.Effects.Add("HealAmount", 30.0f);
    AvailableSpells.Add(Heal);

    // Illusion spells
    FMagicSpell Invisibility;
    Invisibility.SpellName = "Invisibility";
    Invisibility.Description = "Become invisible to enemies";
    Invisibility.School = EMagicSchool::Illusion;
    Invisibility.Type = ESpellType::Utility;
    Invisibility.ManaCost = 25;
    Invisibility.RequiredLevel = 3;
    Invisibility.Effects.Add("Duration", 30.0f);
    AvailableSpells.Add(Invisibility);

    // Summoning spells
    FMagicSpell SummonFamiliar;
    SummonFamiliar.SpellName = "Summon Familiar";
    SummonFamiliar.Description = "Summon a magical creature to aid you";
    SummonFamiliar.School = EMagicSchool::Conjuration;
    SummonFamiliar.Type = ESpellType::Summoning;
    SummonFamiliar.ManaCost = 30;
    SummonFamiliar.RequiredLevel = 4;
    SummonFamiliar.Effects.Add("Duration", 120.0f);
    AvailableSpells.Add(SummonFamiliar);
}

void UMagicSystem::InitializeCurseLibrary()
{
    FMagicCurse Weakness;
    Weakness.CurseName = "Weakness";
    Weakness.Description = "Reduces physical strength";
    Weakness.School = EMagicSchool::Necromancy;
    Weakness.Duration = 60.0f;
    Weakness.Effects.Add("StrengthPenalty", -5.0f);
    CurseLibrary.Add(Weakness);

    FMagicCurse Confusion;
    Confusion.CurseName = "Confusion";
    Confusion.Description = "Causes disorientation and poor decision making";
    Confusion.School = EMagicSchool::Enchantment;
    Confusion.Duration = 45.0f;
    Confusion.Effects.Add("AccuracyPenalty", -0.5f);
    CurseLibrary.Add(Confusion);

    FMagicCurse BadLuck;
    BadLuck.CurseName = "Bad Luck";
    BadLuck.Description = "Increases chance of negative events";
    BadLuck.School = EMagicSchool::Divination;
    BadLuck.Duration = 300.0f; // 5 minutes
    BadLuck.Effects.Add("LuckPenalty", -0.3f);
    CurseLibrary.Add(BadLuck);
}

void UMagicSystem::ApplySpellEffects(const FMagicSpell& Spell, AActor* Caster, AActor* Target)
{
    switch (Spell.Type)
    {
    case ESpellType::Offensive:
        if (Target)
        {
            // Apply damage
            if (Spell.Effects.Contains("Damage"))
            {
                float Damage = Spell.Effects["Damage"];
                // Apply damage to target (would integrate with combat system)
                UE_LOG(LogTemp, Log, TEXT("Spell damage: %.1f to %s"), Damage, *Target->GetName());
            }
        }
        break;

    case ESpellType::Healing:
        if (Target)
        {
            // Apply healing
            if (Spell.Effects.Contains("HealAmount"))
            {
                float HealAmount = Spell.Effects["HealAmount"];
                // Apply healing to target (would integrate with health system)
                UE_LOG(LogTemp, Log, TEXT("Spell healing: %.1f to %s"), HealAmount, *Target->GetName());
            }
        }
        break;

    case ESpellType::Utility:
        // Apply utility effects (invisibility, buffs, etc.)
        if (Spell.SpellName == "Invisibility")
        {
            // Make caster invisible
            UE_LOG(LogTemp, Log, TEXT("Applied invisibility to %s"), *Caster->GetName());
        }
        break;

    case ESpellType::Summoning:
        // Summon creatures
        UE_LOG(LogTemp, Log, TEXT("Summoned creature for %s"), *Caster->GetName());
        break;
    }
}

void UMagicSystem::ApplyCurseEffects(const FMagicCurse& Curse, AActor* Target)
{
    // Apply curse effects to target
    for (const auto& Effect : Curse.Effects)
    {
        UE_LOG(LogTemp, Log, TEXT("Applied curse effect %s: %.1f to %s"),
            *Effect.Key, Effect.Value, *Target->GetName());
    }
}

void UMagicSystem::UpdateMana(float DeltaTime)
{
    if (CurrentMana < MaxMana)
    {
        CurrentMana = FMath::Min(CurrentMana + ManaRegenRate * DeltaTime, MaxMana);
    }
}

void UMagicSystem::UpdateCurses(float DeltaTime)
{
    // Update curse durations and remove expired curses
    TArray<AActor*> ActorsToRemove;

    for (auto& CursePair : ActiveCurses)
    {
        AActor* Target = CursePair.Key;
        FActiveCurses& Active = CursePair.Value;
        TArray<FMagicCurse>& Curses = Active.Curses;

        for (int32 i = Curses.Num() - 1; i >= 0; --i)
        {
            FMagicCurse& Curse = Curses[i];
            Curse.Duration -= DeltaTime;

            if (Curse.Duration <= 0.0f)
            {
                // Curse expired
                UE_LOG(LogTemp, Log, TEXT("Curse %s expired on %s"), *Curse.CurseName, *Target->GetName());
                Curses.RemoveAt(i);
            }
        }

        if (Curses.Num() == 0)
        {
            ActorsToRemove.Add(Target);
        }
    }

    // Remove actors with no active curses
    for (AActor* Actor : ActorsToRemove)
    {
        ActiveCurses.Remove(Actor);
    }
}

FMagicSpell* UMagicSystem::FindSpell(const FString& SpellName)
{
    for (FMagicSpell& Spell : AvailableSpells)
    {
        if (Spell.SpellName == SpellName)
        {
            return &Spell;
        }
    }
    return nullptr;
}

FMagicCurse* UMagicSystem::FindCurse(const FString& CurseName)
{
    for (FMagicCurse& Curse : CurseLibrary)
    {
        if (Curse.CurseName == CurseName)
        {
            return &Curse;
        }
    }
    return nullptr;
}

bool UMagicSystem::CanCastSpell(const FMagicSpell& Spell) const
{
    return CurrentMana >= Spell.ManaCost && Spell.bUnlocked;
}

void UMagicSystem::CreateSupernaturalEvent(const FString& EventType, const FVector& Location)
{
    FSupernaturalEvent NewEvent;
    NewEvent.EventName = EventType;
    NewEvent.Location = Location;
    NewEvent.bActive = true;
    NewEvent.Intensity = FMath::RandRange(0.5f, 2.0f);

    if (EventType == "BloodMoon")
    {
        NewEvent.Description = "A blood moon rises, empowering dark magic";
        NewEvent.PrimarySchool = EMagicSchool::Necromancy;
        NewEvent.Effects.Add("NecromancyPower");
        NewEvent.Effects.Add("UndeadSpawn");
    }
    else if (EventType == "MeteorShower")
    {
        NewEvent.Description = "Meteors rain from the sky, carrying magical energy";
        NewEvent.PrimarySchool = EMagicSchool::Elemental;
        NewEvent.Effects.Add("ElementalPower");
        NewEvent.Effects.Add("RandomSpells");
    }
    else if (EventType == "SpiritSurge")
    {
        NewEvent.Description = "Spirits walk among the living";
        NewEvent.PrimarySchool = EMagicSchool::Divination;
        NewEvent.Effects.Add("SpiritSight");
        NewEvent.Effects.Add("GhostEncounters");
    }

    ActiveEvents.Add(NewEvent);
    UE_LOG(LogTemp, Log, TEXT("Supernatural event triggered: %s at %s"), *EventType, *Location.ToString());
}

void UMagicSystem::ProcessSupernaturalEffects(float DeltaTime)
{
    for (FSupernaturalEvent& Event : ActiveEvents)
    {
        if (!Event.bActive) continue;

        // Process event effects based on type
        for (const FString& Effect : Event.Effects)
        {
            if (Effect == "NecromancyPower")
            {
                // Boost necromancy spells in area
            }
            else if (Effect == "ElementalPower")
            {
                // Boost elemental spells in area
            }
            else if (Effect == "SpiritSight")
            {
                // Allow seeing invisible spirits
            }
        }

        // Event duration (simplified - would have proper timing)
        Event.Intensity -= DeltaTime * 0.01f;
        if (Event.Intensity <= 0.0f)
        {
            Event.bActive = false;
            UE_LOG(LogTemp, Log, TEXT("Supernatural event ended: %s"), *Event.EventName);
        }
    }

    // Remove inactive events
    ActiveEvents.RemoveAll([](const FSupernaturalEvent& Event) {
        return !Event.bActive;
    });
}