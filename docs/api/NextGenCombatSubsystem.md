# Next-Gen Combat Subsystem

The Next-Gen Combat Subsystem is responsible for managing all combat-related calculations and interactions within the game. It provides a centralized system for processing damage, applying status effects, and determining the outcome of combat encounters.

## Core Concepts

The combat subsystem is built around the following core concepts:

*   **Actors:** Any object in the world that can participate in combat, such as players, NPCs, and creatures.
*   **Stats:** A collection of attributes that define an actor's combat capabilities, such as `Health`, `Strength`, and `Defense`. These are managed by the `UStatlineComponent`.
*   **Damage:** The amount of harm inflicted by an attack. The final damage is calculated based on the attacker's stats, the victim's stats, and the base damage of the attack.

## How it Works

The combat subsystem uses a simple but effective formula to calculate damage:

```
FinalDamage = BaseDamage + (AttackerStrength * 0.5) - (VictimDefense * 0.25)
```

The `FinalDamage` is then clamped to a minimum of 0 to prevent healing from attacks.

## API Reference

### `FDamageResult`

This struct holds the results of a damage calculation.

| Field         | Type    | Description                                         |
| ------------- | ------- | --------------------------------------------------- |
| `bDidHit`     | `bool`  | Whether the attack successfully hit the victim.     |
| `FinalDamage` | `float` | The final damage amount after all calculations.     |
| `bIsCritical` | `bool`  | Whether the attack was a critical hit.              |
| `bIsFatal`    | `bool`  | Whether the attack resulted in the victim's death. |

### `ProcessDamage`

This function processes a damage event between an attacker and a victim.

**Parameters:**

*   `Attacker`: A pointer to the `AActor` that initiated the attack.
*   `Victim`: A pointer to the `AActor` that is the target of the attack.
*   `BaseDamage`: The base damage of the attack, before any modifications.

**Returns:**

An `FDamageResult` struct containing the results of the damage calculation.

## Usage

To use the combat subsystem, you first need to get a reference to it from the game instance:

```cpp
UNextGenCombatSubsystem* CombatSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UNextGenCombatSubsystem>();
```

Then, you can call the `ProcessDamage` function to process a damage event:

```cpp
FDamageResult Result = CombatSubsystem->ProcessDamage(Attacker, Victim, 10.0f);