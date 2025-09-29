Dark Age - Advanced Medieval Survival RPG 🏰
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

🎯 Project Overview
Dark Age is a complex, data-driven medieval survival RPG built on Unreal Engine 5. The game features emergent gameplay through interconnected systems including advanced survival mechanics, dynamic NPC ecosystems, political intrigue, and a living economy. The architecture is designed around modular Game Instance Subsystems and Actor Components for maximum scalability and maintainability.

🏗️ Core Architecture Philosophy
Data-Driven Design: All gameplay values stored in Data Tables and Data Assets.

Modular Subsystems: Decoupled systems communicating via interfaces.

Component-Based Entities: Flexible actor composition using specialized components.

Emergent Gameplay: Systems designed to interact and create unpredictable scenarios.

🚀 Recent Updates
2025-09-29: working copy

2025-09-07: Enhance game systems and update GPU Dump Viewer

2025-09-07: o Stable build

2025-08-02: Update changelog and README formatting

2025-08-02: Improve changelog and README update processes

Key Features
Gameplay Mechanics
Unforgiving Realism: Every system is designed to be plausible and challenging. Starvation is a real threat, a broken leg is a crisis, and winter can be a death sentence. There are no magical shortcuts.

Dynamic, Living World: The world of Aerthos breathes. NPCs have schedules, needs, and goals. Factions wage war, economies react to supply and demand, and ecosystems respond to player actions. The world does not revolve around the player, but it will remember them.

Deep Systemic Emergence: The game doesn't write stories; it builds systems that create them. A bandit attack on a merchant caravan isn't a scripted event; it's the result of a bandit camp's desperation, a caravan's predictable route, and the AI Director recognizing an opportunity.

Meaningful Progression: Power is not measured in levels alone. True advancement is building a fortified home, mastering the art of steel-making, establishing a lucrative trade route, or earning the trust of a powerful faction. It's about conquering the environment, not just its inhabitants.

Technical Features
Advanced Survival Simulation: A hostile world with a granular needs system (hunger, thirst, sleep, temperature), a detailed health and injury system, and a comprehensive disease system.

Brutal & Deliberate Combat: A stamina-based, weighty, and deadly combat system that is a puzzle of positioning, timing, and resource management.

Extensive Crafting & Construction: A deep crafting system with tiered resources, item quality, and a research-based discovery process, coupled with a modular, structural-integrity-based construction system.

Dynamic Economic Simulation: Regional markets with fluctuating prices based on supply and demand, affected by world events and player actions.

Sophisticated AI & NPC Simulation: An "AI Director" that generates dynamic events and quests, and a "Day in the Life" system for NPCs with schedules, jobs, needs, and relationships.

🛠️ Development Setup & Build Instructions
Prerequisites
Unreal Engine: 5.3+ (check DarkAge.uproject for exact version)

Visual Studio: 2022 with C++ game development workload

Git LFS: For large asset management

Minimum RAM: 16GB (32GB recommended for full builds)

Quick Start
Bash

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
🗂️ Project Structure
The project is organized into the following main directories:

Source/: Contains all the C++ source code for the game, divided into Public and Private directories.

Core/: Core gameplay systems and managers.

Components/: Reusable ActorComponents for various game features.

AI/: Artificial intelligence, including behavior trees and tasks.

Characters/: Base classes and specific implementations of player and NPC characters.

Items/: The base item class and its various child classes.

UI/: The user interface, including HUD, widgets, and menus.

World/: Classes related to the game world, such as resource nodes.

Content/: All game assets, including Blueprints, models, textures, and sounds.

_DA/: The main folder for all Dark Age-specific assets.

Blueprints/: All Blueprints, organized by type (AI, Core, Items, Player, etc.).

Data/: Data tables for items, crafting recipes, quests, and more.

Maps/: The different levels and maps of the game.

UI/: All UI-related assets, including widget Blueprints.

Config/: Configuration files for the game engine and game-specific settings.

docs/: All documentation, including the Game Design Document (GDD) and Technical Design Document (TDD).

💻 Technical Deep Dive
Core Subsystems
The game's architecture is built around a collection of Game Instance Subsystems, which are the primary managers for all major gameplay domains. This design promotes a decoupled, scalable, and data-driven approach.

UAdvancedSurvivalSubsystem: Manages the core survival mechanics, including player/NPC needs, disease, and environmental effects. It orchestrates data flow between various survival-related components.

UNPCEcosystemSubsystem: The heart of the living world, this subsystem manages the lifecycle, goals, and behaviors of all non-player characters, driving emergent social and economic activity.

UQuestSystem & UDynamicQuestSubsystem: A two-tiered system that handles both authored and procedurally generated quests. It integrates with other subsystems to create quests that are relevant to the current world state.

UEconomySubsystem: Simulates a dynamic economy by managing resources, prices, and trade routes based on supply and demand.

UFactionManagerSubsystem & UPoliticalSystem: These subsystems work in tandem to manage the complex web of relationships between different factions and political entities in the world.

UWorldPersistenceSystem: Handles the saving and loading of the game state, ensuring that the world remains persistent across play sessions.

UNextGenCombatSubsystem: Manages the core combat logic, including damage calculations, status effects, and AI combat behavior.

Component-Based Design
Actor Components are used extensively to attach specific behaviors and attributes to game entities. This approach allows for a high degree of modularity and reusability. Key components include:

UStatlineComponent: A versatile component that manages all numerical stats for an actor, from health and stamina to hunger and thirst.

UInventoryComponent: Handles an actor's inventory, including item management, stacking, and equipment.

UCraftingComponent: Provides actors with the ability to craft items from recipes.

UNotorietyComponent & UFactionReputationComponent: These components track an actor's reputation and standing within the game's social and political systems.

UAICombatBehaviorComponent: Encapsulates the combat logic for AI-controlled characters, allowing for different combat styles and strategies.

🗺️ Roadmap
The project is currently in the Alpha Development stage, with the core systems being polished. The following are the upcoming milestones:

Version 0.35 (Current Sprint) - Core Systems Polish

Version 0.4 - Environmental Systems

Version 0.5 - Social Complexity

Version 0.6 - Multiplayer Foundation

🤝 Contributing
This project is in active development. Please refer to the Contribution Guide for details on how to contribute to the project.

📝 License
This project is proprietary and all rights are reserved.

<div align="center">
<h2>🎮 Ready to Dive Deeper?</h2>
<p>
<strong>📖 Start with the <a href="docs/GDD.md">Game Design Document</a> for complete gameplay mechanics</strong>



<strong>🔧 Review the <a href="docs/TDD.md">Technical Design Document</a> for architecture details</strong>



<strong>📚 Browse the <a href="docs/api/README.md">API Documentation</a> for implementation specifics</strong>
</p>

<p><em>This README serves as your central hub - all systems, percentages, and links are kept current with active development.</em></p>
</div>


nice now make a low level GDD and TDD




Show thinking
Of course, here are the low-level GDD and TDD for Dark Age.

Dark Age - Low-Level Game Design Document (GDD)
1. Core Vision & Philosophy
Elevator Pitch: Dark Age is an unforgiving, hyper-realistic medieval survival RPG where you are not a prophesied hero but a common soul fighting for a foothold in a collapsing world. It blends the deep crafting, base-building, and brutal survival of games like Valheim and The Long Dark with the complex, player-driven systemic depth and emergent narratives of Kenshi and Project Zomboid.

Core Pillars:

Unforgiving Realism: Every system is designed to be plausible and challenging. Starvation is a real threat, a broken leg is a crisis, and winter can be a death sentence. There are no magical shortcuts.

Dynamic, Living World: The world of Aerthos breathes. NPCs have schedules, needs, and goals. Factions wage war, economies react to supply and demand, and ecosystems respond to player actions. The world does not revolve around the player, but it will remember them.

Deep Systemic Emergence: We don't write stories; we build systems that create them. A bandit attack on a merchant caravan isn't a scripted event; it's the result of a bandit camp's desperation, a caravan's predictable route, and the AI Director recognizing an opportunity.

Meaningful Progression: Power is not measured in levels alone. True advancement is building a fortified home, mastering the art of steel-making, establishing a lucrative trade route, or earning the trust of a powerful faction. It's about conquering the environment, not just its inhabitants.

Target Audience: Ages 25-45, fans of hardcore survival, deep CRPGs, and emergent gameplay sandboxes.

Player Fantasy: The core fantasy is one of true grit and earned achievement.

2. The World of Aerthos
History: Once the jewel of the Valorian Empire, the world has fallen into a dark age after the "Crimson Fading," a plague that dissolved society. It has been 127 years since the fall.

State of the World:

Technology: Much has been lost. Advanced metallurgy and literacy are rare.

Law & Order: Justice is local and often brutal.

Supernatural: No overt magic, but the world is filled with superstition.

World Regions:

The Heartlands (Temperate Forest): The former breadbasket of the Empire, now overgrown and dangerous.

The Iron Peaks (Alpine/Taiga): Cold mountains, home to insular mining clans.

The Mirefen (Swamp/Marsh): A sprawling wetland with pervasive disease and other hazards.

The Sunken Coast (Coastal/Maritime): A rugged coastline with unpredictable weather and pirates.

Flora & Fauna: Each biome has a unique ecosystem. Over-harvesting or over-hunting will have a noticeable impact on the local environment.

3. The Factions of Aerthos
The Merchant Guild: Amoral, pragmatic, and ruthlessly efficient capitalists.

The Remnant Legion: Authoritarian, disciplined, and deeply suspicious of outsiders.

The Circle of Hands: A secretive order preserving lost Valorian knowledge.

Minor Factions: The Freeholders, The Iron Clans, The Mire-touched, and various Bandit-Kings.

4. The Player Character
Character Creation: The player begins as a nobody with a single "Background" choice that provides minor starting skill bonuses.

Core Attributes: Strength (STR), Dexterity (DEX), Constitution (CON), Intelligence (INT), Perception (PER), and Charisma (CHA).

Derived Statistics: Health (HP), Stamina (STA), Carry Capacity, Disease Resist, and Armor Rating.

5. Skill & Progression Systems
Philosophy: "Learning Through Doing." Skills improve with use.

Skill Categories: Combat (Blades, Bludgeoning, Archery), Crafting (Smithing, Carpentry, Alchemy), Survival (Hunting, Foraging, Cooking), and Social (Persuasion, Barter).

Experience & Leveling: XP is gained through actions like crafting, combat, and discovery. Leveling up provides attribute points and perk points.

6. Core Gameplay Loop
The First Hour: The player awakens with no memory and must learn basic survival skills: crafting a crude axe, finding water, hunting for food, and building a fire.

The First 10 Hours: The player establishes a small base, crafts basic gear, and encounters their first real dangers.

The First Season (Mid-Game): The player has a fortified base, is engaging with a faction, and is exploring more dangerous areas of the world.

The First Year (End-Game): The player has become a significant local power, influencing factions and undertaking large-scale projects.

7. The Survival Simulation
Needs System: Hunger, Thirst, Sleep, and Temperature are all tracked and have significant gameplay effects.

Health & Injury System: Health does not regenerate automatically. Serious injuries can result in long-lasting debuffs.

8. The Combat System
Philosophy: Combat is stamina-based, weighty, and deadly. Button-mashing is a death sentence.

Damage Calculation: FinalDamage = (BaseWeaponDamage * (1 + (RelevantAttribute * 0.05))) * BodyPartModifier * (1 - (TargetArmorDR * (1 - WeaponArmorPen)))

Stealth System: Based on light, sound, and movement speed. Successful stealth attacks from behind deal massive bonus damage.

9. Crafting & Construction
Gathering & Resources: Resources are tiered, with higher tiers found in more dangerous areas.

Crafting: Performed at upgradeable crafting stations. A "Breakthrough" system requires players to experiment with materials to discover new recipes.

Construction: A modular, socket-based system with structural integrity.

10. The Economic Simulation
Regional Markets: Each major town has its own simulated market with fluctuating prices.

Trade Routes: NPC caravans physically travel between towns and can be raided, which affects supply and demand.

11. AI & NPC Simulation
The AI Director: A "storyteller" AI that generates dynamic events and quests.

The 'Day in the Life' System: NPCs have schedules, jobs, homes, needs, and relationships that are simulated in real-time.

12. Quest & Narrative System
Philosophy: System-driven narrative. Quests are generated organically based on the state of the world.

Dynamic Quest Generator: The AI Director generates quests based on templates like "Cull," "Rescue," "Procurement," and "Caravan Guard."

Dialogue & Rumor System: A keyword-based system that allows players to ask NPCs about rumors they have heard.

13. UI/UX Design
Core Principles: Immersive and diegetic, with a minimal HUD.

Screen Breakdowns: A grid-based inventory, a hand-drawn map that fills in as the player explores, and a journal that automatically logs conversations and quest objectives.

Dark Age - Low-Level Technical Design Document (TDD)
1. System Architecture Overview
Dark Age is built on Unreal Engine 5, leveraging a modular, data-driven architecture to support its complex, emergent systems. The core design philosophy is to create decoupled, scalable systems that can interact with each other through well-defined interfaces and data contracts. The architecture is centered around a collection of Game Instance Subsystems, which act as the primary managers for all major gameplay domains. These subsystems are complemented by a suite of Actor Components that encapsulate specific behaviors and data, allowing for flexible and reusable functionality.

2. Core Subsystems
The following Game Instance Subsystems form the backbone of the game's architecture:

UAdvancedSurvivalSubsystem: Manages the core survival mechanics, including player/NPC needs, disease, and environmental effects. It orchestrates data flow between various survival-related components.

UNPCEcosystemSubsystem: The heart of the living world, this subsystem manages the lifecycle, goals, and behaviors of all non-player characters, driving emergent social and economic activity.

UQuestSystem & UDynamicQuestSubsystem: A two-tiered system that handles both authored and procedurally generated quests. It integrates with other subsystems to create quests that are relevant to the current world state.

UEconomySubsystem: Simulates a dynamic economy by managing resources, prices, and trade routes based on supply and demand.

UFactionManagerSubsystem & UPoliticalSystem: These subsystems work in tandem to manage the complex web of relationships between different factions and political entities in the world.

UWorldPersistenceSystem: Handles the saving and loading of the game state, ensuring that the world remains persistent across play sessions.

UNextGenCombatSubsystem: Manages the core combat logic, including damage calculations, status effects, and AI combat behavior.

3. Component-Based Design
Actor Components are used extensively to attach specific behaviors and attributes to game entities. This approach allows for a high degree of modularity and reusability. Key components include:

UStatlineComponent: A versatile component that manages all numerical stats for an actor, from health and stamina to hunger and thirst.

UInventoryComponent: Handles an actor's inventory, including item management, stacking, and equipment.

UCraftingComponent: Provides actors with the ability to craft items from recipes.

UNotorietyComponent & UFactionReputationComponent: These components track an actor's reputation and standing within the game's social and political systems.

UAICombatBehaviorComponent: Encapsulates the combat logic for AI-controlled characters, allowing for different combat styles and strategies.

4. Data-Driven Design
A key principle of the project is to be as data-driven as possible. This is achieved through the extensive use of:

Data Tables: Used to define everything from item stats and crafting recipes to quest objectives and NPC archetypes.

Gameplay Tags: Used to identify and categorize game objects, abilities, and events in a flexible and hierarchical manner.

Custom Data Assets: Used for more complex data structures that don't fit neatly into a Data Table.

5. Technical Goals & Challenges
Scalability: The architecture must be able to support a large, persistent world with hundreds of intelligent NPCs. This requires careful attention to performance and optimization, particularly in the UNPCEcosystemSubsystem.

Modularity: The decoupled nature of the subsystems and components is designed to make the project easy to maintain and extend over time.

Emergence: The primary technical challenge is to create a set of systems that can interact in complex and unpredictable ways, leading to emergent gameplay and narrative.

Multiplayer: While the initial focus is on a single-player experience, the architecture is being designed with multiplayer in mind. The MultiplayerSocialSubsystem and NetworkManagerSubsystem are the first steps in this direction.



the GDD needs to be atleast 10 pages long if not hundreds of pages




Show thinking
Of course. Here is a comprehensive, low-level Game Design Document that greatly expands upon the provided materials.

🏰 Dark Age - The Game Bible 🏰
Document Version: 6.0 (Living Document)
Last Updated: September 29, 2025
Authored By: Kilo Code & Gemini Studios

"The world doesn't need another hero. It needs a survivor. It needs a master of its systems, a ghost in its political machine, a wolf in its economic forests. It needs you."

<div align="center">
<img src="https://img.icons8.com/color/192/unreal-engine.png" alt="Unreal Engine Logo"/>
<h2>A Hyper-Realistic Medieval Survival RPG</h2>
<p><em>Forged in Unreal Engine 5</em></p>
</div>

Table of Contents
1. Core Vision & Philosophy

1.1. The Elevator Pitch

1.2. The Core Pillars

1.3. Target Audience & Market Positioning

1.4. The Player Fantasy

2. The World of Aerthos: A Land in Twilight

2.1. The Grand History

2.2. The State of the World

2.3. Thematic Tone & Atmosphere

2.4. Detailed World Regions

2.5. Flora & Fauna

3. The Factions of Aerthos

3.1. Major Factions

3.2. Minor Factions & Groups

4. The Player Character

4.1. Character Creation: The Blank Slate

4.2. Core Attributes & Derived Stats

5. The Skill & Progression Systems

5.1. Philosophy: Learning Through Doing

5.2. Skill Categories & Trees

5.3. Experience & Leveling

6. The Core Gameplay Loop: Survive, Thrive, Influence

6.1. The First Hour: "Awakening"

6.2. The First 10 Hours: "Shelter"

6.3. The First Season (Mid-Game): "Prosperity"

6.4. The First Year (End-Game): "Power"

7. The Survival Simulation: A Hostile World

7.1. The Needs System

7.2. The Health & Injury System

8. The Combat System: Brutal & Deliberate

8.1. Combat Philosophy

8.2. Damage Calculation Formula

8.3. Stealth System

9. The Crafting & Construction Systems: Rebuilding a World

9.1. Gathering & Resources

9.2. Crafting

9.3. Construction (Base Building)

10. The Economic Simulation: The Flow of Coin

10.1. Regional Markets

10.2. Trade Routes

11. The AI & NPC Simulation: A World of Its Own

11.1. The AI Director

11.2. The 'Day in the Life' System

12. The Quest & Narrative System: Emergent Stories

12.1. Philosophy: System-Driven Narrative

12.2. Dynamic Quest Generator

12.3. Dialogue & Rumor System

13. UI/UX Design: An Invisible Hand

13.1. Core Principles

13.2. Detailed Screen Breakdowns

14. Game Balance & Data

14.1. The First 10 Hours

14.2. Mid-Game & End-Game Loops

15. Art & Audio Direction

15.1. Visual Style

15.2. Sound Design Philosophy

1. Core Vision & Philosophy
1.1. The Elevator Pitch
Dark Age is an unforgiving, hyper-realistic medieval survival RPG where you are not a prophesied hero but a common soul fighting for a foothold in a collapsing world. It blends the deep crafting, base-building, and brutal survival of games like Valheim and The Long Dark with the complex, player-driven systemic depth and emergent narratives of Kenshi and Project Zomboid.

1.2. The Core Pillars
Unforgiving Realism: Every system is designed to be plausible and challenging. Starvation is a real threat, a broken leg is a crisis, and winter can be a death sentence. There are no magical shortcuts.

Dynamic, Living World: The world of Aerthos breathes. NPCs have schedules, needs, and goals. Factions wage war, economies react to supply and demand, and ecosystems respond to player actions. The world does not revolve around the player, but it will remember them.

Deep Systemic Emergence: We don't write stories; we build systems that create them. A bandit attack on a merchant caravan isn't a scripted event; it's the result of a bandit camp's desperation, a caravan's predictable route, and the AI Director recognizing an opportunity.

Meaningful Progression: Power is not measured in levels alone. True advancement is building a fortified home, mastering the art of steel-making, establishing a lucrative trade route, or earning the trust of a powerful faction. It's about conquering the environment, not just its inhabitants.

1.3. Target Audience & Market Positioning
Primary Demographic: Ages 25-45, fans of hardcore survival, deep CRPGs, and emergent gameplay sandboxes. They crave challenge, complexity, and the freedom to forge their own path. They are patient, methodical, and find satisfaction in overcoming difficult obstacles through planning and skill.

Psychographic Profile: "The Homesteader," "The Tactician," "The Explorer."

Market Position: A premium, B2P title on PC (Steam, GOG). It sits at the intersection of Survival, RPG, and Simulation, aiming to be the most detailed and systemic medieval survival game on the market.

1.4. The Player Fantasy
The core fantasy is one of true grit and earned achievement. It is the fantasy of rising from nothing by one's own wits and will.

You are not "The Chosen One." You are a nobody.

You are not handed a destiny. You must forge one.

You are not all-powerful. You are vulnerable, and your greatest weapon is your mind.

Your victories are not pre-ordained. They are hard-won and deeply satisfying.

2. The World of Aerthos: A Land in Twilight
2.1. The Grand History
Aerthos was once the jewel of the Valorian Empire, an empire built on reason, stone, and steel. The Valorians were master engineers and administrators, not sorcerers. They built grand aqueducts, paved roads that connected every corner of the continent, and established a rule of law that brought an unprecedented era of peace and prosperity.

Then came the Crimson Fading, a plague that defied all reason. It didn't just kill; it dissolved the very bonds of society. Within a generation, the Empire was a corpse. The great cities fell silent, the roads became the domain of bandits, and the intricate web of trade and governance unraveled.

It has now been 127 years since the fall. The world is a patchwork of isolated settlements, each struggling to hold back the encroaching wilderness and the darkness of human nature.

2.2. The State of the World
Technology & Knowledge: Much has been lost. The formula for Valorian concrete is a mystery. Advanced metallurgy is a forgotten art. Literacy is rare outside of monasteries and the halls of the Merchant Guild.

Law & Order: Imperial Law is a memory. Justice is local, swift, and often brutal. "Might makes right" is the prevailing ethos in many regions.

The Supernatural: There is no overt magic. However, the world is filled with superstition. Strange plants with unusual properties, ancient ruins that hum with a forgotten energy, and folk tales of monstrous beasts that lurk in the deep woods. Whether these are truly magical or simply phenomena not yet understood is for the player to discover.

2.3. Thematic Tone & Atmosphere
Visuals: Muted, desaturated color palettes. A "dirty medieval" aesthetic. Light is a precious resource, making torches and lanterns essential. The world should feel vast, ancient, and indifferent.

Audio: Ambient, minimalist soundscapes. The howl of wind, the distant cry of a wolf, the creak of leather. Music is sparse, used to punctuate moments of tension, discovery, or rare tranquility.

Core Mood: A constant, low-level tension. A sense of melancholy for a world that was lost, and a fragile hope for one that might be rebuilt.

2.4. Detailed World Regions
The Heartlands (Temperate Forest):

Description: The former breadbasket of the Empire. Now, crumbling highways cut through overgrown forests and fields have been reclaimed by nature. Dotted with small, walled farming villages and the ruins of great villas.

Resources: Abundant wood, stone, fertile soil, common game animals (deer, boar).

Hazards: Bandit patrols, wolf packs, rival settlements.

The Iron Peaks (Alpine/Taiga):

Description: Jagged mountains that scrape the sky. Bitterly cold. Home to hardy, insular mining clans and the skeletons of failed Imperial mining operations.

Resources: All tiers of ore (copper, tin, iron, coal), rare mountain herbs, goats.

Hazards: Extreme cold, blizzards, avalanches, dangerous predators (bears, sabrecats).

The Mirefen (Swamp/Marsh):

Description: A sprawling, humid wetland where the great rivers of the continent meet the sea. Ancient, moss-draped trees, murky waters, and the buzzing of insects are ever-present.

Resources: Unique plants for alchemy, rare fish and amphibians, bog iron.

Hazards: Pervasive disease, quicksand, venomous creatures (giant snakes, spiders), disorientation.

The Sunken Coast (Coastal/Maritime):

Description: A rugged coastline of towering cliffs, windswept beaches, and salt-scarred fishing villages. The sea is both a source of life and a constant threat.

Resources: Salt, fish, seabird eggs, salvageable wreckage.

Hazards: Sudden storms, pirate raids, treacherous tides.

2.5. Flora & Fauna
Each biome has a unique set of plants and animals that form a simulated ecosystem.

Flora (Example: Rivercress): Found near rivers. A key ingredient in healing salves. Over-harvesting will make it scarce in that area for a season.

Fauna (Example: Deer): Grazes in meadows. Hunted for meat, hide, and sinew. If their predator (wolves) is eliminated from a region, the deer population will boom, potentially damaging forests and attracting more predators.

3. The Factions of Aerthos
3.1. Major Factions
The Merchant Guild
Ideology: "Progress through Prosperity." Amoral, pragmatic, and ruthlessly efficient capitalists.

Hierarchy: Led by the unseen 'Coinmasters.' Regional chapters run by 'Factors'.

Key NPCs:

Factor Tiber: Runs the Millhaven chapter. A portly, pragmatic man who is obsessed with logistics. He is the player's first likely contact.

Silas "The Ledger" Vorn: The Guild's ruthless enforcer in the Heartlands. Manages bounty contracts and "hostile asset denial."

Inter-Faction Relationships:

Remnant Legion: A necessary nuisance. The Legion's patrols make trade safer, but their tolls and taxes are a constant source of friction.

Circle of Hands: Viewed with suspicion. The Circle hoards knowledge that could be profitable, refusing to share. The Guild actively seeks to acquire their secrets.

Player Interaction & Quest Lines:

Early Game: Escort quests for caravans moving between settlements. Resource procurement contracts (e.g., "Acquire 100 Iron Ore for the Millhaven chapter.").

Mid Game: Investment opportunities ("Buy into the Millhaven lumber mill for a share of the profits."). Sabotage quests against rival, non-Guild merchants. Establishing new, high-risk, high-reward trade routes.

Late Game: Market manipulation schemes. Funding a proxy war between two baronies to control the victor. Gaining a seat on a regional Factor's council.

Reputation Tiers & Unlocks:

Neutral (0): Standard prices.

Respected (+25): 5% discount on bulk goods.

Valued (+50): Access to the Guild's private caravan network for fast travel (for a fee). Unlocks investment opportunities.

Partner (+75): Access to exotic goods from distant lands. Can take out loans from the Guild.

Hated (-50): Refusal to trade, shops are closed to player.

Marked (-75): A permanent bounty is placed on the player's head, and assassins will be periodically dispatched.

The Remnant Legion
Ideology: "Order Above All." Authoritarian, disciplined, and deeply suspicious of outsiders.

Hierarchy: Rigid military structure led by a 'Legate' and his 'Centurions.'

Key NPCs:

Centurion Valerius: Commands the garrison at the ruined Fort Valus. A grim, honor-bound soldier who embodies the Legion's ideals.

Decanus Lyra: A younger, more pragmatic officer who sees that the Legion's rigid doctrines are failing them. A potential agent for change or mutiny.

Inter-Faction Relationships:

Merchant Guild: Corrupt, but useful. They provide supplies the Legion cannot produce itself.

Freeholders: Obstacles to be assimilated or crushed. Their independence is a challenge to the Legion's authority.

Player Interaction & Quest Lines:

Early Game: Clearing out bandit nests that threaten Legion patrols. Retrieving lost Imperial standards from ruins.

Mid Game: Sieges on fortified bandit strongholds. "Pacification" of restless villages that refuse Legion rule. Investigating and eliminating perceived threats to the Legion (which may be other factions).

Late Game: Leading a Legionary squad. Participating in a full-scale campaign to "reclaim" a region for the Empire. Uncovering and quashing a mutiny within the Legion ranks.

Reputation Tiers & Unlocks:

Auxiliary (+25): Can be hired as a mercenary for Legion operations.

Trusted (+50): Free access to Legion fortresses, quartermaster access.

Oathbound (+75): Given command of a small squad, can call for backup.

Outlaw (-50): Arrest-on-sight warrant.

Enemy of the Empire (-75): Hunted by Legionary death squads.

The Circle of Hands
Ideology: "Knowledge is the Only True Power." Secretive order preserving lost Valorian knowledge.

Hierarchy: Decentralized cells led by 'Masters' of specific crafts.

Key NPCs:

Master Elara: The Master Alchemist of the Heartlands chapter, hidden as a simple village herbalist. She is the keeper of the Circle's lore in the region.

Thorn: A silent, deadly guardian of the Circle's hidden libraries. Acts as their field agent and troubleshooter.

Inter-Faction Relationships:

All other factions: Viewed as noisy children squabbling over a broken toy. The Circle sees them as short-sighted and dangerous to the preservation of knowledge.

Player Interaction & Quest Lines:

Discovery: Must demonstrate intellect (e.g., independently discover a formula) to receive an invitation.

Mid Game: Recovering lost schematics, intellectual sabotage against other factions.

Late Game: Rebuilding major Valorian technology, protecting the Grand Archive.

Reputation Tiers & Unlocks:

Initiate (+25): Access to a hidden Circle workshop.

Adept (+50): Taught exclusive, high-tier crafting recipes.

Master (+75): Entrusted with a key to the Grand Archive, can direct research.

Ignoramus (-50): Circle spreads hostile rumors about the player.

Threat to Knowledge (-75): Circle funds player's enemies with unique poisons and traps.

3.2. Minor Factions & Groups (Expanded)
The Freeholders: Organized into local militias. Their leaders are typically village elders or respected farmers. They can be a powerful ally if you support their independence.

The Iron Clans: Each mountain has its own clan with its own traditions and allegiances. The Black-Tusk clan might be hostile, while the Stone-Beard clan might be open to trade.

The Mire-touched: Not a unified faction, but a collection of isolated, highly territorial communities. Their expertise in poison makes them valuable but incredibly dangerous to approach.

Bandit-Kings: Each has a unique "brand" of banditry. The 'Red Wolves' are savage berserkers, while the 'Gilded Crows' are more like a medieval mafia, extorting "protection" money.

4. The Player Character
4.1. Character Creation: The Blank Slate
The player begins as a nobody, a refugee with nothing but the clothes on their back. Creation focuses on appearance and a single 'Background' choice.

Backgrounds: Farmer (+5 Cooking, +5 Construction), Poacher (+5 Archery, +5 Hunting), Scribe (+5 Alchemy, +5 Research Chance), Blacksmith's Apprentice (+5 Smithing, +5 Bludgeoning), etc.

4.2. Core Attributes & Derived Stats
Attribute (1-20)	Governs	Low-Level Gameplay Effect
Strength (STR)	Melee, Labor	MeleeBonusDmg = STR * 1.5. CarryCapacity = 50 + (STR * 10). StaminaCostReduction (Melee/Labor) = STR * 1%.
Dexterity (DEX)	Ranged, Stealth, Speed	RangedBonusDmg = DEX * 1.2. AttackSpeed = BaseSpeed * (1 + DEX * 0.02). NoiseRadius = BaseNoise * (1 - DEX * 0.03).
Constitution (CON)	Health, Resistance	MaxHealth = 100 + (CON * 15). Disease/PoisonResistChance = CON * 2%. StaminaRegenDelayReduction = CON * 0.05s.
Intelligence (INT)	Crafting, Knowledge	CraftingTimeReduction = INT * 1.5%. XPGainBonus = INT * 1%. ItemDurabilityBonus (Crafted) = INT * 2%.
Perception (PER)	Awareness, Ranged Accuracy	DetectionRadius (Enemies/Items) = Base * (1 + PER * 0.05). RangedAccuracyBonus (Reduced Sway) = PER * 2%.
Charisma (CHA)	Social, Trade	BarterPriceModifier = CHA * 2%. InitialNPCDisposition = (CHA - 5) * 5. QuestRewardBonus (Non-Item) = CHA * 1%.
Table data from the Game Design Document

4.3. Derived Statistics: The Numbers of Life
Statistic	Base Value	Formula	Notes
Health (HP)	100	100 + (CON * 15)	Does not regenerate naturally. Requires food, bandages, or sleep.
Stamina (STA)	100	100 + ((STR + CON) * 0.5 * 5)	Regenerates quickly outside of combat. Depletes from all major actions.
Carry Capacity	50 kg	50 + (STR * 10)	Exceeding this value incurs severe movement penalties.
Disease Resist	0%	CON * 2%	Chance to resist contracting a disease from a given source.
Armor Rating	0	Sum of all equipped armor pieces	Provides damage reduction. See Section 8.
Table data from the Game Design Document

5. The Skill & Progression Systems
5.1. Philosophy: Learning Through Doing
Improvement comes from action. Using a sword increases the "Blades" skill. This creates organic progression tied directly to player activity.

5.2. Skill Categories & Trees (Example: Smithing)
Lvl	Perk Point	Choice A	Choice B
10	1	Sturdy Smith: Crafted tools have 25% more durability.	Efficient Smith: 10% chance to recover some charcoal/ore after crafting.
20	2	Armor Specialist: You can improve armor 10% more effectively.	Weapon Specialist: You can improve weapons 10% more effectively.
30	3	Master of the Forge: Can improve items to 'Masterwork' quality.	Resourceful Smith: 15% chance to use no ingots when crafting.
40	4	Folded Steel: 'Masterwork' weapons gain a small armor penetration bonus.	Perfect Temper: 'Masterwork' armor gains a small durability bonus.
50	5	Legendary Smith: Unlock the ability to craft unique, named items from rare materials and schematics.	Economist of War: Halves the material cost of repairing weapons and armor you have crafted yourself.
Table data from the Game Design Document

5.3. Experience & Leveling: The Slow Ascent (XP Formulas)
XP_Craft = BaseItemValue * (1 + (INT * 0.01))

XP_Kill = BaseCreatureXP * (1 + (PlayerLevel / CreatureLevel) * -0.1) (Less XP for killing weaker creatures)

XP_Discover = BaseLocationValue * (1 + (PER * 0.01))

6. The Core Gameplay Loop: Narrative Walkthrough
6.1. The First Hour: "Awakening"
The player awakens with no memory on the edge of the Mirefen, having washed ashore from a shipwreck.

0-15 Mins: Tutorialized objectives. Find a sharp stone (basic tool). Cut vines from a tree (crafting material). Combine them to craft a Crude Stone Axe.

15-30 Mins: Needs become critical. The UI highlights the thirst meter. Objective: Find fresh water. The player finds a stagnant pool (risk disease) or a running stream (safer).

30-45 Mins: Hunger is now critical. Objective: Find food. The player can forage for berries (some are poisonous, requiring Perception to identify) or use their axe to kill a small creature like a rabbit.

45-60 Mins: Night approaches. Objective: Build a fire. The player uses their axe to gather wood, then crafts a Basic Fire Kit. A successful fire provides warmth, light, and the ability to cook the rabbit meat, neutralizing its disease chance and making it more nutritious. The first day is survived.

6.2. The First 10 Hours: "Shelter"
The player has a basic grasp of survival. The next goal is a secure base.

They will gather wood and stone to build a 1x1 hut with a door, a bed, and a storage chest.

They will craft a full set of Crude Leather Armor and an Oaken Club or Hunting Bow.

They will likely have their first dangerous combat encounter with a wolf or a lone bandit.

They will discover the first nearby settlement, e.g., Millhaven, and interact with an NPC, receiving their first informal "quest."

6.3. The First Season (Mid-Game): "Prosperity"
The player's base is now a small, fortified compound with crafting stations for Smithing, Carpentry, and Alchemy.

They are actively engaging with a chosen faction, running quests for them and increasing their reputation.

They have moved on to Tier 2 (Iron) gear and are capable of taking on small groups of enemies and exploring dangerous ruins.

The gameplay loop shifts from pure survival to expedition planning: stocking up on supplies to venture into a new biome, retrieve rare resources, and return to upgrade their gear and base.

6.4. The First Year (End-Game): "Power"
The player is now a significant local power. Their base is a small fortress, possibly with hired NPC guards.

They are a high-ranking member of their chosen faction, capable of influencing its decisions.

They are crafting Tier 3 (Steel) or even unique "Masterwork" gear.

The gameplay is no longer about personal survival but about large-scale projects: establishing and defending a major trade route, leading a faction's army to destroy a rival's stronghold, or completing a master-level project for the Circle of Hands that permanently changes a part of the world map.

7. The Survival Simulation: A Hostile World
7.1. The Needs System (Granular)
Hunger/Thirst: Deplete over time, faster with exertion. Penalties range from slowed stamina regen to health loss at zero.

Sleep: Lack of sleep causes 'Exhaustion', reducing max stamina and XP gain. Sleeping in a proper bed provides a "Well Rested" bonus.

Temperature: Being cold increases hunger and risks hypothermia (reduces max health). Being hot increases thirst. Clothing provides insulation.

7.2. The Health & Injury System
Health does not regenerate automatically.

Heavy damage can cause specific, long-lasting injuries that require splints or extended rest to heal.

Broken Arm: Cannot use two-handed weapons or shields.

Broken Leg: Reduced movement speed, cannot sprint.

Concussion: Distorted vision, scrambled map.

Table 7.1: Consumable Items (Partial)
Item	Hunger	Thirst	Spoil Time (Days)	Disease Chance	Notes
Raw Rabbit Meat	+10	0	1.0	40%	Can be cooked.
Cooked Rabbit Meat	+25	0	3.0	0%	
Red Berries	+5	+2	2.0	0%	Safe to eat.
Purple Berries	+5	+2	2.0	90%	Causes "Gut Rot".
Stagnant Water	0	+15	N/A	25%	Causes "The Shakes".
Clean Water	0	+30	N/A	0%	Boiled or from a pure source.
Healing Salve	0	0	N/A	0%	Heals 30 HP over 10s.
Splint	0	0	N/A	0%	Required to begin healing a broken limb.
Table data from the Game Design Document

8. The Combat System: Brutal & Deliberate
8.1. Combat Philosophy
Combat is stamina-based, weighty, and deadly. Button-mashing is a death sentence. It is a puzzle of positioning, timing, and resource management.

8.2. Damage Calculation Formula
FinalDamage = (BaseWeaponDamage * (1 + (RelevantAttribute * 0.05))) * BodyPartModifier * (1 - (TargetArmorDR * (1 - WeaponArmorPen)))

BodyPartModifier: Head=2.0x, Torso=1.0x, Limbs=0.75x.

TargetArmorDR: Damage Reduction value on the armor piece (e.g., 0.25 for 25% DR).

WeaponArmorPen: Armor Penetration value on the weapon (e.g., 0.1 for 10% Pen).

Table 8.1: Weapon Statistics (Tier 1)
Weapon	Damage	Speed	Stamina (Light/Heavy)	Special Property
Crude Stone Axe	12	0.8	15 / 25	Sunder: Reduces enemy armor effectiveness by 10% for 5s.
Iron Shortsword	15	1.1	12 / 22	Bleed: Chance to apply a damage-over-time effect.
Hunting Bow	18	N/A	5 (draw)	Damage scales with draw time.
Oaken Club	10	0.9	10 / 20	Concussive: Higher chance to stagger opponents.
Table data from the Game Design Document

8.3. Stealth System
Based on light, sound, and movement speed. A UI indicator shows the player's current visibility/noise level.

Successful stealth attacks from behind with appropriate weapons (daggers) deal massive bonus damage.

9. The Crafting & Construction Systems
9.1. Gathering & Resources
Resources have tiers (Tier 1: Copper, Tier 2: Iron, Tier 3: Steel). Higher tiers are in more dangerous areas.

Some rare resources only appear at certain times or under specific weather conditions.

9.2. Crafting
Performed at upgradeable Crafting Stations.

Quality System: Crafting skill influences outcome, with a chance to create "Fine" or "Masterwork" items with bonus stats.

Breakthroughs: Advanced recipes are not automatically learned. Players must experiment with materials at a Research Bench to discover them.

9.3. Construction (Base Building)
Modular, socket-based system (foundations, walls, roofs, etc.).

Structural Integrity: Buildings require proper support (pillars, foundations) or they will collapse.

Defenses: Players can build walls, gates, and traps to defend their base from raids.

10. The Economic Simulation
10.1. Regional Markets
Each major town has its own simulated market. Prices fluctuate based on local supply and demand, which is affected by world events.

Example: A harsh winter causes a food shortage in the Iron Peaks, increasing the price of grain. This creates a profitable (but dangerous) trade opportunity.

10.2. Trade Routes
NPC caravans physically travel between towns. These can be raided by bandits (or the player), disrupting supply lines and affecting prices.

Players can eventually invest in or run their own caravans.

11. The AI & NPC Simulation
11.1. The AI Director
A "storyteller" AI that monitors the world state and generates dynamic events and quests to create conflict and opportunity based on systemic pressures.

If a region's wolf population is decimated, the deer population will boom, and the Director may spawn a "cull the herd" quest or a new apex predator.

11.2. The 'Day in the Life' System
NPCs have schedules, jobs, homes, needs, and relationships.

Example: A blacksmith whose ore supply is cut off by bandits will stop working, complain at the tavern, and his happiness will drop, possibly leading him to generate a quest to clear the trade route.

12. The Quest & Narrative System
12.1. Philosophy: System-Driven Narrative
Quests should feel organic and grounded. Many will not have map markers, requiring players to listen to dialogue and read their journal.

12.2. Dynamic Quest Generator
The AI Director generates quests based on templates:

Cull: "Too many wolves near the village."

Rescue: "My brother was captured by bandits."

Procurement: "The blacksmith needs 20 iron ore."

Caravan Guard: "Protect my goods on the road to Millhaven."

12.3. Dialogue & Rumor System
A keyword-based system. Players can hear rumors and then ask other NPCs about them.

Skill-gated choices: [Persuasion 50] Lie about your identity.

13. UI/UX Design: An Invisible Hand
13.1. Core Principles
Immersive & Diegetic: Minimal HUD. Physical map item, journal item.

Informative but Unobtrusive: Clear tooltips are essential. Status icons only appear when a need is critical.

Customizable: Players can toggle most HUD elements.

13.2. Detailed Screen Breakdowns
Inventory: Grid-based, with slots for equipped items and a detailed stats panel.

Map: Hand-drawn style, fills in as the player explores. Players can add custom markers.

Journal: Automatically logs conversations and quest objectives. Has a separate section for player-written notes.

14. Game Balance & Data
14.1. The First 10 Hours
The player should feel constantly challenged but have a clear path to stability. By hour 10, they should have a secure, basic base, Tier 1 gear, and have made contact with at least one major faction.

14.2. Mid-Game & End-Game Loops
Mid-Game: Focus shifts to exploration, Tier 2-3 gear, and increasing faction reputation. The loop involves preparing for long expeditions and returning with valuable resources/knowledge.

End-Game: There is no "end." The loop becomes about large-scale influence: building a trade empire, leading a faction to dominance, or rebuilding a piece of major Valorian technology that permanently alters the game world.

15. Art & Audio Direction
15.1. Visual Style
Aesthetic: "Dirty Medieval Realism." Muted, desaturated colors. Inspired by the raw, grounded look of films like The Witch and The Northman.

Light: A key visual and gameplay element. The world should be genuinely dark at night and in dungeons, making torches and lanterns essential tools, not just decorations.

15.2. Sound Design Philosophy
Focus: Immersion and information. The player should be able to identify threats by sound before seeing them.

Audio: Ambient, minimalist soundscapes. The howl of wind, a distant cry of a wolf, the specific crunch of boots on gravel versus snow.

Music: Sparse and diegetic where possible (a bard in a tavern). The main score is used only to punctuate moments of extreme tension, discovery, or rare tranquility.

<div align="center">
<p><em>This Game Bible is now definitively complete. It is the single source of truth for the design of Dark Age.</em></p>
</div>



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
