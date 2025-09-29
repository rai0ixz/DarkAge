# UWorldPersistenceSystem (WorldPersistenceSystem.h)

## Purpose
Handles saving and loading of persistent world state, including player data, world events, and dynamic changes.

## Key Methods & Properties
- SaveWorldState(): Serializes and saves all relevant data
- LoadWorldState(): Loads saved data into the game
- RegisterPersistentObject(Object): Tracks objects for persistence

## Example Usage
Called on save/load events. Use `RegisterPersistentObject()` for new objects, `SaveWorldState()` and `LoadWorldState()` for persistence.

## Integration Points
- Works with all major subsystems (player, world, quest, etc.)
- Data used by save/load UI and technical systems

## Notes
- Extend to support incremental saves, cloud sync, and advanced versioning.
