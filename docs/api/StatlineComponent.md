# UStatlineComponent (StatlineComponent.h)

## Purpose
The `UStatlineComponent` is a powerful, data-driven component responsible for managing all numerical attributes for an actor. It handles stats, status effects, and replication, serving as the central point for any character's health, stamina, hunger, and other vitals.

## Core Data Structures

### `FStat` (Struct)
Represents a single stat.
- `BaseValue (float)`: The unmodified maximum value of the stat.
- `CurrentValue (float)`: The current value of the stat.
- `Category (EStatCategory)`: An enum for UI sorting (Primary, Survival, Combat, Generic).

### `FStatEntry` (Struct)
A container that pairs a stat's name with its data.
- `StatName (FName)`: The unique identifier for the stat (e.g., "Health", "Stamina").
- `Stat (FStat)`: The `FStat` struct containing the stat's data.

## Key Properties
- `Stats (TArray<FStatEntry>)`: The main array holding all stats for this component. This property is replicated with an `OnRep` function.
- `ActiveStatusEffects (TArray<FActiveStatusEffect>)`: An array of status effects currently affecting the actor.

## Key Delegates
- `OnStatChanged (FName StatName, float NewValue)`: Broadcasts when a stat's `CurrentValue` changes.
- `OnDeath`: Broadcasts when the "Health" stat reaches zero.
- `OnStatusEffectChanged`: Broadcasts when a status effect is applied or removed.

## Key Methods

### Stat Management
- `UpdateStat(FName StatName, float Delta)`: The primary method for modifying a stat's `CurrentValue`. This is a server-only function that handles replication.
- `GetCurrentStatValue(FName StatName) const`: Returns the current value of a stat.
- `GetStatPercentage(FName StatName) const`: Returns the stat's current value as a percentage of its base value (0.0 to 1.0).
- `GetAllStats() const`: Returns the entire array of `FStatEntry` structs.

### Status Effect Management
- `ApplyStatusEffect(const FDataTableRowHandle& EffectDataRowHandle, UObject* Instigator)`: Applies a status effect to the actor. The effect's properties are defined in a DataTable.
- `RemoveStatusEffect(FName EffectID)`: Removes an active status effect by its unique ID.

### Replication
- `ServerUpdateStat(FName StatName, float Delta)`: A server RPC called by the public `UpdateStat` function to ensure stat changes are processed on the server.
- `OnRep_Stats()`: The `OnRep` function for the `Stats` array. It iterates through the replicated stats and broadcasts `OnStatChanged` for any that have changed, ensuring the UI stays in sync.

## Example Usage

To apply damage to a character:
```cpp
// This function should only be called on the server.
void AMyProjectile::OnHit(AActor* OtherActor)
{
    UStatlineComponent* Stats = OtherActor->FindComponentByClass<UStatlineComponent>();
    if (Stats)
    {
        Stats->UpdateStat(FName("Health"), -25.0f); // Apply 25 damage
    }
}
```

To update a health bar in the UI:
```cpp
// In a UHealthBarWidget's Initialize method
UStatlineComponent* Stats = GetOwningPlayerPawn()->FindComponentByClass<UStatlineComponent>();
if (Stats)
{
    // Bind to the delegate
    Stats->OnStatChanged.AddDynamic(this, &UHealthBarWidget::HandleStatChanged);
    
    // Set initial value
    HealthBar->SetPercent(Stats->GetStatPercentage(FName("Health")));
}

// The handler function
void UHealthBarWidget::HandleStatChanged(FName StatName, float NewValue)
{
    if (StatName == FName("Health"))
    {
        // Get the new percentage and update the UI
        UStatlineComponent* Stats = Cast<UStatlineComponent>(GetOwningComponent()); // Or get it again
        if(Stats)
        {
            HealthBar->SetPercent(Stats->GetStatPercentage(FName("Health")));
        }
    }
}
```

## Integration Points
- **DataTables**: This component is highly data-driven. All stats should be added to the component's `Stats` array in the Blueprint editor, and all status effects should be defined in a `DT_StatusEffects` DataTable.
- **`UAdvancedSurvivalSubsystem`**: This subsystem calls `UpdateStat` to deplete survival stats like Hunger and Thirst over time.
- **UI Widgets**: All stat-displaying UI elements should bind to `OnStatChanged` to ensure they are always in sync with the replicated data.
