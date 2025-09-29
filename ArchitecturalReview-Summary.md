# Architectural & Structural Review: Dark Age

**Report Date:** 2025-08-02
**Prepared by:** Kilo Code, Systems Architect

---

## 1. Executive Summary

The Dark Age project exhibits a remarkably professional and robust architecture that is, in many areas, well-prepared for the ambitious goal of a large-scale, persistent world with complex NPCs. The high-level design correctly leverages modern Unreal Engine patterns, including a decoupled Subsystem-based architecture and a strong Entity-Component model. The code is clean, well-organized, and supported by high-quality documentation.

However, the current implementation contains several critical performance bottlenecks that will prevent the project from scaling to the target number of NPCs. The recommendations in this report are focused almost exclusively on refactoring the *implementation* of the AI and data management systems to move from a per-actor, tick-based model to a centralized, managed, and time-sliced approach.

The project's foundation is solid; the necessary changes are primarily concerned with optimizing the execution and data flow of the existing logic to achieve massive scale.

---

## 2. Overall Architecture Assessment

| Category | Grade | Commentary |
| :--- | :--- | :--- |
| **High-Level Architecture** | **A+** | Exemplary use of Subsystems and Actor Components. Decoupled and scalable. |
| **NPC & AI Systems** | **B-** | Functionally strong (Utility AI) but critically unscalable due to tick-based logic. |
| **World & Interaction** | **B+** | Excellent event system, but lacks a managed, optimized collision framework. |
| **Data Management & Flow** | **B** | Strong persistence concept but lacks streaming and data layout optimizations. |
| **Code & Project Organization**| **A+** | Professional, clean, and highly maintainable. |
| **Documentation** | **A** | Thorough, accurate, and provides excellent insight into project design. |

---

## 3. Key Findings & Actionable Recommendations

### 3.1. NPC & AI Systems (Highest Priority)

**Finding 1: Tick-Based Logic is Unscalable**
- The `UAINeedsPlanningComponent` updates needs and evaluates context on `Tick` for every NPC. This is the **single greatest threat to performance** and will not scale to a large NPC count.

**Finding 2: Inefficient World Queries**
- The AI's environmental queries (`EvaluateDangerLevel`, `GetSocialIsolationMultiplier`) use `GetAllActorsOfClass`. This is catastrophically inefficient at scale, approaching O(n²) complexity.

**Recommendation 1: Centralize AI Logic in a Manager Subsystem**
- **Refactor all "thinking" logic** out of `UAINeedsPlanningComponent::TickComponent` and into a central manager (e.g., `AdvancedAIDecisionSubsystem`).
- This manager will run the simulation. It will iterate through NPCs and update them.

**Recommendation 2: Implement Time Slicing & AI Tiers**
- The AI Manager must not update all NPCs every frame. Implement a time-slicing or "bucketing" system.
-   **Tier 1 (Active):** NPCs near the player. Update logic frequently (e.g., every 5-10 frames).
-   **Tier 2 (Simulated):** NPCs far from the player. Despawn the Actor. Update their state via simplified math in the manager (e.g., `NPC is hungry, it is daytime, he is a farmer -> Work goal is progressing, Hunger is increasing`). Only spawn an Actor when a player gets close.

**Recommendation 3: Optimize World Queries**
- **Immediately replace all uses of `GetAllActorsOfClass`** in frequently called code.
- The AI Manager should maintain its own optimized spatial data structures (`FSpatialHash` or `FActionGrid`) or cached lists of relevant actors (e.g., `TArray<AHostileActor*>`) for NPCs to query against.

### 3.2. Data Management and Flow

**Finding 3: Monolithic World State Persistence**
- The `UWorldPersistenceSystem` currently saves and loads the entire world state at once. This will cause significant hitches and high memory usage in a large world.

**Finding 4: Suboptimal Data Layout**
- The use of `TArray` of structs (e.g., `TArray<FWorldAction>`) is not ideal for cache performance when processing large amounts of data.

**Recommendation 4: Implement Zoned Data Streaming**
- Refactor the persistence system to save/load data on a **per-region basis**.
- Only load the regions immediately surrounding the player into memory. Stream other regions in/out as the player moves.

**Recommendation 5: Use Struct-of-Arrays (SoA) for Hot Data**
- For performance-critical data that is iterated over frequently in the simulation (like the `PlayerActions` log), refactor the storage from an Array-of-Structs to a Struct-of-Arrays to improve memory access patterns and CPU cache efficiency.

**Recommendation 6: Prune Action History**
- Implement a mechanism in the `WorldPersistenceSystem` to periodically summarize and prune the `PlayerActions` log to prevent it from growing infinitely, which would impact save/load times and file size.

### 3.3. World & Interaction Systems

**Finding 5: Unmanaged Collision**
- Collision is configured on a per-asset basis in Blueprints without a unifying framework. This is a high risk for future performance issues and physics bugs.

**Recommendation 7: Define and Enforce Custom Collision Channels**
- **Define custom Object Channels** in `Config/DefaultEngine.ini` (e.g., Player, NPC, Projectile, InteractiveObject).
- **Create Collision Presets** in Project Settings based on these channels.
- **Audit key assets** (characters, weapons, props) to ensure they use the correct presets. This will provide massive performance gains by preventing unnecessary collision checks.

---

## 4. Architectural Strengths

It is important to recognize the exceptional quality of the existing foundation.
- **Decoupling:** The use of Subsystems and the `GlobalEventBus` are best-in-class examples of how to build a decoupled, maintainable architecture.
- **Modularity:** The component-based design is excellent and allows for flexible and reusable game features.
- **Clarity:** The code is clean, the naming conventions are consistent, and the project organization is logical and easy to navigate.
- **Documentation:** The project is supported by high-quality technical documentation that accurately reflects the codebase.

---

## 5. Conclusion

This project is on a path to success. The architectural foundation is sound, and the most difficult part – a clean, organized, and decoupled design – has already been achieved.

The recommended changes are not a fundamental redesign, but rather a critical **optimization pass** focused on how and when data is processed. By transitioning the AI and persistence systems from a simple, per-actor model to a centralized, managed, and streamed approach, the project will be able to fulfill its ambitious design goals and support a truly massive, dynamic, and persistent world.