# UAdvancedSurvivalSubsystem (AdvancedSurvivalSubsystem.h)

## Purpose
The central manager for all survival-related mechanics in the game. This subsystem is responsible for tracking all characters (players and NPCs) that are subject to survival effects and orchestrating updates via other specialized systems. It is designed with multiplayer in mind, handling client-to-server registration.

## Key Methods & Properties

### Character Registration
- `RegisterCharacter(ACharacter* Character)`: The public-facing function called on clients to begin tracking a character's survival state. This function internally calls a server RPC.
- `ServerRegisterCharacter(ACharacter* Character)`: The server-side implementation that adds the character to the `RegisteredCharacters` set.
- `UnregisterCharacter(ACharacter* Character)`: The public-facing function to stop tracking a character. This also calls a server RPC.
- `ServerUnregisterCharacter(ACharacter* Character)`: The server-side implementation that removes the character from the tracking set.

### Core Logic
- `Tick(float DeltaTime)`: The main update loop for the subsystem. It iterates through all `RegisteredCharacters` and calls `UpdateCharacterSurvival` for each one.
- `UpdateCharacterSurvival(ACharacter* Character, float DeltaTime)`: A private helper function that integrates with other components and subsystems to apply survival effects (e.g., depleting needs, applying disease effects).

## Example Usage

When a character spawns into the world, they must be registered with the subsystem.

```cpp
// In the BeginPlay() method of a character class
UAdvancedSurvivalSubsystem* SurvivalSubsystem = GetGameInstance()->GetSubsystem<UAdvancedSurvivalSubsystem>();
if (SurvivalSubsystem)
{
    SurvivalSubsystem->RegisterCharacter(this);
}
```

## Integration Points
This subsystem acts as a high-level orchestrator and does not manage survival logic directly. Instead, it delegates tasks to other, more specialized systems and components:
- **`UNeedsCalculationSubsystem`**: Called by `UpdateCharacterSurvival` to calculate the depletion of needs like hunger and thirst.
- **`UDiseaseManagementComponent`**: This component, present on the character, is responsible for applying the effects of any active diseases.
- **`UStatlineComponent`**: The ultimate destination for many survival effects, where stats like Health, Hunger, and Thirst are actually modified.

## Best Practices
- **Register on Spawn:** Always register characters on `BeginPlay` and unregister them on `EndPlay` to ensure they are correctly tracked.
- **Authority Checks:** Note that the actual registration logic is performed on the server. Client calls are routed via RPCs.
- **Decoupled Logic:** Do not add specific survival logic (e.g., how much hunger a food item restores) to this subsystem. That logic belongs in data assets or more specialized components. This subsystem should only be concerned with managing the update loop.
