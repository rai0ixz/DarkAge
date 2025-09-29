#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MagicSystem.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EMagicSchool : uint8
{
    Elemental     UMETA(DisplayName = "Elemental"),
    Necromancy    UMETA(DisplayName = "Necromancy"),
    Illusion      UMETA(DisplayName = "Illusion"),
    Enchantment   UMETA(DisplayName = "Enchantment"),
    Divination    UMETA(DisplayName = "Divination"),
    Conjuration   UMETA(DisplayName = "Conjuration"),
    Transmutation UMETA(DisplayName = "Transmutation")
};

UENUM(BlueprintType)
enum class ESpellType : uint8
{
    Offensive     UMETA(DisplayName = "Offensive"),
    Defensive     UMETA(DisplayName = "Defensive"),
    Utility       UMETA(DisplayName = "Utility"),
    Healing       UMETA(DisplayName = "Healing"),
    Summoning     UMETA(DisplayName = "Summoning"),
    Curse         UMETA(DisplayName = "Curse")
};

USTRUCT(BlueprintType)
struct FMagicSpell
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    FString SpellName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    EMagicSchool School;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    ESpellType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    int32 ManaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    float CastTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    TMap<FString, float> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    bool bUnlocked;

    FMagicSpell()
    {
        ManaCost = 10;
        CastTime = 1.0f;
        Cooldown = 5.0f;
        RequiredLevel = 1;
        bUnlocked = false;
    }
};

USTRUCT(BlueprintType)
struct FMagicCurse
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    FString CurseName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    EMagicSchool School;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    TMap<FString, float> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curse")
    bool bRemovable;

    FMagicCurse()
    {
        Duration = 60.0f;
        bRemovable = true;
    }
};
 
USTRUCT()
struct FActiveCurses
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FMagicCurse> Curses;
};
 
USTRUCT(BlueprintType)
struct FSupernaturalEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FString EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    EMagicSchool PrimarySchool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    TArray<FString> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
    bool bActive;

    FSupernaturalEvent()
    {
        Intensity = 1.0f;
        bActive = false;
    }
};

UCLASS()
class DARKAGE_API UMagicSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMagicSystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Spell management
    UFUNCTION(BlueprintCallable, Category = "Magic")
    bool CastSpell(const FString& SpellName, AActor* Caster, AActor* Target = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Magic")
    bool LearnSpell(const FString& SpellName);

    UFUNCTION(BlueprintPure, Category = "Magic")
    bool HasSpell(const FString& SpellName) const;

    UFUNCTION(BlueprintPure, Category = "Magic")
    const TArray<FMagicSpell>& GetKnownSpells() const { return KnownSpells; }

    // Curse system
    UFUNCTION(BlueprintCallable, Category = "Magic")
    bool ApplyCurse(const FString& CurseName, AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Magic")
    bool RemoveCurse(const FString& CurseName, AActor* Target);

    UFUNCTION(BlueprintPure, Category = "Magic")
    bool HasCurse(AActor* Target, const FString& CurseName) const;

    // Mana system
    UFUNCTION(BlueprintCallable, Category = "Magic")
    void ModifyMana(float Amount);

    UFUNCTION(BlueprintPure, Category = "Magic")
    float GetCurrentMana() const { return CurrentMana; }

    UFUNCTION(BlueprintPure, Category = "Magic")
    float GetMaxMana() const { return MaxMana; }

    // Supernatural events
    UFUNCTION(BlueprintCallable, Category = "Magic")
    void TriggerSupernaturalEvent(const FString& EventType, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Magic")
    void UpdateSupernaturalEvents(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Magic")
    const TArray<FSupernaturalEvent>& GetActiveEvents() const { return ActiveEvents; }

    // Magic progression
    UFUNCTION(BlueprintCallable, Category = "Magic")
    void GainMagicExperience(float XP);

    UFUNCTION(BlueprintPure, Category = "Magic")
    int32 GetMagicLevel() const { return MagicLevel; }

    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "Magic")
    void InitializeSpellbook();

    UFUNCTION(BlueprintCallable, Category = "Magic")
    void InitializeCurseLibrary();

private:
    // Spell system
    UPROPERTY()
    TArray<FMagicSpell> KnownSpells;

    UPROPERTY()
    TArray<FMagicSpell> AvailableSpells;

    // Curse system
    UPROPERTY()
    TArray<FMagicCurse> CurseLibrary;

    UPROPERTY()
    TMap<AActor*, FActiveCurses> ActiveCurses;

    // Mana system
    UPROPERTY()
    float CurrentMana;

    UPROPERTY()
    float MaxMana;

    UPROPERTY()
    float ManaRegenRate;

    // Supernatural events
    UPROPERTY()
    TArray<FSupernaturalEvent> ActiveEvents;

    // Magic progression
    UPROPERTY()
    int32 MagicLevel;

    UPROPERTY()
    float MagicExperience;

    // Core functions
    void ApplySpellEffects(const FMagicSpell& Spell, AActor* Caster, AActor* Target);
    void ApplyCurseEffects(const FMagicCurse& Curse, AActor* Target);
    void UpdateMana(float DeltaTime);
    void UpdateCurses(float DeltaTime);
    FMagicSpell* FindSpell(const FString& SpellName);
    FMagicCurse* FindCurse(const FString& CurseName);
    bool CanCastSpell(const FMagicSpell& Spell) const;
    void CreateSupernaturalEvent(const FString& EventType, const FVector& Location);
    void ProcessSupernaturalEffects(float DeltaTime);
};