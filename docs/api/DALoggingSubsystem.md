# DALoggingSubsystem API Reference

## Overview

The `UDALoggingSubsystem` provides centralized logging functionality for the DarkAge project, replacing the scattered use of `UE_LOG(LogTemp, ...)` throughout the codebase with a structured, categorized logging system.

## Features

- **Structured Logging**: Organized by category and severity level
- **File Output**: Automatic log file generation with timestamps
- **Performance Monitoring**: Built-in performance metrics logging
- **Thread Safety**: Safe for use across multiple threads
- **Blueprint Integration**: Full Blueprint support for runtime logging
- **Memory Management**: Circular buffer with configurable history size

## Quick Start

### C++ Usage

```cpp
#include "Core/DALoggingSubsystem.h"

// Basic logging
DA_LOG_INFO(EDALogCategory::Economy, TEXT("Market price updated: %s = %f"), *ItemName, NewPrice);

// Category-specific macros
DA_LOG_ECONOMY(EDALogLevel::Warning, TEXT("Low supply detected for %s"), *ResourceName);
DA_LOG_NPC(EDALogLevel::Debug, TEXT("NPC %s changed behavior state"), *NPCName);
DA_LOG_COMBAT(EDALogLevel::Error, TEXT("Invalid weapon data: %s"), *WeaponID);
```

### Blueprint Usage

```cpp
// Get logging subsystem
UDALoggingSubsystem* LoggingSubsystem = GetGameInstance()->GetSubsystem<UDALoggingSubsystem>();

// Log a message
LoggingSubsystem->LogMessage(EDALogLevel::Info, EDALogCategory::Quests, 
    TEXT("Quest completed"), TEXT("Player"));
```

## API Reference

### Core Functions

#### LogMessage
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
void LogMessage(EDALogLevel Level, EDALogCategory Category, const FString& Message, const FString& Context = TEXT(""));
```
Log a message with specified level and category.

**Parameters:**
- `Level`: Severity level (Trace, Debug, Info, Warning, Error, Critical)
- `Category`: Log category (Economy, NPC, Combat, etc.)
- `Message`: The log message
- `Context`: Optional context information

#### LogMessageWithSource
```cpp
void LogMessageWithSource(EDALogLevel Level, EDALogCategory Category, const FString& Message, 
    const FString& Context = TEXT(""), const FString& Function = TEXT(""), int32 Line = 0);
```
Log with source code information (used by macros).

#### GetRecentLogs
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
TArray<FDALogEntry> GetRecentLogs(int32 Count = 100, EDALogLevel MinLevel = EDALogLevel::Debug);
```
Retrieve recent log entries for debugging.

#### GetLogsByCategory
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
TArray<FDALogEntry> GetLogsByCategory(EDALogCategory Category, int32 Count = 50);
```
Get logs filtered by category.

### Configuration Functions

#### SetFileLoggingEnabled
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
void SetFileLoggingEnabled(bool bEnabled);
```
Enable or disable file logging at runtime.

#### SetConsoleLogLevel
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
void SetConsoleLogLevel(EDALogLevel MinLevel);
```
Set minimum log level for console output.

#### SetFileLogLevel
```cpp
UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
void SetFileLogLevel(EDALogLevel MinLevel);
```
Set minimum log level for file output.

## Log Categories

| Category | Purpose | Example Usage |
|----------|---------|---------------|
| `General` | General system messages | Subsystem initialization |
| `Economy` | Economic system events | Price changes, trade events |
| `NPC` | NPC behavior and AI | State changes, interactions |
| `Crime` | Crime and justice system | Crime reports, investigations |
| `Politics` | Political system events | Elections, policy changes |
| `Quests` | Quest system events | Quest completion, generation |
| `Survival` | Survival mechanics | Hunger, thirst, disease |
| `Weather` | Weather system events | Weather changes, effects |
| `AI` | AI decision making | Pathfinding, behavior trees |
| `Combat` | Combat system events | Damage, status effects |
| `UI` | User interface events | Widget creation, input |
| `Network` | Networking events | Connections, synchronization |
| `Security` | Security and anti-cheat | Suspicious activity, bans |
| `Performance` | Performance monitoring | FPS drops, memory usage |

## Log Levels

| Level | Purpose | When to Use |
|-------|---------|-------------|
| `Trace` | Very detailed debugging | Function entry/exit |
| `Debug` | Debugging information | Variable values, state changes |
| `Info` | General information | Normal operation events |
| `Warning` | Potential issues | Recoverable errors, deprecation |
| `Error` | Error conditions | Failed operations, invalid data |
| `Critical` | Critical failures | System crashes, data corruption |

## Convenience Macros

### General Logging
```cpp
DA_LOG_TRACE(Category, Format, ...)
DA_LOG_DEBUG(Category, Format, ...)
DA_LOG_INFO(Category, Format, ...)
DA_LOG_WARNING(Category, Format, ...)
DA_LOG_ERROR(Category, Format, ...)
DA_LOG_CRITICAL(Category, Format, ...)
```

### Category-Specific Macros
```cpp
DA_LOG_ECONOMY(Level, Format, ...)
DA_LOG_NPC(Level, Format, ...)
DA_LOG_CRIME(Level, Format, ...)
DA_LOG_POLITICS(Level, Format, ...)
DA_LOG_QUESTS(Level, Format, ...)
DA_LOG_SURVIVAL(Level, Format, ...)
DA_LOG_WEATHER(Level, Format, ...)
DA_LOG_AI(Level, Format, ...)
DA_LOG_COMBAT(Level, Format, ...)
DA_LOG_UI(Level, Format, ...)
DA_LOG_NETWORK(Level, Format, ...)
DA_LOG_SECURITY(Level, Format, ...)
DA_LOG_PERFORMANCE(Level, Format, ...)
```

## Migration Guide

### From UE_LOG(LogTemp, ...)

**Before:**
```cpp
UE_LOG(LogTemp, Warning, TEXT("Economy: Market price changed for %s"), *ItemName);
```

**After:**
```cpp
DA_LOG_ECONOMY(EDALogLevel::Warning, TEXT("Market price changed for %s"), *ItemName);
```

### Benefits of Migration

1. **Categorization**: Logs are automatically categorized
2. **Filtering**: Easy to filter logs by category or level
3. **File Output**: Automatic structured file logging
4. **Performance**: Better performance monitoring integration
5. **Debugging**: Enhanced debugging with source information

## File Output Format

Log files are created in `ProjectLogDir/DarkAge/` with the format:
```
[2024-01-15 14:30:25] [INFO] [Economy] Market price updated [Context: Player] [Function:UpdatePrice:123]
[2024-01-15 14:30:26] [WARN] [NPC] Low morale detected [Context: Miller] [Function:UpdateMorale:456]
```

## Performance Considerations

- Log entries are stored in a circular buffer (default 5000 entries)
- File I/O is performed asynchronously when possible
- Thread-safe operations use critical sections
- Memory usage is bounded by `MaxLogHistorySize`

## Configuration

The logging subsystem can be configured via game settings:

```ini
[/Script/DarkAge.DALoggingSubsystem]
bFileLoggingEnabled=true
ConsoleLogLevel=Info
FileLogLevel=Debug
MaxLogHistorySize=5000
```

## Best Practices

1. **Use appropriate log levels**: Don't spam with Debug messages in production
2. **Include context**: Provide meaningful context information
3. **Use category-specific macros**: Makes filtering easier
4. **Avoid logging in tight loops**: Can impact performance
5. **Include relevant data**: Log the information needed for debugging

## Examples

### Economic System Logging
```cpp
// Price change
DA_LOG_ECONOMY(EDALogLevel::Info, TEXT("Price updated: %s from %f to %f"), 
    *ItemName, OldPrice, NewPrice);

// Market event
DA_LOG_ECONOMY(EDALogLevel::Warning, TEXT("Market disruption in %s: %s"), 
    *RegionName, *EventDescription);
```

### NPC System Logging
```cpp
// Behavior change
DA_LOG_NPC(EDALogLevel::Debug, TEXT("NPC %s changed state from %s to %s"), 
    *NPCName, *OldState, *NewState);

// Population event
DA_LOG_NPC(EDALogLevel::Info, TEXT("Settlement %s population changed: %d -> %d"), 
    *SettlementName, OldPopulation, NewPopulation);
```

### Error Handling
```cpp
// Recoverable error
DA_LOG_COMBAT(EDALogLevel::Warning, TEXT("Invalid weapon stats for %s, using defaults"), 
    *WeaponID);

// Critical error
DA_LOG_SECURITY(EDALogLevel::Critical, TEXT("Potential exploit detected from player %s"), 
    *PlayerID);