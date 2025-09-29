// Dark Age - MVP Implementation Plan
//
// This file contains the complete, low-level to-do list for implementing the
// Minimum Viable Product (MVP). Follow these steps sequentially to build the
// core gameplay loop.

// TODO: [MVP-PLAN] PHASE 1: FOUNDATION - SYSTEM VALIDATION AND EDITOR SETUP

// TODO: [P1.1] Create Blueprint class `BP_DAPlayerCharacter` from `DAPlayerCharacter`.
// TODO: [P1.1] Create Blueprint class `BP_Miller` from its C++ base.
// TODO: [P1.1] Create Blueprint class `BP_Landowner` from its C++ base.
// TODO: [P1.1] Create Blueprint class `BP_TavernKeeper` from its C++ base.
// TODO: [P1.1] Create Blueprint class `BP_ResourceItem` from `DABaseItem`.
// TODO: [P1.1] Create Blueprint class `BP_ToolItem` from `DABaseItem`.
// TODO: [P1.1] Create Blueprint class `BP_WeaponItem` from `DABaseItem`.
// TODO: [P1.1] Create Blueprint class `BP_DAGameMode` from `DAGameMode`.
// TODO: [P1.1] Create Blueprint class `BP_DAGameInstance` from `DAGameInstance`.

// TODO: [P1.2] Create `DataTable` asset `DT_Quests` and link it in the `QuestManagementSubsystem`.
// TODO: [P1.2] Create `DataTable` asset `DT_AnimalSpecies` and link it in the `WorldEcosystemSubsystem`.
// TODO: [P1.2] Create `DataTable` asset `DT_SkillDefinitions` and link it in the `StatManager`.

// TODO: [P1.3] Open `ThirdPersonMap` and set `BP_DAGameMode` in World Settings.
// TODO: [P1.3] Verify `BP_DAGameMode` is set to use `BP_DAPlayerCharacter` as the Default Pawn.
// TODO: [P1.3] Place instances of `BP_Miller`, `BP_Landowner`, and `BP_TavernKeeper` in the level.

// TODO: [P1.4] Test basic interaction: approach an NPC and verify the `WBP_InteractionPrompt` appears.
// TODO: [P1.4] Test interaction key press: verify a simple "Hello" dialogue box appears.

// TODO: [MVP-PLAN] PHASE 2: CORE GAMEPLAY LOOP - "URGENT DELIVERY" QUEST

// TODO: [P2.1] Add a new row to `DT_Quests` for "Q001 - Urgent Delivery".
// TODO: [P2.1] Quest Giver: `BP_TavernKeeper`.
// TODO: [P2.1] Objectives: `Objective_TalkTo_Miller`, `Objective_ReturnTo_TavernKeeper`.
// TODO: [P2.1] Rewards: `10 Gold`, `Item_Bread`.

// TODO: [P2.2] In `BP_TavernKeeper`, implement logic to offer quest "Q001" on interaction.
// TODO: [P2.2] On acceptance, verify the quest appears in `WBP_UI_QuestLog`.

// TODO: [P2.3] In `BP_Miller`, implement logic to advance "Q001" if the player is on the correct objective.
// TODO: [P2.3] Verify the quest log UI updates to the "Return" objective.

// TODO: [P2.4] In `BP_TavernKeeper`, implement logic to complete the quest on the final step.
// TODO: [P2.4] On completion, call `EconomySubsystem` to add 10 gold.
// TODO: [P2.4] On completion, call inventory system to add `Item_Bread`.
// TODO: [P2.4] Verify gold updates on HUD and bread appears in inventory UI.

// TODO: [MVP-PLAN] PHASE 3: EXPANDING PLAYER AGENCY - SURVIVAL & ECONOMY

// TODO: [P3.1] Ensure `DAPlayerStatusWidget` correctly displays Hunger and Thirst from `AdvancedSurvivalSubsystem`.
// TODO: [P3.1] Create a `BP_ResourceNode` for "Berry Bush".
// TODO: [P3.1] Implement logic so interacting with the bush grants a "Berry" item and restores Hunger.

// TODO: [P3.2] Designate `BP_Landowner` as a vendor and create a basic store UI.
// TODO: [P3.2] Implement logic to allow the player to sell "Berries" to the `BP_Landowner`.

// TODO: [P3.3] Define a recipe in data: `2x Stick + 1x Flint = 1x StoneAxe`.
// TODO: [P3.3] Create a basic crafting UI to execute the recipe.
// TODO: [P3.3] On successful craft, grant "Crafting" skill XP via `StatManager`.
// TODO: [P3.3] Ensure the `StoneAxe` tool provides a tangible benefit (e.g., faster wood gathering).

// TODO: [MVP-PLAN] PHASE 4: BUILDING A DYNAMIC WORLD

// TODO: [P4.1] Use `NPCEcosystemSubsystem` to give `BP_Miller` a simple daily schedule (e.g., home to mill).

// TODO: [P4.2] Use `WorldEcosystemSubsystem` to spawn a `BP_Wolf` periodically to enable combat testing.

// TODO: [P4.3] Use `FactionManagerSubsystem` to grant `+10` "Village" reputation on "Q001" completion.
// TODO: [P4.3] Implement logic where NPCs refuse to talk if village reputation is below -20.

// TODO: [P4.4] Use `GovernanceSubsystem` to enact a "No Stealing" law.
// TODO: [P4.4] Place an item owned by `BP_TavernKeeper` in the world.
// TODO: [P4.4] Verify that taking the item notifies the `CrimeManagerSubsystem` and applies a consequence (e.g., reputation loss).