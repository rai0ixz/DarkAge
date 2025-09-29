#include "Components/CraftingComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/PlayerSkillsComponent.h"
#include "Core/DAGameInstance.h"
#include "Data/CraftingData.h"
#include "Data/InventoryData.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Core/KnowledgeSubsystem.h" // Add this include

UCraftingComponent::UCraftingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    OwningInventoryComponent = nullptr;
    GameInstance = nullptr;
    LoadedCraftingRecipesDataTable = nullptr;
    OwningSkillsComponent = nullptr;
}

void UCraftingComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (Owner)
    {
        OwningInventoryComponent = Owner->FindComponentByClass<UInventoryComponent>();
        if (!OwningInventoryComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("CraftingComponent: Owning actor %s does not have an InventoryComponent."), *Owner->GetName());
        }

        OwningSkillsComponent = Owner->FindComponentByClass<UPlayerSkillsComponent>();
        if (!OwningSkillsComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("CraftingComponent: Owning actor %s does not have a PlayerSkillsComponent. Skill checks will be ignored."), *Owner->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CraftingComponent: Could not get Owner actor."));
    }

    if (GetWorld())
    {
        GameInstance = Cast<UDAGameInstance>(GetWorld()->GetGameInstance());
        if (!GameInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("CraftingComponent: Could not get DAGameInstance. Item lookups will fail."));
        }
    }

    if (CraftingRecipesDataTablePtr.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("CraftingComponent: CraftingRecipesDataTablePtr is not set in Blueprint Defaults for %s."), *GetNameSafe(GetOwner()));
        LoadedCraftingRecipesDataTable = nullptr;
    }
    else
    {
        LoadedCraftingRecipesDataTable = CraftingRecipesDataTablePtr.LoadSynchronous();
        if (!LoadedCraftingRecipesDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("CraftingComponent: Failed to load CraftingRecipesDataTable from path: %s"), *CraftingRecipesDataTablePtr.ToSoftObjectPath().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("CraftingComponent: CraftingRecipesDataTable '%s' loaded successfully."), *LoadedCraftingRecipesDataTable->GetName());
        }
    }
}

bool UCraftingComponent::GetRecipeData(FName RecipeID, FCraftingRecipe& OutRecipeData) const
{
    if (!LoadedCraftingRecipesDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetRecipeData: LoadedCraftingRecipesDataTable is NULL."));
        return false;
    }
    if (RecipeID == NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetRecipeData: RecipeID is NAME_None."));
        return false;
    }

    FString ContextString(TEXT("Getting Crafting Recipe Data"));
    FCraftingRecipe* Row = LoadedCraftingRecipesDataTable->FindRow<FCraftingRecipe>(RecipeID, ContextString);

    if (Row)
    {
        OutRecipeData = *Row;
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("GetRecipeData: RecipeID '%s' not found in CraftingRecipesDataTable '%s'."), *RecipeID.ToString(), *GetNameSafe(LoadedCraftingRecipesDataTable));
    return false;
}

bool UCraftingComponent::CanCraftRecipe(FName RecipeID) const
{
    if (!OwningInventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanCraftRecipe: OwningInventoryComponent is NULL."));
        return false;
    }
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanCraftRecipe: GameInstance is NULL."));
        return false;
    }

    FCraftingRecipe RecipeData;
    if (!GetRecipeData(RecipeID, RecipeData))
    {
        return false;
    }

    // Check Skill Requirements
    if (RecipeData.RequiredSkillID != NAME_None)
    {
        if (OwningSkillsComponent)
        {
            if (OwningSkillsComponent->GetSkillLevel(RecipeData.RequiredSkillID) < RecipeData.RequiredSkillLevel)
            {
                return false; // Skill level too low
            }
        }
        else
        {
            return false; // Skill is required but component is missing
        }
    }

    // Check Ingredient Requirements
    for (const FCraftingIngredient& Ingredient : RecipeData.Ingredients)
    {
        if (Ingredient.ItemID == NAME_None || Ingredient.Quantity <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("CanCraftRecipe: Recipe '%s' has invalid ingredient (ItemID None or Quantity <= 0)."), *RecipeID.ToString());
            return false;
        }
        FItemData ItemData;
        ItemData.ItemID = Ingredient.ItemID;
        if (!OwningInventoryComponent->HasItem(ItemData, Ingredient.Quantity))
        {
            return false;
        }
    }

    return true;
}

bool UCraftingComponent::AttemptCraftRecipe(FName RecipeID)
{
    if (!OwningInventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AttemptCraftRecipe: OwningInventoryComponent is NULL."));
        return false;
    }
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("AttemptCraftRecipe: GameInstance is NULL. Cannot verify output item data."));
        return false;
    }

    // Use the combined CanCraftRecipe function to check ingredients and skills
    if (!CanCraftRecipe(RecipeID))
    {
        UE_LOG(LogTemp, Warning, TEXT("AttemptCraftRecipe: Cannot craft recipe '%s' - ingredients or skill check failed."), *RecipeID.ToString());
        return false;
    }

    FCraftingRecipe RecipeData;
    GetRecipeData(RecipeID, RecipeData); // We know this will succeed because CanCraftRecipe passed

    FItemData OutputItemInfo;
    if (RecipeData.OutputItemID == NAME_None || !GameInstance->GetItemData(RecipeData.OutputItemID, OutputItemInfo) || RecipeData.OutputQuantity <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("AttemptCraftRecipe: Recipe '%s' has an invalid OutputItemID or OutputQuantity."), *RecipeID.ToString());
        return false;
    }

    // All checks passed, proceed to remove ingredients
    for (const FCraftingIngredient& Ingredient : RecipeData.Ingredients)
    {
        FItemData ItemData;
        ItemData.ItemID = Ingredient.ItemID;
        OwningInventoryComponent->RemoveItem(ItemData, Ingredient.Quantity);
        {
            UE_LOG(LogTemp, Error, TEXT("AttemptCraftRecipe: CRITICAL - Failed to remove ingredient '%s' after CanCraftRecipe passed."), *Ingredient.ItemID.ToString());
            // NOTE: This case should ideally trigger a rollback of previously removed items.
            // For now, we'll log the error and fail the craft.
            return false;
        }
    }

    // --- Next-level: Blueprint, Rarity, Quality ---
    // Check for blueprint requirement
    FItemData BlueprintItemData;
    BlueprintItemData.ItemID = RecipeData.RequiredBlueprintID;
    if (RecipeData.bRequiresBlueprint && !OwningInventoryComponent->HasItem(BlueprintItemData, 1))
    {
        UE_LOG(LogTemp, Warning, TEXT("AttemptCraftRecipe: Missing required blueprint '%s' for recipe '%s'."), *RecipeData.RequiredBlueprintID.ToString(), *RecipeID.ToString());
        return false;
    }
    // Remove blueprint if it's consumable (optional: implement blueprint consumption logic)
    // --- End Blueprint Check ---

    // --- Skill/Perk Integration Start ---
    EItemRarity OutputRarity = RecipeData.OutputRarity;
    int32 OutputQuality = FMath::RandRange(RecipeData.OutputQualityMin, RecipeData.OutputQualityMax);
    int32 OutputQuantity = RecipeData.OutputQuantity;
    if (OwningSkillsComponent && RecipeData.RequiredSkillID != NAME_None)
    {
        int32 SkillLevel = OwningSkillsComponent->GetSkillLevel(RecipeData.RequiredSkillID);
        // Example: +1 quality per 5 skill levels
        OutputQuality += SkillLevel / 5;
        // Example: If player has a perk, increase quantity
        if (OwningSkillsComponent->HasPerk(RecipeData.RequiredSkillID, FName("CRAFT_BonusYield")))
        {
            OutputQuantity += 1;
        }
        // Example: If player has a perk, increase rarity
        if (OwningSkillsComponent->HasPerk(RecipeData.RequiredSkillID, FName("CRAFT_RarityBoost")))
        {
            OutputRarity = static_cast<EItemRarity>(FMath::Clamp((int32)OutputRarity + 1, 0, (int32)EItemRarity::Legendary));
        }
        // Clamp quality to max
        OutputQuality = FMath::Clamp(OutputQuality, RecipeData.OutputQualityMin, 100); // Assuming 100 is max quality
    }
    // --- Skill/Perk Integration End ---

    // Add output item(s) with skill/perk-modified rarity/quality/quantity
    FItemData OutputItemData;
    OutputItemData.ItemID = RecipeData.OutputItemID;
    OwningInventoryComponent->AddItem(OutputItemData, OutputQuantity);
    bool bAdded = true; // Assume AddItem is void and successful
    if (bAdded)
    {
        UE_LOG(LogTemp, Log, TEXT("AttemptCraftRecipe: Successfully crafted '%s' (Rarity: %d, Quality: %d, Quantity: %d)."), *RecipeData.DisplayName.ToString(), (int32)OutputRarity, OutputQuality, OutputQuantity);
        OnRecipeCrafted.Broadcast(RecipeID);

        // --- Knowledge System Integration ---
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GameInstanceObj = World->GetGameInstance())
            {
                UKnowledgeSubsystem* KnowledgeSubsystem = GameInstanceObj->GetSubsystem<UKnowledgeSubsystem>();
                if (KnowledgeSubsystem)
                {
                    // Gather input item IDs
                    TArray<FName> InputIDs;
                    for (const FCraftingIngredient& Ingredient : RecipeData.Ingredients)
                    {
                        InputIDs.Add(Ingredient.ItemID);
                    }
                    // Use RecipeID as process, OutputItemID as result, and Owner's name as PlayerID
                    FString PlayerID = GetOwner() ? GetOwner()->GetName() : TEXT("Unknown");
                    KnowledgeSubsystem->RecordExperiment(InputIDs, RecipeID, RecipeData.OutputItemID, PlayerID);
                }
            }
        }
        // --- End Knowledge System Integration ---
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AttemptCraftRecipe: CRITICAL - Removed ingredients but FAILED to add output item '%s' (Inventory full?)."), *RecipeData.OutputItemID.ToString());
        // Refund ingredients
        for (const FCraftingIngredient& Ingredient : RecipeData.Ingredients)
        {
            FItemData ItemData;
            ItemData.ItemID = Ingredient.ItemID;
            OwningInventoryComponent->AddItem(ItemData, Ingredient.Quantity);
        }
        return false;
    }
}

bool UCraftingComponent::CraftItem(FName RecipeID)
{
    // CraftItem is a simple wrapper around AttemptCraftRecipe for debug purposes
    UE_LOG(LogTemp, Log, TEXT("CraftItem: Attempting to craft recipe '%s'"), *RecipeID.ToString());
    
    bool bSuccess = AttemptCraftRecipe(RecipeID);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("CraftItem: Successfully crafted recipe '%s'"), *RecipeID.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CraftItem: Failed to craft recipe '%s'"), *RecipeID.ToString());
    }
    
    return bSuccess;
}