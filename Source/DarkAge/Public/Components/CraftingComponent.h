#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CraftingComponent.generated.h"

// Forward declarations
class UInventoryComponent;
class UDAGameInstance;
class UPlayerSkillsComponent;
struct FItemData;
struct FCraftingRecipe;

// Optional: Delegate for UI updates or other systems to react to successful crafting
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeCraftedSuccessfully, FName, RecipeID);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UCraftingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCraftingComponent();

protected:
    virtual void BeginPlay() override;

    // Pointer to the player's inventory component
    UPROPERTY()
    TObjectPtr<UInventoryComponent> OwningInventoryComponent;

    // Add the new OwningSkillsComponent pointer
    UPROPERTY()
    TObjectPtr<UPlayerSkillsComponent> OwningSkillsComponent;

    // Pointer to the Game Instance for accessing Data Tables
    UPROPERTY()
    TObjectPtr<UDAGameInstance> GameInstance;

    // Data Table containing all crafting recipes
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crafting")
    TSoftObjectPtr<UDataTable> CraftingRecipesDataTablePtr;

    // Loaded version of the crafting recipes Data Table
    UPROPERTY()
    TObjectPtr<UDataTable> LoadedCraftingRecipesDataTable;

public:
    /**
     * Checks if a specific recipe can be crafted with the items currently in the OwningInventoryComponent.
     * @param RecipeID The Row Name of the recipe in the CraftingRecipesDataTable.
     * @return True if the recipe can be crafted, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Crafting")
    bool CanCraftRecipe(FName RecipeID) const;

    /**
     * Attempts to craft a specific recipe.
     * If successful, ingredients will be removed from the OwningInventoryComponent and the output item(s) will be added.
     * @param RecipeID The Row Name of the recipe in the CraftingRecipesDataTable.
     * @return True if the recipe was crafted successfully, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Crafting")
    bool AttemptCraftRecipe(FName RecipeID);

    UFUNCTION(BlueprintCallable, Category = "Crafting")
    bool CraftItem(FName RecipeID);

    // Optional: Delegate to broadcast when a recipe is successfully crafted
    UPROPERTY(BlueprintAssignable, Category = "Crafting")
    FOnRecipeCraftedSuccessfully OnRecipeCrafted;

private:
    // Helper function to get recipe data
    bool GetRecipeData(FName RecipeID, FCraftingRecipe& OutRecipeData) const;
};