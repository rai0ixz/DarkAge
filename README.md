<div align="center">
  <img src="https://img.icons8.com/color/96/unreal-engine.png" alt="Unreal Engine Logo" width="96"/>
  <h1>🏰 Dark Age - Advanced Medieval Survival RPG</h1>
  <p><strong>Internal Development Hub & Technical Documentation</strong></p>
  <p>
    <img src="https://img.shields.io/badge/Unreal_Engine-5.6-blue.svg?style=for-the-badge&logo=unrealengine" alt="Unreal Engine 5">
    <img src="https://img.shields.io/badge/Status-Alpha_Development-orange.svg?style=for-the-badge" alt="Status">
    <img src="https://img.shields.io/badge/Platform-Windows-informational.svg?style=for-the-badge" alt="Platform">
    <img src="https://img.shields.io/badge/Progress-69%25-green.svg?style=for-the-badge" alt="Overall Progress">
    <a href="CHANGELOG.md"><img src="https://img.shields.io/badge/Changelog-Keep_a_Changelog-yellow.svg?style=for-the-badge" alt="Changelog"></a>
  </p>
</div>

---

## 🎯 Project Overview

**Dark Age** is a complex, data-driven medieval survival RPG built on Unreal Engine 5. The game features emergent gameplay through interconnected systems including advanced survival mechanics, dynamic NPC ecosystems, political intrigue, and a living economy. The architecture is designed around modular **Game Instance Subsystems** and **Actor Components** for maximum scalability and maintainability.

### 🏗️ Core Architecture Philosophy
- **Data-Driven Design**: All gameplay values stored in [`Data Tables`](Content/_DA/Data/) and [`Data Assets`](docs/TDD.md#data-driven-design)
- **Modular Subsystems**: Decoupled systems communicating via interfaces - see [`TDD.md`](docs/TDD.md#core-subsystems)
- **Component-Based Entities**: Flexible actor composition using specialized components - see [`TDD.md`](docs/TDD.md#component-based-design)
- **Emergent Gameplay**: Systems designed to interact and create unpredictable scenarios

---

## 🚀 Recent Updates
<!-- LATEST_COMMIT_START -->
- 2025-09-29: working copy
- 2025-09-07: Enhance game systems and update GPU Dump Viewer
- 2025-09-07: o Stable build
- 2025-08-02: Update changelog and README formatting
- 2025-08-02: Improve changelog and README update processes
<!-- LATEST_COMMIT_END -->

---

##  Implementation Status & Roadmap

### 🟢 Core Systems (85% Complete)

#### Player Character System
**Status: 90% Complete** | **Files: [`DAPlayerCharacter.cpp`](Source/DarkAge/Private/BaseClass/DAPlayerCharacter.cpp)**
- ✅ **Attribute System**: [`StatlineComponent`](docs/api/StatlineComponent.md) - Full implementation
- ✅ **Character Progression**: Level-based attribute allocation
- ✅ **Equipment System**: [`InventoryComponent`](docs/api/InventoryComponent.md) integration
- ✅ **Faction Integration**: [`FactionReputationComponent`](docs/api/FactionReputationComponent.md)
- 🔄 **Political Actions**: [`DAPlayerCharacter_PoliticalActions.cpp`](Source/DarkAge/Private/BaseClass/DAPlayerCharacter_PoliticalActions.cpp) - 70% complete

#### Survival Mechanics
**Status: 90% Complete** | **Core: [`AdvancedSurvivalSubsystem`](docs/api/AdvancedSurvivalSubsystem.md)**
- ✅ **Hunger/Thirst System**: Real-time depletion and damage when stats are at zero.
- ✅ **Disease System**: [`DiseaseSystem`](docs/api/DiseaseSystem.md) with staged progression
- ✅ **Environmental Effects**: [`ClimateAdaptationComponent`](docs/api/ClimateAdaptationComponent.md)
- ✅ **Shelter Management**: [`ShelterManagementComponent`](docs/api/ShelterManagementComponent.md)
- 🔄 **Advanced Disease Interactions**: 60% complete
- 📋 **Seasonal Survival Effects**: Planned for v1.2

### 🟡 NPC & AI Systems (70% Complete)

#### NPC Ecosystem
**Status: 75% Complete** | **Core: [`NPCEcosystemSubsystem`](docs/api/NPCEcosystemSubsystem.md)**
- ✅ **NPC Archetypes**: [`DAMillerNPC`](Source/DarkAge/Private/Characters/DAMillerNPC.cpp), [`DALandownerNPC`](Source/DarkAge/Private/Characters/DALandownerNPC.cpp), [`DATavernKeeperNPC`](Source/DarkAge/Private/Characters/DATavernKeeperNPC.cpp)
- ✅ **Daily Schedules**: Time-based behavior patterns
- ✅ **Needs System**: Hunger, sleep, social needs affecting behavior
- 🔄 **Population Dynamics**: Migration and breeding systems - 40% complete
- 📋 **Advanced Social Interactions**: Planned for v1.3

#### AI Behavior
**Status: 65% Complete** | **Components: [`AICombatBehaviorComponent`](docs/api/AICombatBehaviorComponent.md)**
- ✅ **Combat AI**: Basic combat behaviors and tactics
- ✅ **Memory System**: [`AIMemoryComponent`](Source/DarkAge/Private/Components/AIMemoryComponent.cpp)
- ✅ **Morale System**: [`AIMoraleComponent`](Source/DarkAge/Private/Components/AIMoraleComponent.cpp)
- 🔄 **Advanced Planning**: [`AINeedsPlanningComponent`](Source/DarkAge/Private/Components/AINeedsPlanningComponent.cpp) - 50% complete
- 📋 **Group AI Coordination**: Planned for v1.4

### 🟡 Economy & Crafting (65% Complete)

#### Dynamic Economy
**Status: 75% Complete** | **Core: [`EconomySubsystem`](docs/api/EconomySubsystem.md)**
- ✅ **Supply/Demand Simulation**: Price fluctuation based on regional trade
- ✅ **Trade Routes**: Inter-settlement commerce
- ✅ **Vendor Integration**: NPC merchant behavior
- 🔄 **Advanced Market Events**: Economic crises, booms - 30% complete
- 📋 **Player-Driven Economy**: Large-scale player impact - Planned for v1.5

#### Crafting System
**Status: 50% Complete** | **Component: [`CraftingComponent`](docs/api/CraftingComponent.md)**
- ✅ **Basic Recipes**: Item creation from components
- ✅ **Skill Requirements**: Level-gated crafting
- ✅ **Station-Based Crafting**: Forge, workbench, etc.
- 🔄 **Quality System**: Item quality variations - 40% complete
- 📋 **Master Crafting**: Legendary item creation - Planned for v1.6

### 🟡 Political & Social Systems (55% Complete)

#### Faction System
**Status: 60% Complete** | **Core: [`FactionManagerSubsystem`](docs/api/FactionManagerSubsystem.md)**
- ✅ **Faction Reputation**: [`FactionReputationComponent`](docs/api/FactionReputationComponent.md)
- ✅ **Basic Faction Relations**: Friend/enemy dynamics
- 🔄 **Political Events**: [`PoliticalSystem`](docs/api/PoliticalSystem.md) - 45% complete
- 📋 **Faction Wars**: Large-scale conflicts - Planned for v1.7

#### Crime & Justice
**Status: 50% Complete** | **Core: [`CrimeManagerSubsystem`](docs/api/CrimeManagerSubsystem.md)**
- ✅ **Notoriety System**: [`NotorietyComponent`](docs/api/NotorietyComponent.md)
- 🔄 **Crime Detection**: Witness system - 60% complete
- 🔄 **Justice System**: Courts, punishment - 30% complete
- 📋 **Bounty System**: Player bounties - Planned for v1.8

### 🟡 Quest & Content Systems (65% Complete)

#### Quest Framework
**Status: 70% Complete** | **Core: [`QuestSystem`](docs/api/QuestSystem.md)**
- ✅ **Static Quests**: Hand-authored quest content
- ✅ **Dynamic Quest Generation**: [`DynamicQuestSubsystem`](Source/DarkAge/Private/Core/DynamicQuestSubsystem.cpp)
- ✅ **Quest Log UI**: [`DAQuestLogWidget`](docs/api/DAQuestLogWidget.md)
- 🔄 **Complex Quest Chains**: Multi-stage narratives - 50% complete
- 📋 **Emergent Storylines**: AI-driven narratives - Planned for v2.0

### 🔴 Advanced Features (25% Complete)

#### Multiplayer Foundation
**Status: 30% Complete** | **Core: [`MultiplayerSocialSubsystem`](docs/api/MultiplayerSocialSubsystem.md)**
- 🔄 **Network Architecture**: Basic client-server setup - 40% complete
- 🔄 **Social Features**: Player interactions - 20% complete
- 📋 **Persistent World**: Server-side world state - Planned for v2.1

#### Weather & Environment
**Status: 20% Complete** | **Core: [`WeatherSystem`](docs/api/WeatherSystem.md)**
- 🔄 **Basic Weather**: Rain, snow effects - 30% complete
- 📋 **Seasonal Changes**: Long-term climate cycles - Planned for v1.9
- 📋 **Weather Impact**: Survival and gameplay effects - Planned for v1.9

---

## 🗂️ Technical Documentation Library

### 📋 Core Documentation
| Document | Purpose | Completion | Last Updated |
|----------|---------|------------|--------------|
| **[📄 Game Design Document](docs/GDD.md)** | Complete gameplay mechanics & balance | 95% | Current |
| **[📄 Technical Design Document](docs/TDD.md)** | C++ architecture & system design | 90% | Current |
| **[📚 API Reference](docs/api/README.md)** | Complete C++ class documentation | 85% | Current |
| **[📜 Changelog](CHANGELOG.md)** | Project update history | 100% | Current |

### 🔧 System-Specific Documentation
| System | API Docs | Implementation | Status |
|--------|----------|----------------|---------|
| **Survival** | [`AdvancedSurvivalSubsystem`](docs/api/AdvancedSurvivalSubsystem.md) | [`AdvancedSurvivalSubsystem.cpp`](Source/DarkAge/Private/Core/AdvancedSurvivalSubsystem.cpp) | 85% ✅ |
| **NPC Ecosystem** | [`NPCEcosystemSubsystem`](docs/api/NPCEcosystemSubsystem.md) | [`NPCEcosystemSubsystem.h`](Source/DarkAge/Public/Core/NPCEcosystemSubsystem.h) | 75% 🔄 |
| **Economy** | [`EconomySubsystem`](docs/api/EconomySubsystem.md) | [`EconomySubsystem.cpp`](Source/DarkAge/Private/Core/EconomySubsystem.cpp) | 70% 🔄 |
| **Combat** | [`NextGenCombatSubsystem`](docs/api/NextGenCombatSubsystem.md) | [`NextGenCombatSubsystem.h`](Source/DarkAge/Public/Core/NextGenCombatSubsystem.h) | 60% 🔄 |
| **Quests** | [`QuestSystem`](docs/api/QuestSystem.md) | [`DynamicQuestSubsystem.cpp`](Source/DarkAge/Private/Core/DynamicQuestSubsystem.cpp) | 70% 🔄 |
| **Politics** | [`PoliticalSystem`](docs/api/PoliticalSystem.md) | [`FactionManagerSubsystem.cpp`](Source/DarkAge/Private/Core/FactionManagerSubsystem.cpp) | 55% 🔄 |

### 🎨 UI System Documentation
| Component | API Docs | Implementation | Features |
|-----------|----------|----------------|----------|
| **Main HUD** | [`ADAHUD`](docs/api/ADAHUD.md) | [`DAHUD.cpp`](Source/DarkAge/Private/UI/DAHUD.cpp) | Health, stamina, minimap |
| **Inventory** | [`UDAInventoryWidget`](docs/api/UDAInventoryWidget.md) | [`DAInventoryWidget.cpp`](Source/DarkAge/Private/UI/SubWidgets/DAInventoryWidget.cpp) | Item management, equipment |
| **Player Status** | [`UDAPlayerStatusWidget`](docs/api/UDAPlayerStatusWidget.md) | [`DAPlayerStatusWidget.cpp`](Source/DarkAge/Private/UI/SubWidgets/DAPlayerStatusWidget.cpp) | Survival stats, conditions |
| **Notifications** | [`UDANotificationWidget`](docs/api/UDANotificationWidget.md) | [`DANotificationWidget.cpp`](Source/DarkAge/Private/UI/SubWidgets/DANotificationWidget.cpp) | System messages, alerts |
| **Interactions** | [`UDAInteractionPromptWidget`](docs/api/UDAInteractionPromptWidget.md) | [`DAInteractionPromptWidget.cpp`](Source/DarkAge/Private/UI/DAInteractionPromptWidget.cpp) | World interaction prompts |

---

## 🎮 Gameplay Mechanics Deep-Dive

### ⚔️ Combat System
**Implementation: [`NextGenCombatSubsystem`](docs/api/NextGenCombatSubsystem.md) | Status: 60% Complete**

#### Damage Calculation Formula
```cpp
FinalDamage = (BaseWeaponDamage + (RelevantAttribute * WeaponModifier)) - ArmorDR
```

| Weapon Type | Base Damage | Attribute Modifier | Stamina Cost | Special Properties |
|-------------|-------------|-------------------|--------------|-------------------|
| **Swords** | 12-18 | STR × 0.5 | Light: 5, Heavy: 15 | Balanced speed/damage |
| **Axes** | 15-25 | STR × 0.7 | Light: 7, Heavy: 20 | +50% vs wooden shields |
| **Bows** | 8-15 | DEX × 0.6 | 3 per shot | Damage scales with draw time |
| **Daggers** | 6-12 | DEX × 0.4 | Light: 3, Heavy: 8 | +100% backstab damage |

#### Status Effects System
**Component: [`StatusEffectComponent`](Source/DarkAge/Private/Components/StatusEffectComponent.cpp)**
- **Bleeding**: -2 HP/sec for 30 seconds
- **Poisoned**: -1 HP/sec for 60 seconds, -10% movement speed
- **Stunned**: Cannot move or attack for 3 seconds
- **Diseased**: Various effects based on [`DiseaseSystem`](docs/api/DiseaseSystem.md)

### 🍖 Survival Mechanics
**Implementation: [`AdvancedSurvivalSubsystem`](docs/api/AdvancedSurvivalSubsystem.md) | Status: 85% Complete**

#### Needs System (Real-Time)
| Need | Depletion Rate | Critical Threshold | Death Threshold | Effects |
|------|----------------|-------------------|-----------------|---------|
| **Hunger** | -0.1 per second | <25% | 0% | Stamina regen -50% → -0.5 HP/sec |
| **Thirst** | -0.2 per second | <25% | 0% | Stamina regen -50% → -1 HP/sec |
| **Sleep** | -1 per 5 minutes | <20% | N/A | -25% XP gain, hallucinations |
| **Warmth** | Variable by climate | <30% | 0% | Movement -25% → -1 HP/15s |

#### Disease System Details
**Implementation: [`DiseaseSystem`](docs/api/DiseaseSystem.md)**

**Example: "The Shakes" Disease Progression**
```cpp
// Stage 1: Incubation (24 hours real-time)
- No visible effects
- 10% chance from consuming dirty water

// Stage 2: Symptomatic (72 hours real-time)  
- Dexterity: -5 points
- Periodic coughing animations
- 5% chance to spread to nearby NPCs

// Stage 3: Severe (48 hours real-time)
- Dexterity: -10 points, Strength: -5 points
- Stamina regeneration disabled
- 15% chance to spread to nearby NPCs

// Cure: Herbal Tincture
- Recipe: 3× Rivercress + 1× Clean Water
- Requires Alchemy skill level 5
- 95% cure rate, 5% chance of complications
```

### 🏘️ NPC Ecosystem
**Implementation: [`NPCEcosystemSubsystem`](docs/api/NPCEcosystemSubsystem.md) | Status: 75% Complete**

#### NPC Archetype: Miller
**Implementation: [`DAMillerNPC`](Source/DarkAge/Private/Characters/DAMillerNPC.cpp)**
```cpp
// Daily Schedule (Game Time)
07:00 - Wake up, check grain supplies
08:00 - Begin mill operations (if grain available)
12:00 - Lunch break, social interaction
13:00 - Resume mill operations  
17:00 - End work, walk to tavern
19:00 - Socialize at tavern (different dialogue tree)
22:00 - Return home, sleep

// Needs-Based Behavior Overrides
if (Hunger < 30) { AbandonWork(); SeekFood(); }
if (Social < 20) { SeekSocialInteraction(); }
if (Happiness < 10) { GeneratePlayerQuest("Help the Miller"); }
```

#### Dynamic Quest Generation
**Implementation: [`DynamicQuestSubsystem`](Source/DarkAge/Private/Core/DynamicQuestSubsystem.cpp)**
```cpp
// Example: Wolf Problem Quest Generation
1. NPCEcosystemSubsystem detects Landowner.Happiness < 25
2. Cause analysis: Wolf attacks on livestock (tracked via WorldEcosystemSubsystem)
3. DynamicQuestSubsystem generates "Cull Quest"
   - Title: "Wolf Troubles"
   - Objective: Kill 5 wolves in Millhaven Forest
   - Reward: 100 gold + 10 Millhaven reputation
   - Failure condition: 7 days timeout
4. Quest becomes available from Landowner NPC
```

### 💰 Economic Simulation
**Implementation: [`EconomySubsystem`](docs/api/EconomySubsystem.md) | Status: 70% Complete**

#### Supply & Demand Example
```cpp
// Scenario: Player floods market with iron ore
Initial State:
- Iron Ore price: 5 gold/unit
- Millhaven supply: 50 units
- Demand: 10 units/day

Player Action: Sells 100 Iron Ore units

Economic Response:
- New supply: 150 units
- Price adjustment: -40% (now 3 gold/unit)
- Ripple effects:
  * Iron Sword price: -15% (crafting cost reduced)
  * Neighboring town Oakstead: Iron shortage detected
  * Dynamic quest generated: "Iron Delivery to Oakstead"
  * Trade route established: Millhaven → Oakstead
```

---

## 🛠️ Development Setup & Build Instructions

### Prerequisites
- **Unreal Engine**: 5.3+ (check [`DarkAge.uproject`](DarkAge.uproject) for exact version)
- **Visual Studio**: 2022 with C++ game development workload
- **Git LFS**: For large asset management
- **Minimum RAM**: 16GB (32GB recommended for full builds)

### Quick Start
```bash
# 1. Clone repository with LFS
git clone --recursive [repository_url]
cd DarkAge

# 2. Generate project files
# Right-click DarkAge.uproject → "Generate Visual Studio project files"

# 3. Build solution
# Open DarkAge.sln in Visual Studio
# Build → Build Solution (Development Editor configuration)

# 4. Launch editor
# Double-click DarkAge.uproject
```

### Key Development Maps
| Map | Purpose | Location |
|-----|---------|----------|
| **ThirdPersonMap** | Main gameplay testing | [`Content/_DA/Maps/ThirdPersonMap.umap`](Content/_DA/Maps/ThirdPersonMap.umap) |
| **TestLevel** | System integration testing | [`Content/ThirdPerson/Lvl_ThirdPerson.umap`](Content/ThirdPerson/Lvl_ThirdPerson.umap) |

### Data Configuration
| System | Data Location | Format |
|--------|---------------|--------|
| **Items** | [`Content/_DA/Data/Items/`](Content/_DA/Data/) | Data Tables |
| **NPCs** | [`Content/_DA/Data/NPCs/`](Content/_DA/Data/) | Data Assets |
| **Quests** | [`Content/_DA/Data/Quests/`](Content/_DA/Data/) | Data Assets |
| **Recipes** | [`Content/_DA/Data/Crafting/`](Content/_DA/Data/) | Data Tables |

---

## 🔧 Development Principles & Best Practices

### 🎯 Core Design Philosophy
1. **Data-Driven First**: Gameplay values belong in Data Tables, not C++ hardcoding
2. **Modular Architecture**: Systems communicate via interfaces, avoid tight coupling
3. **Performance Critical**: Profile regularly, especially [`NPCEcosystemSubsystem`](docs/api/NPCEcosystemSubsystem.md)
4. **Emergent Design**: Systems should interact to create unpredictable gameplay

### 📝 Code Standards
```cpp
// Example: Proper subsystem interaction
void UAdvancedSurvivalSubsystem::UpdateCharacterSurvival(ACharacter* Character, float DeltaTime)
{
    // ✅ Good: Use interface for loose coupling
    if (IStatlineInterface* StatlineInterface = Cast<IStatlineInterface>(Character))
    {
        StatlineInterface->ModifyHunger(-DeltaTime * HungerDepletionRate);
    }
    
    // ❌ Bad: Direct component access creates tight coupling
    // UStatlineComponent* StatComp = Character->GetComponent<UStatlineComponent>();
}
```

### 🚀 Performance Guidelines
- **Tick Optimization**: Use `SetTickInterval()` for non-critical updates
- **Object Pooling**: Reuse objects for frequently spawned items (projectiles, effects)
- **LOD Systems**: Implement distance-based detail reduction for NPCs
- **Async Loading**: Stream world content to maintain 60+ FPS

---

## 🗺️ Upcoming Milestones

### 📅 Version 0.35 (Current Sprint) - Core Systems Polish
**Target: Q3 2025 | Progress: 80%**
- 🔄 Complete advanced disease interactions
- 🔄 Finish NPC population dynamics
- 🔄 Implement complex quest chains
- 📋 Performance optimization pass

### 📅 Version 0.4 - Environmental Systems
**Target: Q4 2025 | Progress: 15%**
- 📋 Seasonal survival effects
- 📋 Advanced weather impact on gameplay
- 📋 Climate-based NPC migration
- 📋 Environmental storytelling

### 📅 Version 0.5 - Social Complexity
**Target: Q1 2025 | Progress: 5%**
- 📋 Advanced NPC social interactions
- 📋 Relationship webs between NPCs
- 📋 Social event propagation
- 📋 Reputation consequence system

### 📅 Version 0.6 - Multiplayer Foundation
**Target: Q2 2026 | Progress: 0%**
- 📋 Persistent world server architecture
- 📋 Player-to-player interactions
- 📋 Shared economy simulation
- 📋 Emergent multiplayer storylines

---

## 📈 Performance Metrics & Targets

### Current Benchmarks (Development Build)
| Metric | Current | Target | Status |
|--------|---------|--------|--------|
| **Frame Rate** | 45-60 FPS | 60+ FPS | 🔄 Optimizing |
| **Memory Usage** | 8.2 GB | <6 GB | 🔄 Optimizing |
| **NPC Count** | 150 active | 300+ active | 🔄 Scaling |
| **World Size** | 4km² | 16km² | 📋 Planned |
| **Load Times** | 45 seconds | <30 seconds | 🔄 Optimizing |

### Critical Performance Areas
1. **[`NPCEcosystemSubsystem`](docs/api/NPCEcosystemSubsystem.md)**: Primary bottleneck for large NPC populations
2. **[`WorldEcosystemSubsystem`](docs/api/WorldEcosystemSubsystem.md)**: Environmental simulation overhead
3. **UI Rendering**: Complex HUD elements during gameplay
4. **Asset Streaming**: Large world content loading

---

<div align="center">
  <h2>🎮 Ready to Dive Deeper?</h2>
  <p>
    <strong>📖 Start with the <a href="docs/GDD.md">Game Design Document</a> for complete gameplay mechanics</strong><br>
    <strong>🔧 Review the <a href="docs/TDD.md">Technical Design Document</a> for architecture details</strong><br>
    <strong>📚 Browse the <a href="docs/api/README.md">API Documentation</a> for implementation specifics</strong>
  </p>
  
  <p><em>This README serves as your central hub - all systems, percentages, and links are kept current with active development.</em></p>
</div>
