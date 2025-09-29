# UFactionManagerSubsystem (FactionManagerSubsystem.h)

## Purpose
This subsystem is the master authority for all faction-related data and logic. It manages the creation of factions, their relationships with each other, the player's reputation with them, and their control over world territories. Its state is persistent and saved/loaded with the game.

## Core Data Structures
- `Factions (TMap<FName, FFaction>)`: The primary map containing the data for every faction in the game, keyed by a unique `FName`.
- `TerritoryControl (TMap<FName, FName>)`: A map where the key is a Region ID and the value is the Faction ID that controls it.
- `PlayerFactionReputations (TMap<FName, float>)`: A map tracking the player's numerical reputation with each faction, keyed by Faction ID.

## Key Delegates
- `OnPlayerReputationChanged (FName FactionID, float NewReputation)`: Broadcasts whenever the player's reputation with a faction changes. UI elements should bind to this to display up-to-date information.

## Key Methods

### Faction & Relationship Management
- `CreateFaction(FName FactionID, const FText& DisplayName)`: Creates a new faction and adds it to the `Factions` map.
- `GetFactionData(FName FactionID, FFaction& OutFaction) const`: Retrieves the full data struct for a specific faction.
- `SetFactionRelationship(FName FactionA, FName FactionB, EFactionRelationshipType NewRelationship)`: Sets the diplomatic relationship between two factions (e.g., Allied, Neutral, Hostile).
- `ModifyFactionRelationship(FName Faction1ID, FName Faction2ID, int32 Delta, bool bMutual)`: Modifies the underlying numerical relationship value between two factions.

### Player Reputation
- `SetPlayerReputation(FName FactionID, float NewReputation)`: Directly sets the player's reputation with a faction to a specific value.
- `ModifyPlayerReputation(FName FactionID, float Delta)`: Adds or subtracts from the player's reputation with a faction. This is the most common way to change reputation.
- `GetPlayerReputation(FName FactionID) const`: Returns the player's current numerical reputation with a faction.

### Territory Control
- `SetTerritoryControl(const FName& RegionID, const FName& FactionID, float Influence)`: Assigns control of a region to a specific faction.
- `GetTerritoryController(const FName& RegionID) const`: Returns the Faction ID of the faction that currently controls the specified region.

### AI & Simulation
- `SimulateFactionAI(float DeltaTime)`: A high-level tick function that can be used to drive the strategic decision-making for factions (e.g., declaring war, expanding territory).

## Persistence
- `SaveFactions()` / `LoadFactions()`: Private methods that handle the serialization of the subsystem's state to the player's save game file.

## Example Usage

When a player completes a quest for the "MerchantsGuild" faction:
```cpp
// This code would likely be in the Quest reward logic.
UFactionManagerSubsystem* FactionManager = GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
if (FactionManager)
{
    // Award the player 25 reputation points with the MerchantsGuild.
    FactionManager->ModifyPlayerReputation(FName("MerchantsGuild"), 25.0f);
}
```

To check if two factions are at war before spawning an AI:
```cpp
UFactionManagerSubsystem* FactionManager = GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
if (FactionManager)
{
    EFactionRelationshipType Relationship = FactionManager->GetFactionRelationship(FName("RoyalGuard"), FName("ThievesGuild"));
    if (Relationship == EFactionRelationshipType::Hostile)
    {
        // Spawn the guard with hostile behavior towards the thief.
    }
}
```

## Integration Points
- **`UNotorietyComponent` & `UCrimeManagerSubsystem`**: Actions that affect notoriety or are reported as crimes should often also call `ModifyPlayerReputation` to affect the player's standing with the relevant faction (e.g., committing a crime in a town lowers reputation with the town's controlling faction).
- **Quest System**: Quests are a primary driver for reputation changes. Quest rewards should frequently call `ModifyPlayerReputation`.
- **AI Systems**: AI behavior should be heavily influenced by the data in this subsystem. An AI character's disposition towards the player should be based on `GetPlayerReputation`, and their behavior towards other AI should be based on `GetFactionRelationship`.
- **UI**: A "Factions" screen in the UI would be populated by data from this subsystem, binding to `OnPlayerReputationChanged` to stay up-to-date.
