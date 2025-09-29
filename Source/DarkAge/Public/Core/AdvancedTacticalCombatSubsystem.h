#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Data/CombatTypes.h"
#include "AdvancedTacticalCombatSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECombatFormation : uint8
{
    None            UMETA(DisplayName = "No Formation"),
    Line            UMETA(DisplayName = "Line Formation"),
    Column          UMETA(DisplayName = "Column Formation"),
    Wedge           UMETA(DisplayName = "Wedge Formation"),
    Circle          UMETA(DisplayName = "Circle Formation"),
    Phalanx         UMETA(DisplayName = "Phalanx Formation"),
    Skirmish        UMETA(DisplayName = "Skirmish Formation"),
    Ambush          UMETA(DisplayName = "Ambush Formation")
};

UENUM(BlueprintType)
enum class ECombatTactic : uint8
{
    Aggressive      UMETA(DisplayName = "Aggressive Assault"),
    Defensive       UMETA(DisplayName = "Defensive Hold"),
    Flanking        UMETA(DisplayName = "Flanking Maneuver"),
    Retreat         UMETA(DisplayName = "Strategic Retreat"),
    Feint           UMETA(DisplayName = "Feint Attack"),
    Charge          UMETA(DisplayName = "Cavalry Charge"),
    Siege           UMETA(DisplayName = "Siege Warfare"),
    Guerrilla       UMETA(DisplayName = "Guerrilla Tactics")
};


UENUM(BlueprintType)
enum class EArmorType : uint8
{
    None            UMETA(DisplayName = "No Armor"),
    Cloth           UMETA(DisplayName = "Cloth Armor"),
    Leather         UMETA(DisplayName = "Leather Armor"),
    Chain           UMETA(DisplayName = "Chain Mail"),
    Scale           UMETA(DisplayName = "Scale Armor"),
    Plate           UMETA(DisplayName = "Plate Armor"),
    Magical         UMETA(DisplayName = "Magical Armor")
};

UENUM(BlueprintType)
enum class ECombatRole : uint8
{
    Frontline       UMETA(DisplayName = "Frontline Fighter"),
    Archer          UMETA(DisplayName = "Archer"),
    Cavalry         UMETA(DisplayName = "Cavalry"),
    Support         UMETA(DisplayName = "Support"),
    Commander       UMETA(DisplayName = "Commander"),
    Scout           UMETA(DisplayName = "Scout"),
    Siege           UMETA(DisplayName = "Siege Operator"),
    Healer          UMETA(DisplayName = "Healer")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FCombatant
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    FString CombatantID;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    FString FactionID;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    ECombatRole Role = ECombatRole::Frontline;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    FVector Position;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Health = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float MaxHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Stamina = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float MaxStamina = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Morale = 75.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Experience = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    EWeaponType PrimaryWeapon = EWeaponType::Sword;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    EWeaponType SecondaryWeapon = EWeaponType::Shield;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    EArmorType ArmorType = EArmorType::Leather;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float AttackPower = 25.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Defense = 15.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Speed = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    float Accuracy = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    bool bIsAlive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    bool bIsRouted = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combatant")
    TArray<FString> StatusEffects;

    FCombatant()
    {
        CombatantID = TEXT("");
        Name = TEXT("");
        FactionID = TEXT("");
        Role = ECombatRole::Frontline;
        Position = FVector::ZeroVector;
        Health = 100.0f;
        MaxHealth = 100.0f;
        Stamina = 100.0f;
        MaxStamina = 100.0f;
        Morale = 75.0f;
        Experience = 0.0f;
        PrimaryWeapon = EWeaponType::Sword;
        SecondaryWeapon = EWeaponType::Shield;
        ArmorType = EArmorType::Leather;
        AttackPower = 25.0f;
        Defense = 15.0f;
        Speed = 10.0f;
        Accuracy = 0.8f;
        bIsAlive = true;
        bIsRouted = false;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FCombatUnit
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    FString UnitID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    FString UnitName;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    FString FactionID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    TArray<FString> Members;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    FString CommanderID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    ECombatFormation Formation = ECombatFormation::Line;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    ECombatTactic CurrentTactic = ECombatTactic::Aggressive;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    FVector CenterPosition;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    float UnitMorale = 75.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    float Cohesion = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    float EffectiveCombatPower = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    bool bIsEngaged = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Unit")
    TArray<FString> EngagedEnemies;

    FCombatUnit()
    {
        UnitID = TEXT("");
        UnitName = TEXT("");
        FactionID = TEXT("");
        CommanderID = TEXT("");
        Formation = ECombatFormation::Line;
        CurrentTactic = ECombatTactic::Aggressive;
        CenterPosition = FVector::ZeroVector;
        UnitMorale = 75.0f;
        Cohesion = 1.0f;
        EffectiveCombatPower = 100.0f;
        bIsEngaged = false;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FCombatEncounter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    FString EncounterID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    FString EncounterName;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    FVector BattlefieldCenter;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    float BattlefieldRadius = 5000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    TArray<FString> ParticipatingFactions;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    TArray<FString> ParticipatingUnits;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    bool bIsResolved = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    FString Victor;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    TMap<FString, int32> Casualties;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    TMap<FString, float> TerrainModifiers;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Encounter")
    TArray<FString> WeatherEffects;

    FCombatEncounter()
    {
        EncounterID = TEXT("");
        EncounterName = TEXT("");
        BattlefieldCenter = FVector::ZeroVector;
        BattlefieldRadius = 5000.0f;
        StartTime = 0.0f;
        Duration = 0.0f;
        bIsActive = false;
        bIsResolved = false;
        Victor = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FCombatAction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    FString ActionID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    FString ActorID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    FString TargetID;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    FString ActionType;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    float Damage = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    float Accuracy = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    float ExecutionTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    bool bWasSuccessful = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat Action")
    TArray<FString> AppliedEffects;

    FCombatAction()
    {
        ActionID = TEXT("");
        ActorID = TEXT("");
        TargetID = TEXT("");
        ActionType = TEXT("");
        Damage = 0.0f;
        Accuracy = 1.0f;
        ExecutionTime = 0.0f;
        bWasSuccessful = false;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FTacticalAdvantage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Tactical Advantage")
    FString AdvantageType;

    UPROPERTY(BlueprintReadOnly, Category = "Tactical Advantage")
    float Magnitude = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Tactical Advantage")
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Tactical Advantage")
    FString Source;

    UPROPERTY(BlueprintReadOnly, Category = "Tactical Advantage")
    TArray<FString> AffectedUnits;

    FTacticalAdvantage()
    {
        AdvantageType = TEXT("");
        Magnitude = 1.0f;
        Duration = 0.0f;
        Source = TEXT("");
    }
};

// Combat System Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatEncounterStarted, const FCombatEncounter&, Encounter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatEncounterEnded, const FCombatEncounter&, Encounter, const FString&, Victor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatantDefeated, const FString&, CombatantID, const FString&, DefeatedBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitRouted, const FString&, UnitID, float, MoraleLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTacticalAdvantageGained, const FString&, UnitID, const FString&, AdvantageType, float, Magnitude);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedTacticalCombatSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedTacticalCombatSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedTacticalCombatSubsystem, STATGROUP_Tickables); }

    // Combat System Delegates
    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnCombatEncounterStarted OnCombatEncounterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnCombatEncounterEnded OnCombatEncounterEnded;

    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnCombatantDefeated OnCombatantDefeated;

    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnUnitRouted OnUnitRouted;

    UPROPERTY(BlueprintAssignable, Category = "Combat Events")
    FOnTacticalAdvantageGained OnTacticalAdvantageGained;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    FString StartCombatEncounter(const TArray<FString>& FactionIDs, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    void EndCombatEncounter(const FString& EncounterID, const FString& Victor);

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    FString CreateCombatUnit(const FString& FactionID, const TArray<FString>& MemberIDs, const FString& CommanderID);

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    void SetUnitFormation(const FString& UnitID, ECombatFormation Formation);

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    void SetUnitTactic(const FString& UnitID, ECombatTactic Tactic);

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    bool ExecuteCombatAction(const FString& ActorID, const FString& TargetID, const FString& ActionType);

    UFUNCTION(BlueprintPure, Category = "Tactical Combat")
    TArray<FCombatEncounter> GetActiveCombatEncounters() const;

    UFUNCTION(BlueprintPure, Category = "Tactical Combat")
    FCombatUnit GetCombatUnit(const FString& UnitID) const;

    UFUNCTION(BlueprintPure, Category = "Tactical Combat")
    TArray<FTacticalAdvantage> GetUnitAdvantages(const FString& UnitID) const;

    UFUNCTION(BlueprintCallable, Category = "Tactical Combat")
    void ApplyTacticalAdvantage(const FString& UnitID, const FString& AdvantageType, float Magnitude, float Duration);

    UFUNCTION(BlueprintPure, Category = "Tactical Combat")
    float CalculateCombatEffectiveness(const FString& UnitID) const;

protected:
    // Combat system data
    UPROPERTY()
    TMap<FString, FCombatant> Combatants;

    UPROPERTY()
    TMap<FString, FCombatUnit> CombatUnits;

    UPROPERTY()
    TArray<FCombatEncounter> ActiveEncounters;

    UPROPERTY()
    TArray<FCombatAction> CombatActionQueue;

    // Note: TMap with TArray values cannot be exposed to Blueprint
    TMap<FString, TArray<FTacticalAdvantage>> UnitAdvantages;

    // Combat configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Configuration")
    float CombatUpdateInterval = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Configuration")
    float BaseHitChance = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Configuration")
    float MoraleBreakThreshold = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Configuration")
    bool bEnableFormationBonuses = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Configuration")
    bool bEnableTerrainEffects = true;

    // Internal state
    float CombatUpdateTimer = 0.0f;
    int32 NextEncounterID = 1;
    int32 NextUnitID = 1;
    int32 NextActionID = 1;

    // Initialization functions
    void InitializeCombatSystem();
    void InitializeFormationBonuses();
    void InitializeTacticEffects();

    // Combat encounter management
    void UpdateActiveCombatEncounters(float DeltaTime);
    void ProcessCombatActions(float DeltaTime);
    void ResolveCombatEncounter(FCombatEncounter& Encounter);
    bool CheckVictoryConditions(const FCombatEncounter& Encounter);

    // Unit management functions
    void UpdateCombatUnits(float DeltaTime);
    void UpdateUnitMorale(FCombatUnit& Unit);
    void UpdateUnitCohesion(FCombatUnit& Unit);
    void CheckUnitRouting(FCombatUnit& Unit);
    void CalculateUnitEffectiveness(FCombatUnit& Unit);

    // Combat action processing
    void ProcessCombatAction(const FCombatAction& Action);
    bool CalculateHitChance(const FCombatant& Attacker, const FCombatant& Defender);
    float CalculateDamage(const FCombatant& Attacker, const FCombatant& Defender);
    void ApplyCombatDamage(FCombatant& Target, float Damage);

    // Formation system
    void ApplyFormationBonuses(FCombatUnit& Unit);
    float GetFormationBonus(ECombatFormation Formation, const FString& BonusType);
    void UpdateUnitPositions(FCombatUnit& Unit);
    bool ValidateFormation(const FCombatUnit& Unit);

    // Tactical system
    void ApplyTacticEffects(FCombatUnit& Unit);
    float GetTacticModifier(ECombatTactic Tactic, const FString& ModifierType);
    void ProcessTacticalManeuvers(FCombatUnit& Unit);
    void CalculateTacticalAdvantages(FCombatUnit& Unit);

    // Environmental effects
    void ApplyTerrainEffects(FCombatEncounter& Encounter);
    void ApplyWeatherEffects(FCombatEncounter& Encounter);
    float GetTerrainModifier(const FString& TerrainType, const FString& ModifierType);
    float GetWeatherModifier(const FString& WeatherType, const FString& ModifierType);

    // Morale and psychology system
    void ProcessMoraleEffects(float DeltaTime);
    void ApplyMoraleModifiers(FCombatant& Combatant, const TArray<FString>& Modifiers);
    float CalculateMoraleImpact(const FCombatAction& Action);
    void PropagateRouting(const FCombatUnit& RoutedUnit);

    // AI combat decision making
    void ProcessAICombatDecisions();
    ECombatTactic DetermineOptimalTactic(const FCombatUnit& Unit, const FCombatEncounter& Encounter);
    ECombatFormation DetermineOptimalFormation(const FCombatUnit& Unit, const FCombatEncounter& Encounter);
    FString SelectCombatTarget(const FCombatant& Attacker, const FCombatEncounter& Encounter);

    // Combat analytics and learning
    void RecordCombatData(const FCombatAction& Action);
    void AnalyzeCombatEffectiveness();
    void UpdateCombatAI();
    void ProcessCombatExperience();

    // Utility functions
    FString GenerateUniqueEncounterID();
    FString GenerateUniqueUnitID();
    FString GenerateUniqueActionID();
    float CalculateDistance(const FVector& PositionA, const FVector& PositionB);
    bool IsInCombatRange(const FCombatant& Attacker, const FCombatant& Target);
    TArray<FString> GetNearbyEnemies(const FCombatant& Combatant, float Range);
    void UpdateCombatStatistics();
};