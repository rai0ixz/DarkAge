# UCraftingComponent (CraftingComponent.h)

## Purpose
This component manages all crafting-related logic for an actor. It is designed to be highly data-driven, pulling all recipe information from a central DataTable. It works in close conjunction with the `UInventoryComponent` and `UPlayerSkillsComponent` to validate and execute crafting attempts.

## Key Properties
- `CraftingRecipesDataTablePtr (TSoftObjectPtr<UDataTable>)`: A soft pointer to the `UDataTable` that contains all crafting recipes in the game. This is set in the component's Blueprint defaults.

## Key Delegates
- `OnRecipeCrafted (FName RecipeID)`: A multicast delegate that is broadcast whenever a recipe is successfully crafted. UI or other gameplay systems can bind to this to provide feedback.

## Key Methods

### Core Crafting Logic
- `CanCraftRecipe(FName RecipeID) const`: Checks if the owning actor can craft a specific recipe. It validates three things:
    1.  Does the `OwningInventoryComponent` have the required ingredients?
    2.  Does the `OwningSkillsComponent` meet the skill requirements (if any)?
    3.  Does the recipe exist in the `CraftingRecipesDataTable`?
- `AttemptCraftRecipe(FName RecipeID)`: If `CanCraftRecipe` returns true, this function will:
    1.  Remove the required ingredients from the `OwningInventoryComponent`.
    2.  Add the output item(s) to the `OwningInventoryComponent`.
    3.  Broadcast the `OnRecipeCrafted` delegate.

## Data-Driven Design

This component is entirely driven by a `UDataTable` asset. The table should use the `FCraftingRecipe` struct as its row type.

### `FCraftingRecipe` (Struct)
- `Ingredients (TMap<FName, int32>)`: A map where the key is the Item ID (from the `DT_Items` table) and the value is the required quantity.
- `OutputItems (TMap<FName, int32>)`: A map defining the item(s) and quantities produced by the recipe.
- `SkillRequirements (TMap<FName, int32>)`: A map where the key is the Skill ID (e.g., "Blacksmithing") and the value is the required level.

## Example Usage

From a UI widget, when a player clicks a "Craft" button:
```cpp
// Assume CraftingComponent is a pointer to the player's crafting component
// and RecipeID is the FName of the recipe they want to craft.
if (CraftingComponent && CraftingComponent->CanCraftRecipe(RecipeID))
{
    // Attempt the craft. This should be called on the server.
    bool bSuccess = CraftingComponent->AttemptCraftRecipe(RecipeID);
    if (bSuccess)
    {
        // Play a success sound, show a notification, etc.
    }
    else
    {
        // This case should ideally not happen if CanCraftRecipe was checked,
        // but could occur in a multiplayer race condition.
        // Show a "Crafting Failed" message.
    }
}
else
{
    // The button should have been disabled, but as a fallback,
    // show a message like "Missing ingredients or skill level too low."
}
```

## Integration Points
- **`UInventoryComponent`**: This component is **required** on the same actor. The `CraftingComponent` gets a pointer to it in `BeginPlay` and uses it to check for and consume ingredients, and to add crafted items.
- **`UPlayerSkillsComponent`**: This component is also **required**. The `CraftingComponent` uses it to verify if the character meets the skill requirements for a given recipe.
- **`DT_CraftingRecipes` (DataTable)**: The component is useless without a properly configured DataTable containing all the game's recipes.
- **UI Widgets**: The UI reads the `DT_CraftingRecipes` to display available recipes and calls the functions on this component to perform the crafting actions.
