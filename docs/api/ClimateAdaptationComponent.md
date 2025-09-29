# UClimateAdaptationComponent (ClimateAdaptationComponent.h)

## Purpose
This component manages a character's adaptation to various climate types. It tracks the character's current climate, their adaptation level to all climates, and applies gameplay effects accordingly. The component is designed for a multiplayer environment, with state replicated from the server to clients.

## Core Data Structures & Enums

### `EClimateType` (Enum)
A `UENUM` defining the different climate types in the world.
- `Arid`
- `Temperate`
- `Tundra`
- `Tropical`

### `FClimateAdaptationEntry` (Struct)
Represents a character's adaptation to a single climate.
- `Climate (EClimateType)`: The climate type.
- `Level (float)`: The character's adaptation level for this climate (e.g., 0.0 to 100.0).

## Key Properties
- `AdaptationLevels (TArray<FClimateAdaptationEntry>)`: An array containing the character's adaptation level for each climate type. This property is replicated.
- `CurrentClimate (EClimateType)`: The climate the character is currently in. This property is also replicated.

## Key Methods

### Core Logic
- `UpdateAdaptation(float DeltaTime)`: Public-facing function to be called on the owning client. It routes the call to the server via an RPC.
- `ServerUpdateAdaptation(float DeltaTime)`: The server-side implementation that contains the core logic for increasing or decreasing adaptation levels based on `CurrentClimate` and `DeltaTime`.
- `ApplyClimateEffects()`: Public-facing function that routes to the server.
- `ServerApplyClimateEffects()`: The server-side implementation that applies gameplay effects (e.g., status effects, stat modifications) based on the character's adaptation to the `CurrentClimate`.

### Data Retrieval
- `GetAdaptationLevel(EClimateType Climate) const`: Returns the character's current adaptation level for a specified climate.
- `GetCurrentClimate() const`: Returns the climate the character is currently in.

### Replication
- `OnRep_Adaptation()`: The `OnRep` function for the `AdaptationLevels` array. This can be used to trigger UI updates on the client when adaptation levels change.
- `OnRep_CurrentClimate()`: The `OnRep` function for the `CurrentClimate`. This can be used to update UI elements that display the current climate zone.

## Example Usage

To update a character's adaptation, the `UpdateAdaptation` function should be called periodically.

```cpp
// In the character's Tick function
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ClimateAdaptationComponent)
    {
        ClimateAdaptationComponent->UpdateAdaptation(DeltaTime);
    }
}
```

A separate system (e.g., a trigger volume or the `WeatherSystem`) would be responsible for setting the `CurrentClimate` on the component.

## Integration Points
- **`WeatherSystem` / World Triggers**: Another system must be responsible for detecting the character's current climate and setting the `CurrentClimate` property on this component.
- **`UStatlineComponent`**: The `ApplyClimateEffects` function will likely interact with the `UStatlineComponent` to apply status effects or modify stats (e.g., faster stamina drain in an unadapted climate).
- **UI Widgets**: UI elements can be created to display the character's adaptation levels for each climate, updating when the `OnRep_Adaptation` function is called.