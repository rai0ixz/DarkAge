# Dark Age - Technical Design Document

## 1. System Architecture Overview

Dark Age is built on Unreal Engine 5, leveraging a modular, data-driven architecture to support its complex, emergent systems. The core design philosophy is to create decoupled, scalable systems that can interact with each other through well-defined interfaces and data contracts.

The architecture is centered around a collection of **Game Instance Subsystems**, which act as the primary managers for all major gameplay domains. These subsystems are complemented by a suite of **Actor Components** that encapsulate specific behaviors and data, allowing for flexible and reusable functionality.

## 2. Core Subsystems

The following Game Instance Subsystems form the backbone of the game's architecture:

- **`UAdvancedSurvivalSubsystem`**: Manages the core survival mechanics, including player/NPC needs, disease, and environmental effects. It orchestrates data flow between various survival-related components.
- **`UNPCEcosystemSubsystem`**: The heart of the living world, this subsystem manages the lifecycle, goals, and behaviors of all non-player characters, driving emergent social and economic activity.
- **`UQuestSystem` & `UDynamicQuestSubsystem`**: A two-tiered system that handles both authored and procedurally generated quests. It integrates with other subsystems to create quests that are relevant to the current world state.
- **`UEconomySubsystem`**: Simulates a dynamic economy by managing resources, prices, and trade routes based on supply and demand.
- **`UFactionManagerSubsystem` & `UPoliticalSystem`**: These subsystems work in tandem to manage the complex web of relationships between different factions and political entities in the world.
- **`UWorldPersistenceSystem`**: Handles the saving and loading of the game state, ensuring that the world remains persistent across play sessions.
- **`UNextGenCombatSubsystem`**: Manages the core combat logic, including damage calculations, status effects, and AI combat behavior.

## 3. Component-Based Design

Actor Components are used extensively to attach specific behaviors and attributes to game entities. This approach allows for a high degree of modularity and reusability. Key components include:

- **`UStatlineComponent`**: A versatile component that manages all numerical stats for an actor, from health and stamina to hunger and thirst.
- **`UInventoryComponent`**: Handles an actor's inventory, including item management, stacking, and equipment.
- **`UCraftingComponent`**: Provides actors with the ability to craft items from recipes.
- **`UNotorietyComponent` & `UFactionReputationComponent`**: These components track an actor's reputation and standing within the game's social and political systems.
- **`UAICombatBehaviorComponent`**: Encapsulates the combat logic for AI-controlled characters, allowing for different combat styles and strategies.

## 4. Data-Driven Design

A key principle of the project is to be as data-driven as possible. This is achieved through the extensive use of:

- **Data Tables**: Used to define everything from item stats and crafting recipes to quest objectives and NPC archetypes. This allows designers to easily tweak and balance the game without modifying C++ code.
- **Gameplay Tags**: Used to identify and categorize game objects, abilities, and events in a flexible and hierarchical manner.
- **Custom Data Assets**: Used for more complex data structures that don't fit neatly into a Data Table.

## 5. Technical Goals & Challenges

- **Scalability**: The architecture must be able to support a large, persistent world with hundreds of intelligent NPCs. This requires careful attention to performance and optimization, particularly in the `UNPCEcosystemSubsystem`.
- **Modularity**: The decoupled nature of the subsystems and components is designed to make the project easy to maintain and extend over time.
- **Emergence**: The primary technical challenge is to create a set of systems that can interact in complex and unpredictable ways, leading to emergent gameplay and narrative.
- **Multiplayer**: While the initial focus is on a single-player experience, the architecture is being designed with multiplayer in mind. The `MultiplayerSocialSubsystem` and `NetworkManagerSubsystem` are the first steps in this direction.

## 6. Development & Tooling

- **Version Control**: Git is used for source control.
- **CI/CD**: A continuous integration and delivery pipeline will be set up to automate builds, testing, and deployments.
- **Debugging**: The `GameDebugManagerSubsystem` provides a suite of in-game tools for debugging and testing the various game systems.