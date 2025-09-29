# StatusEffectComponent

**Class:** `UStatusEffectComponent`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Components/StatusEffectComponent.h`  
**Implementation:** `Source/DarkAge/Private/Components/StatusEffectComponent.cpp`

## Overview

The `UStatusEffectComponent` manages temporary and persistent status effects on actors, including diseases, buffs, debuffs, and environmental conditions. It provides a comprehensive system for applying, tracking, and removing effects with complex interaction rules.

## Key Features

- **Data-Driven Effects**: All effects defined in Data Tables for easy balancing
- **Stacking System**: Configurable stacking behavior (duration refresh, intensity stacking, etc.)
- **Effect Interactions**: Effects can modify, cancel, or enhance other effects
- **Tick-Based Processing**: Periodic effect application with variable intervals
- **Network Replication**: Full multiplayer support with client prediction
- **Visual Integration**: Automatic UI updates and visual effect spawning

## Core Functionality

### Effect Application
```cpp
// Apply a status effect from Data Table
UFUNCTION(BlueprintCallable, Category = "Status Effects")
void ApplyStatusEffect(const FDataTableRowHandle& EffectDataRowHandle, UObject* Instigator);

// Apply effect with custom parameters
UFUNCTION(BlueprintCallable, Category = "Status Effects")
void ApplyCustomStatusEffect(const FStatusEffectData& EffectData, UObject* Instigator);

// Remove specific effect by ID
UFUNCTION(BlueprintCallable, Category = "Status Effects")
void RemoveStatusEffect(FGuid EffectID);

// Remove all effects of a specific type
UFUNCTION(BlueprintCallable, Category = "Status Effects")
void RemoveStatusEffectsByType(FName EffectName);
```

### Effect Queries
```cpp
// Check if actor has specific effect
UFUNCTION(BlueprintPure, Category = "Status Effects")
bool HasStatusEffect(FName EffectName) const;

// Get effect data for active effect
UFUNCTION(BlueprintPure, Category = "Status Effects")
FActiveStatusEffect GetStatusEffect(FName EffectName) const;

// Get all active effects
UFUNCTION(BlueprintPure, Category = "Status Effects")
TArray<FActiveStatusEffect> GetAllActiveEffects() const;

// Get effect stack count
UFUNCTION(BlueprintPure, Category = "Status Effects")
int32 GetEffectStackCount(FName EffectName) const;
```

## Data Structures

### FStatusEffectData
```cpp
USTRUCT(BlueprintType)
struct FStatusEffectData : public FTableRowBase
{
    GENERATED_BODY()

    // Basic effect information
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    // Duration settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPermanent = false;

    // Stacking behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanStack = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStackingBehavior StackingBehavior = EStackingBehavior::RefreshDuration;

    // Effect application
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TickInterval = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> StatModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> EffectTags;

    // Visual effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> VisualEffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* ApplySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* RemoveSound;
};
```

### FActiveStatusEffect
```cpp
USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
    GENERATED_BODY()

    // Effect identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid EffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDataTableRowHandle EffectData;

    // Current status
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RemainingDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UObject* Instigator;

    // Timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LastTickTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeApplied;

    // Visual tracking
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* SpawnedVisualEffect;
};
```

## Stacking Behaviors

### EStackingBehavior
```cpp
UENUM(BlueprintType)
enum class EStackingBehavior : uint8
{
    RefreshDuration,        // Reset duration to full on new application
    AddDuration,           // Add new duration to remaining time
    StackIntensity,        // Increase effect strength with each stack
    ReplaceIfStronger,     // Only apply if new effect is more powerful
    PreventStacking        // Block new applications if effect exists
};
```

## Effect Categories

### Common Status Effects
- **Diseases**: The Shakes, Fever, Plague (progressive stages)
- **Environmental**: Hypothermia, Heatstroke, Wet, Poisoned
- **Combat**: Bleeding, Stunned, Weakened, Berserking
- **Magical**: Blessed, Cursed, Charmed, Invisible
- **Social**: Inspired, Intimidated, Drunk, Well-Fed

### Effect Interactions
```cpp
// Define which effects cancel each other
USTRUCT(BlueprintType)
struct FEffectInteraction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EffectA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EffectB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInteractionType InteractionType;
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    Cancel,             // Effects cancel each other
    Enhance,            // Effects enhance each other
    Modify,             // One effect modifies the other
    Prevent             // One effect prevents the other
};
```

## Integration Points

### StatlineComponent
```cpp
// Apply stat modifications from active effects
void ApplyEffectModifiers(const FStatusEffectData* EffectData);

// Remove stat modifications when effect ends
void RemoveEffectModifiers(const FStatusEffectData* EffectData);

// Calculate total modifier from all stacked effects
float CalculateTotalModifier(FName StatName) const;
```

### Global Event Bus
- Listens for: `StatusEffectApplied`, `StatusEffectRemoved`, `EffectInteraction`
- Broadcasts: `StatusEffectExpired`, `EffectStackChanged`, `EffectConflict`

### Disease System Integration
```cpp
// Special handling for disease progression
void ProcessDiseaseStage(FActiveStatusEffect& DiseaseEffect);

// Check for disease transmission
void CheckDiseaseTransmission(const FActiveStatusEffect& DiseaseEffect);

// Apply disease immunity after recovery
void ApplyDiseaseImmunity(FName DiseaseName, float ImmunityDuration);
```

## Visual Effects

### UI Integration
```cpp
// Update status effect UI when effects change
UFUNCTION(BlueprintImplementableEvent, Category = "Status Effects")
void OnStatusEffectUIUpdate(const TArray<FActiveStatusEffect>& ActiveEffects);

// Flash effect when new status applied
UFUNCTION(BlueprintImplementableEvent, Category = "Status Effects")
void OnStatusEffectApplied(const FActiveStatusEffect& NewEffect);
```

### World Effects
```cpp
// Spawn/despawn visual effects for status
void UpdateVisualEffects();

// Apply post-process effects for certain statuses
void UpdatePostProcessEffects();

// Modify character appearance (e.g., pale when diseased)
void UpdateCharacterAppearance();
```

## Performance Optimization

### Tick Management
```cpp
// Optimize effect processing frequency
UPROPERTY(EditAnywhere, Category = "Performance")
float EffectUpdateInterval = 0.1f;

// Batch effect applications
UPROPERTY(EditAnywhere, Category = "Performance")
bool bBatchEffectUpdates = true;

// Limit concurrent visual effects
UPROPERTY(EditAnywhere, Category = "Performance")
int32 MaxConcurrentVisualEffects = 5;
```

## Network Replication

```cpp
// Replicate active effects to clients
UPROPERTY(Replicated)
TArray<FActiveStatusEffect> ReplicatedActiveEffects;

// Server RPC for applying effects
UFUNCTION(Server, Reliable)
void ServerApplyStatusEffect(const FDataTableRowHandle& EffectData, UObject* Instigator);

// RepNotify for UI updates
UFUNCTION()
void OnRep_ActiveEffects();
```

## Usage Examples

### Applying Disease
```cpp
// Apply "The Shakes" disease from contaminated water
FDataTableRowHandle EffectHandle;
EffectHandle.DataTable = StatusEffectDataTable;
EffectHandle.RowName = FName("TheShakes");

if (UStatusEffectComponent* StatusComp = Actor->FindComponentByClass<UStatusEffectComponent>())
{
    StatusComp->ApplyStatusEffect(EffectHandle, ContaminatedWaterSource);
}
```

### Checking Effect Status
```cpp
// Check if player is diseased before allowing certain actions
if (StatusComp->HasStatusEffect(TEXT("Diseased")))
{
    ShowMessage(TEXT("You feel too ill to perform this action."));
    return false;
}
```

### Effect Interaction Handling
```cpp
// Automatically cure disease with medicine
if (StatusComp->HasStatusEffect(TEXT("TheShakes")))
{
    StatusComp->RemoveStatusEffectsByType(TEXT("TheShakes"));
    StatusComp->ApplyStatusEffect(MedicineEffectHandle, MedicineItem);
}
```

## Configuration

Status effects are configured in Data Tables at `Content/_DA/Data/StatusEffects/`:
- `StatusEffectData` - Primary effect definitions
- `EffectInteractionData` - Effect interaction rules
- `DiseaseProgressionData` - Disease-specific progression stages

## See Also

- [StatlineComponent](StatlineComponent.md) - Stat modification system
- [DiseaseSystem](DiseaseSystem.md) - Disease-specific effects
- [AdvancedSurvivalSubsystem](AdvancedSurvivalSubsystem.md) - Environmental effects