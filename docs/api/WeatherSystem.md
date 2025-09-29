# UWeatherSystem (WeatherSystem.h)

## Purpose
Simulates and manages dynamic weather patterns, including transitions, effects, and integration with the ecosystem and gameplay.

## Key Methods & Properties
- SetWeatherType(WeatherType): Changes current weather
- UpdateWeather(): Advances weather simulation
- GetCurrentWeather(): Returns current weather state

## Example Usage
Called by world tick or events. Use `SetWeatherType()` to force weather, `UpdateWeather()` each tick.

## Integration Points
- Works with WorldEcosystemSubsystem, EnvironmentalFactorsComponent, and UI
- Data used by survival and world systems

## Notes
- Extend to support microclimates, extreme events, and weather-based gameplay effects.
