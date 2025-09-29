# UPoliticalSystem (PoliticalSystem.h)

## Purpose
Manages political events, treaties, and diplomacy between factions. Supports political actions, event notifications, and integration with the faction system.

## Key Methods & Properties
- ProposeTreaty(FactionA, FactionB, Terms): Initiates a treaty
- ResolveEvent(EventID): Handles political event resolution
- NotifyPoliticalChange(Event): Broadcasts political changes

## Example Usage
Called by FactionManagerSubsystem or world events. Use `ProposeTreaty()` for diplomacy, `NotifyPoliticalChange()` for UI updates.

## Integration Points
- Works with FactionManagerSubsystem, QuestSystem, and UI (PoliticalEventNotifier)
- Data used by social simulation and world events

## Notes
- Extend to support elections, player-driven politics, and advanced diplomatic systems.
