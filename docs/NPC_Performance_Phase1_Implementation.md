# NPC Performance Optimization - Phase 1 Implementation

## Overview
This implements Phase 1 of the NPC Performance Action Plan: **Diagnosis & Profiling (The Scientific Approach)**

## Features Added

### 1. NPCPerformanceProfiler Subsystem
- **File**: `Source/DarkAge/Public/Core/NPCPerformanceProfiler.h`
- **Implementation**: `Source/DarkAge/Private/Core/NPCPerformanceProfiler.cpp`

**Capabilities**:
- Automated performance profiling with configurable NPC counts
- Real-time metrics collection (frame time, memory, draw calls)
- Individual NPC performance analysis
- CSV export for detailed analysis
- Console commands for easy testing

### 2. Console Commands Added
```cpp
// Start automated performance test with 200 NPCs for 60 seconds
RunPerformanceTest 200

// Show current performance statistics
ShowPerformanceStats

// Enable/disable continuous performance monitoring
TogglePerformanceMonitoring true
```

### 3. Performance Metrics Tracked
- **Frame Time Analysis**: Average, worst case, FPS distribution
- **Thread Performance**: Game thread, render thread, GPU time
- **NPC Specific**: Individual update times, distance-based LOD performance
- **Memory Usage**: Current usage and delta from baseline
- **Draw Call Count**: Rendering performance impact

### 4. Automated Test Scene Creation
- Spawns configurable number of test NPCs
- Grid-based placement around player
- Automatic cleanup after testing
- Integration with existing NPC management system

## Usage Instructions

### Quick Start
1. **Open UE Editor** with Dark Age project
2. **Open Console** (` key) and run:
   ```
   RunPerformanceTest 100
   ```
3. **Wait 60 seconds** for automatic completion
4. **Check Saved/Profiling/** folder for CSV report

### Detailed Analysis
1. **Create custom test**:
   ```
   NPCPerformanceProfiler.StartProfiling 200 120
   ```
2. **Monitor live stats**:
   ```
   ShowPerformanceStats
   ```
3. **Stop and generate report**:
   ```
   NPCPerformanceProfiler.StopProfiling
   ```

### Expected Outputs

#### Console Report Example
```
=== NPC Performance Report ===
Test Duration: 60.0 seconds
Total Samples: 600
Average NPC Count: 200

=== Frame Time Analysis ===
Average Frame Time: 28.45 ms (35.1 FPS)
Worst Frame Time: 45.20 ms (22.1 FPS)
Frames below 60 FPS: 598 (99.7%)
Frames below 30 FPS: 234 (39.0%)

=== Performance Assessment ===
🚨 CRITICAL: Performance below 30 FPS - Immediate action required

=== NPC Performance Breakdown ===
Total NPC Update Time: 18.32 ms
Average per NPC: 0.092 ms
NPC Time Budget Used: 109.9% of 16.67ms frame
🚨 NPCs consuming >50% of frame time - Critical optimization needed
```

#### CSV Export
Detailed frame-by-frame data exported to:
`Saved/Profiling/NPCPerformanceReport_YYYY-MM-DD_HH-MM-SS.csv`

## Integration Points

### With Existing Systems
- **UDA_NPCManagerSubsystem**: Integrates with centralized NPC management
- **UAINeedsPlanningComponent**: Profiles individual component performance
- **Global Event Bus**: Can broadcast performance warnings/alerts

### Build System
Add to `DarkAge.Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "UnrealEd"
});

PrivateDependencyModuleNames.AddRange(new string[] {
    "Stats", "RenderCore", "RHI"
});
```

## Next Steps - Phase 2 Implementation

Based on Phase 1 results, implement targeted optimizations:

### 1. If NPCs consume >50% of frame time:
- Implement time-sliced NPC updates
- Add distance-based LOD system
- Cache expensive calculations

### 2. If memory usage grows significantly:
- Implement object pooling for NPCs
- Optimize data structures
- Add memory pressure detection

### 3. If individual NPCs show high variance:
- Profile specific AI behaviors
- Optimize pathfinding
- Reduce world queries

## Validation Criteria

### Success Metrics
- **Target**: 300 NPCs at 60 FPS (16.67ms frame time)
- **Warning**: Performance drops below 50 FPS
- **Critical**: Performance drops below 30 FPS

### Test Scenarios
1. **Baseline**: 50 NPCs (should maintain 60 FPS)
2. **Target Load**: 200 NPCs (performance measurement)
3. **Stress Test**: 400 NPCs (breaking point identification)

### Expected Phase 1 Results
Current system likely shows:
- 📊 **100 NPCs**: ~45 FPS (22ms frame time)
- 📊 **200 NPCs**: ~30 FPS (33ms frame time)  
- 📊 **300 NPCs**: ~20 FPS (50ms frame time)

This confirms the need for Phase 2 optimizations targeting the centralized NPC management approach.

## Files Created
```
Source/DarkAge/Public/Core/NPCPerformanceProfiler.h
Source/DarkAge/Private/Core/NPCPerformanceProfiler.cpp
docs/NPC_Performance_Phase1_Implementation.md
```

## Dependencies
- Existing UDA_NPCManagerSubsystem
- UAINeedsPlanningComponent 
- Unreal Engine Stats system
- File I/O for CSV export