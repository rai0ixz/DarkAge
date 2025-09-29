# NPC Performance Optimization: Action Plan

**Project:** Dark Age (Unreal Engine / C++)
**Prepared by:** Kilo Code, Performance Engineer
**Date:** 2025-08-02

---

## Introduction

This document outlines a prioritized, five-phase action plan to resolve the critical performance bottleneck related to the high number of NPCs in the Dark Age project. The strategy is based on the findings from the `ArchitecturalReview-Summary.md` and a direct analysis of the `UAINeedsPlanningComponent`.

The goal is to transition the current system from a costly, per-actor `Tick()` approach to a highly efficient, centralized, and time-sliced simulation. This will not only fix the immediate frame rate issues but also provide a scalable foundation for a truly massive and dynamic world.

---

## Phase 1: Diagnosis & Profiling (The Scientific Approach)

Before changing any code, we must establish a clear, data-driven baseline. This will validate our assumptions and allow us to measure the success of our optimizations.

| Goal | Strategy | Expected Outcome |
| :--- | :--- | :--- |
| **Establish Performance Baseline** | 1. Create a test scene with a configurable number of NPCs (e.g., 10, 50, 100, 200).<br>2. Use the `stat unit` console command to view overall frame time, game thread, draw thread, and GPU time.<br>3. Capture a 30-60 second profile using **Unreal Insights** in the high-NPC scenario. | A baseline Unreal Insights trace file that clearly shows the current performance characteristics. We expect the `GameThread` time to be excessively high, with `UAINeedsPlanningComponent::TickComponent` consuming the vast majority of the CPU budget. |
| **Pinpoint CPU Hotspots** | 1. In Unreal Insights, analyze the **CPU Timing** view.<br>2. Filter by `TickComponent` to see the total time spent across all NPCs.<br>3. Identify the most expensive functions called from within the tick, such as `UpdateNeedsWithContext`, `EvaluateDangerLevel`, etc. | Concrete data showing that the AI logic within the tick is the primary bottleneck. This will confirm the findings of the architectural review and provide specific functions to target for optimization. |
| **Analyze Memory & Draw Calls** | 1. Use `stat memory` to check for excessive memory allocation patterns.<br>2. Use `stat game` to look for high numbers of `GetAllActorsOfClass` calls, confirming the inefficient world queries.<br>3. Use `stat scenerendering` to get a baseline for draw calls and primitives, which will become relevant in later optimization phases. | A clear picture of memory usage patterns and the performance cost of current world queries. This data will guide the implementation of the AI manager's cached data structures. |

---

## Phase 2: Targeted NPC Optimization (Dismantling the Bottleneck)

This is the most critical phase, focused on refactoring the core NPC logic to be scalable.

| Goal | Strategy | Expected Outcome |
| :--- | :--- | :--- |
| **Centralize AI Logic** | 1. Create a new `UDA_NPCManagerSubsystem` (or similar `UWorldSubsystem`).<br>2. Remove the `TickComponent` logic from `UAINeedsPlanningComponent`.<br>3. The new subsystem will query for all actors with a `UAINeedsPlanningComponent` on startup and store them in an array.<br>4. The subsystem will have its own `Tick()` method, which will become the main loop for all NPC "thinking." | A fundamental shift in architecture. The expensive logic is moved from N individual actors to **one** central manager. This immediately reduces component ticking overhead and sets the stage for time slicing. |
| **Implement AI LOD & Time Slicing** | 1. In the `NPCManagerSubsystem`, create several update "buckets" (TArrays of `AActor*`). For example: `HighFrequency`, `MediumFrequency`, `LowFrequency`.<br>2. On a timer or every few frames, categorize all registered NPCs into these buckets based on distance from the player or other heuristics (e.g., screen visibility).<br>3. The manager's `Tick()` will process only a fraction of NPCs each frame:<br>   - Update all `HighFrequency` NPCs (e.g., 1-5 per frame).<br>   - Update a smaller portion of `MediumFrequency` NPCs.<br>   - Update an even smaller portion of `LowFrequency` NPCs. | A massive reduction in per-frame CPU load. Instead of 100 NPCs thinking every frame, perhaps only 5-10 do, distributing the workload over time. This is the **single most important performance gain** for NPC scaling. |
| **Optimize Pathfinding** | 1. When an NPC needs to move, instead of a direct pathfinding call, use the `UNavigationSystemV1::RequestPathAsync` function.<br>2. This offloads the pathfinding query to a background thread, preventing a stall on the game thread. The result is returned via a delegate. | Smoother frame rates, as expensive pathfinding calculations no longer block the game thread. |
| **Optimize Animation & Physics** | 1. For off-screen or low-frequency NPCs, disable `Tick` on their `USkeletalMeshComponent` or severely reduce its frequency.<br>2. Implement the recommendation from the review: define custom **Collision Channels** (e.g., `ECC_NPC`, `ECC_PlayerPawn`) and create **Collision Presets** in Project Settings. Audit NPC blueprints to use these presets to prevent unnecessary physics calculations. | Reduced CPU time spent on animation and physics for irrelevant NPCs. cleaner and more performant physics interactions project-wide. |
| **Create an Optimized World Query Cache** | 1. The `NPCManagerSubsystem` will be responsible for maintaining cached lists of key actor types (e.g., known hostiles, water sources, shelter locations).<br>2. It can use spatial hashing or simple cached TArrays that are updated periodically.<br>3. NPC logic will now query the manager's cached data instead of using `GetAllActorsOfClass`. | Catastrophic `O(n²)` world queries are replaced with highly efficient `O(1)` or `O(log n)` lookups from the manager's cache. |

---

## Phase 3: Global Performance Enhancements

With the primary bottleneck addressed, we can implement project-wide optimizations for further gains.

| Goal | Strategy | Expected Outcome |
| :--- | :--- | :--- |
| **Improve Rendering Performance** | 1. Implement **occlusion culling** (default in UE, but ensure it's effective with your scene setup).<br>2. Use the **Hierarchical Level of Detail (HLOD)** system to batch distant static geometry, reducing draw calls.<br>3. Use the **Shader Complexity** view mode to identify and optimize expensive materials. | Fewer draw calls and less overdraw, leading to improved GPU performance, especially in dense city environments. |
| **Reduce Memory Spikes** | 1. Create a simple **Object Pooling system** for frequently spawned/destroyed objects like NPCs, projectiles, or effects. Instead of destroying actors, disable them and return them to the pool. When a new actor is needed, grab one from the pool and re-initialize it.<br>2. Analyze code for frequent creation of UObjects or heavy structs within loops to reduce Garbage Collection (GC) pressure. | A significant reduction in frame hitches caused by memory allocation and subsequent garbage collection. Smoother overall gameplay. |
| **Improve Scene Loading** | 1. Refactor world loading to use **Level Streaming** (`UGameplayStatics::LoadStreamLevel`). This allows the world to be broken into smaller chunks that can be loaded/unloaded in the background as the player moves.<br>2. Organize assets into logical **Asset Bundles** to optimize loading and memory management. | Faster initial load times and the elimination of hitches when moving between different areas of the world. |

---

## Phase 4: Code Architecture & Maintainability

These changes improve performance while making the codebase more robust and easier to manage.

| Goal | Strategy | Expected Outcome |
| :--- | :--- | :--- |
| **Refine AI Behavior Logic** | 1. The current `Execute...` and `Attempt...` functions in the `AINeedsPlanningComponent` can become difficult to manage. Refactor this logic using a more formal system:<br>   - **State Pattern:** For simpler AIs with a limited number of distinct behaviors (e.g., Idle, Work, Flee).<br>   - **Behavior Trees:** For more complex, branching logic. Unreal Engine has a powerful built-in Behavior Tree editor. | A much cleaner, more scalable, and more debuggable AI codebase. Behavior Trees in particular provide a powerful visual tool for designing and tweaking AI without changing C++ code. |
| **Embrace Decoupled Communication** | 1. The review noted the excellent `GlobalEventBus`. Lean into it heavily. Instead of direct calls between systems, have the `NPCManagerSubsystem` fire off events (e.g., `OnNPCStartedFleeing`, `OnNPCNeedsFood`).<br>2. Other systems (UI, Audio, etc.) can listen for these events without needing a direct reference to the NPC or the manager. | A more robust, bug-resistant architecture. Systems can be added, removed, or changed with minimal impact on other parts of the codebase. |

---

## Phase 5: Player Experience Polish

This final phase focuses on translating our technical gains into a better player experience.

| Goal | Strategy | Expected Outcome |
| :--- | :--- | :--- |
| **Ensure Frame Rate Stability** | 1. After implementing the optimizations, re-run the Unreal Insights profiling sessions from Phase 1.<br>2. Target a stable frame rate (e.g., 60 FPS) and use dynamic settings (e.g., adjusting AI LOD tiers or effect quality) if necessary to maintain it under load. | A smooth, consistent gameplay experience, which is one of the most important factors for player immersion and satisfaction. |
| **Reduce Perceptible Hitches** | 1. Playtest extensively, focusing on actions that previously caused hitches (e.g., entering a new area, large battles).<br>2. The object pooling and asynchronous systems from previous phases should eliminate most of these. Use Unreal Insights to hunt down any remaining stalls. | A seamless and uninterrupted world. Players will not be pulled out of the experience by jarring freezes or stutters. |
| **Enhance World Responsiveness** | 1. With the CPU overhead from AI drastically reduced, there is now performance budget available for more interesting environmental details or secondary systems.<br>2. Ensure that high-priority NPCs (those in direct interaction with the player) always feel responsive due to their placement in the `HighFrequency` update bucket. | A game that not only runs better but *feels* better. The world will feel more alive and reactive, creating a more stable and immersive environment for the player. |