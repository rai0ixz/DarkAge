#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.h" // Include ItemData.h for EItemRarity and FItemQuality
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FCraftingIngredient
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Ingredient")
    FName ItemID; // References FItemData RowName from your DT_Items

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Ingredient", meta = (ClampMin = "1"))
    int32 Quantity;

    FCraftingIngredient() : ItemID(NAME_None), Quantity(1) {}
};

USTRUCT(BlueprintType)
struct FCraftingRecipe : public FTableRowBase // Each row in your DT_CraftingRecipes DataTable will be an FCraftingRecipe
{
    GENERATED_BODY()

public:
    // The ItemID of the successfully crafted item
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe")
    FName OutputItemID;  // References FItemData RowName from your DT_Items

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe", meta = (ClampMin = "1"))
    int32 OutputQuantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe")
    TArray<FCraftingIngredient> Ingredients;

    // For UI display of the recipe
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe")
    FText Description; // Optional description for the recipe itself

    // GDD 3.1.2 Skills & Progression / GDD 3.5.4 Crafting - Placeholder for future skill system integration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Requirements")
    FName RequiredSkillID; // e.g., "SKL_ALCHEMY", "SKL_BLACKSMITHING"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Requirements", meta = (ClampMin = "0"))
    int32 RequiredSkillLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Requirements")
    FName CraftingStationTag; // e.g., "Forge", "AlchemyTable" - if specific station is needed

    // --- Next-level: Blueprint/Quality/Rarity ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Blueprints")
    bool bRequiresBlueprint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Blueprints")
    FName RequiredBlueprintID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Output")
    EItemRarity OutputRarity = EItemRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Output")
    int32 OutputQualityMin = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Recipe|Output")
    int32 OutputQualityMax = 100;

    FCraftingRecipe()
        : OutputItemID(NAME_None)
        , OutputQuantity(1)
        , RequiredSkillID(NAME_None)
        , RequiredSkillLevel(0)
    {
    }
};