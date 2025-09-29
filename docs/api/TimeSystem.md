# UTimeSystem (TimeSystem.h)

## Purpose
Manages in-game time, including day/night cycles, time progression, and integration with world and gameplay systems.

## Key Methods & Properties
- SetTimeOfDay(Time): Sets the current time
- AdvanceTime(Delta): Advances time by a given amount
- GetCurrentTime(): Returns current in-game time

## Example Usage
Called by world tick or events. Use `AdvanceTime()` each tick, `SetTimeOfDay()` for scripted events.

## Integration Points
- Works with WorldEcosystemSubsystem, WeatherSystem, and UI
- Data used by survival, quest, and world systems

## Notes
- Extend to support seasons, time-based events, and time scaling.
