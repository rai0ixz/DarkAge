#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/DAStatusEffectBehavior.h"
#include "StatusEffectData.generated.h"

/**
 * Status Effect Types for categorization and behavior
 */
UENUM(BlueprintType)
enum class EStatusEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    Survival        UMETA(DisplayName = "Survival"),       // Hunger, thirst, exhaustion
    Disease         UMETA(DisplayName = "Disease"),        // Illnesses, infections
    Environmental   UMETA(DisplayName = "Environmental"),  // Cold, heat, exposure
    Buff            UMETA(DisplayName = "Buff"),           // Positive temporary effects
    Debuff          UMETA(DisplayName = "Debuff"),         // Negative temporary effects
    Magical         UMETA(DisplayName = "Magical"),        // Curses, enchantments
    Poison          UMETA(DisplayName = "Poison"),         // Toxins, venom
    Injury          UMETA(DisplayName = "Injury")          // Physical wounds, trauma
};

/**
 * Status Effect Stack Behavior
 */
UENUM(BlueprintType)
enum class EStatusEffectStackBehavior : uint8
{
    None            UMETA(DisplayName = "None"),           // Cannot stack
    Stack           UMETA(DisplayName = "Stack"),          // Multiple instances stack
    RefreshDuration UMETA(DisplayName = "Refresh Duration"), // Refresh duration only
    Intensify       UMETA(DisplayName = "Intensify"),      // Increase intensity
    Replace         UMETA(DisplayName = "Replace")         // Replace existing effect
};

/**
 * Status Effect Application Method
 */
UENUM(BlueprintType)
enum class EStatusEffectApplication : uint8
{
    Instant         UMETA(DisplayName = "Instant"),        // Applied once
    OverTime        UMETA(DisplayName = "Over Time"),      // Applied each tick
    OnTrigger       UMETA(DisplayName = "On Trigger"),     // Applied on specific events
    Persistent      UMETA(DisplayName = "Persistent")      // Always active while effect exists
};

/**
 * Stat Modifier Data for Status Effects
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FStatusEffectStatModifier
{
    GENERATED_BODY()

    FStatusEffectStatModifier()
        : StatName()
        , ModifierValue(0.0f)
        , bIsPercentage(false)
        , bIsAdditive(true)
    {
    }

    // Name of the stat to modify (e.g., "Health", "Stamina", "MovementSpeed")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    FName StatName;

    // Value to apply to the stat
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    float ModifierValue;

    // Whether the modifier is a percentage (0.1 = 10%) or absolute value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    bool bIsPercentage;

    // Whether the modifier is additive (+/-) or multiplicative (*/)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    bool bIsAdditive;
};

/**
 * Core Status Effect Data
 * Defines a status effect that can be applied to characters
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FStatusEffectData : public FTableRowBase
{
    GENERATED_BODY()

    FORCEINLINE FStatusEffectData()
        : EffectType(EStatusEffectType::None)
        , StackBehavior(EStatusEffectStackBehavior::None)
        , ApplicationMethod(EStatusEffectApplication::Instant)
        , MaxStacks(1)
        , BaseDuration(0.0f)
        , TickInterval(1.0f)
        , Priority(0)
        , bCanBeCleansed(true)
        , bPersistThroughDeath(false)
        , bSaveWithGame(true)
        , IconTexture(nullptr)
        , BehaviorClass(nullptr)
        , UpgradeEffectID(NAME_None)
        , DowngradeEffectID(NAME_None)
        , AoeRadius(0.0f)
        , VisualEffect(nullptr)
        , AttachSocketName(NAME_None)
        , ApplySound(nullptr)
        , ExpireSound(nullptr)
    {
    }

    // Unique identifier for this status effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FName EffectID;

    // Display name for UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FText DisplayName;

    // Description for tooltip/UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FText Description;

    // Type of status effect for categorization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    EStatusEffectType EffectType;

    // How this effect behaves when applied multiple times
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stacking")
    EStatusEffectStackBehavior StackBehavior;

    // How the effect is applied (instant, over time, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Application")
    EStatusEffectApplication ApplicationMethod;

    // Maximum number of stacks allowed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stacking", meta = (ClampMin = "1"))
    int32 MaxStacks;

    // Base duration in seconds (0 = permanent until removed)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (ClampMin = "0.0"))
    float BaseDuration;

    // How often the effect ticks for over-time effects (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (ClampMin = "0.1"))
    float TickInterval;

    // Priority for application order (higher = applied first)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Application")
    int32 Priority;

    // Can this effect be removed by cleansing/curing abilities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Removal")
    bool bCanBeCleansed;

    // Does this effect persist through character death
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence")
    bool bPersistThroughDeath;

    // Should this effect be saved with game save data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence")
    bool bSaveWithGame;

    // Stat modifiers applied by this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TArray<FStatusEffectStatModifier> StatModifiers;

    // Icon for UI display
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UTexture2D* IconTexture;

    // TODO: Add support for custom effect behaviors via blueprintable classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    TSubclassOf<class UDAStatusEffectBehavior> BehaviorClass;

    // TODO: Implement effect prerequisites and mutual exclusions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    TArray<FName> PrerequisiteEffectIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    TArray<FName> MutualyExclusiveEffectIDs;

    // TODO: Add support for effect upgrade/downgrade chains
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    FName UpgradeEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    FName DowngradeEffectID;

    // TODO: Implement area-of-effect status applications
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    float AoeRadius;

    // TODO: Add resistance/vulnerability modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    TMap<EStatusEffectType, float> ResistanceModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
    TMap<EStatusEffectType, float> VulnerabilityModifiers;

    // TODO: Create visual effect integration points
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    class UParticleSystem* VisualEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    FName AttachSocketName;

    // TODO: Add sound effect integration for status changes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundBase* ApplySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundBase* ExpireSound;
};

/**
 * Active Status Effect Instance
 * Represents an active status effect on a character
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FActiveStatusEffect
{
    GENERATED_BODY()

    FActiveStatusEffect()
        // : EffectData(nullptr) // MODIFIED: Initialization changes with FDataTableRowHandle
        : EffectID()
        , CurrentStacks(1)
        , RemainingDuration(0.0f)
        , LastTickTime(0.0f)
        , ApplicationTime(0.0f)
        , SourceActorID()
        , bIsActive(true)
    {
        // EffectDataRowHandle will be default constructed
    }

    // MODIFIED: Reference to the status effect data (now using FDataTableRowHandle)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    FDataTableRowHandle EffectDataRowHandle;

    // ID of the effect for quick lookup
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    FName EffectID; // You might want to get this from the RowHandle's RowName after it's set

    // Current number of stacks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    int32 CurrentStacks;

    // Time remaining for this effect (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    float RemainingDuration;

    // Last time this effect ticked (for over-time effects)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    float LastTickTime;

    // When this effect was first applied (world time)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    float ApplicationTime;

    // ID of the actor that applied this effect (for tracking/revenge systems)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    FString SourceActorID;

    // Whether this effect is currently active
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Instance")
    bool bIsActive;
};

/**
 * Status Effect Immunity Data
 * Defines immunities to specific status effects or types
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FStatusEffectImmunity
{
    GENERATED_BODY()

    FStatusEffectImmunity()
        : ImmunityType(EStatusEffectType::None)
        , SpecificEffectIDs()
        , bIsTemporary(false)
        , Duration(0.0f)
    {
    }

    // Type of effects to be immune to (None = use specific IDs only)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    EStatusEffectType ImmunityType;

    // Specific effect IDs to be immune to
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    TArray<FName> SpecificEffectIDs;

    // Whether this immunity is temporary
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    bool bIsTemporary;

    // Duration of temporary immunity (ignored if not temporary)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity", meta = (ClampMin = "0.0"))
    float Duration;
};

/**
 * Delegate for status effect events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusEffectChanged, FName, EffectID, bool, bWasApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatusEffectTick, FName, EffectID, int32, CurrentStacks, float, RemainingDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectExpired, FName, EffectID);

// TODO: Add support for custom effect behaviors via blueprintable classes
// TODO: Implement effect prerequisites and mutual exclusions
// TODO: Add support for effect upgrade/downgrade chains
// TODO: Implement area-of-effect status applications
// TODO: Add resistance/vulnerability modifiers
// TODO: Create visual effect integration points
// TODO: Add sound effect integration for status changes
// TODO: Implement status effect combinations/synergies
USTRUCT(BlueprintType)
struct DARKAGE_API FStatusEffectCombination
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combination")
    TArray<FName> SourceEffectIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combination")
    FName ResultingEffectID;
};