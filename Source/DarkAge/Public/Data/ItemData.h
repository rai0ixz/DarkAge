// ItemData.h - Defines item data structures for inventory, equipment, and consumables.
// Cleaned up and documented for clarity and maintainability.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    IT_Generic UMETA(DisplayName = "Generic"),
    IT_Consumable UMETA(DisplayName = "Consumable"),
    IT_Resource UMETA(DisplayName = "Resource"),
    IT_Equipment UMETA(DisplayName = "Equipment"),
    IT_Weapon UMETA(DisplayName = "Weapon"),
    IT_QuestItem UMETA(DisplayName = "Quest Item"),
    IT_Alchemical UMETA(DisplayName = "Alchemical"),
    IT_Tool UMETA(DisplayName = "Tool")
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    ES_Head UMETA(DisplayName = "Head"),
    ES_Chest UMETA(DisplayName = "Chest"),
    ES_Hands UMETA(DisplayName = "Hands"),
    ES_Legs UMETA(DisplayName = "Legs"),
    ES_Feet UMETA(DisplayName = "Feet"),
    ES_Back UMETA(DisplayName = "Back")
};

UENUM(BlueprintType)
enum class EHarvestToolType : uint8
{
    HTT_None UMETA(DisplayName = "None"),
    HTT_Pickaxe UMETA(DisplayName = "Pickaxe"),
    HTT_Hatchet UMETA(DisplayName = "Hatchet"),
    HTT_Sickle UMETA(DisplayName = "Sickle"),
    HTT_SkinningKnife UMETA(DisplayName = "Skinning Knife")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    General UMETA(DisplayName = "General"),
    Food UMETA(DisplayName = "Food"),
    Weapons UMETA(DisplayName = "Weapons"),
    Tools UMETA(DisplayName = "Tools"),
    Luxury UMETA(DisplayName = "Luxury")
};

USTRUCT(BlueprintType)
struct FStatModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    FName StatName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    float Value = 0.0f;
};

USTRUCT(BlueprintType)
struct FItemQuality
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Quality")
    int32 QualityValue = 0; // 0-100 or similar
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Quality")
    FString QualityLabel;
};

/**
 * FItemData
 * Represents all data for an item, including inventory, equipment, consumable, and weapon properties.
 * Used in data tables and for runtime item instantiation.
 */
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

    /** Unique identifier for this item */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    FName ItemID;

    /** Display name for UI and tooltips */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    FText DisplayName;

    /** Description for UI and tooltips */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    FText Description;

    /** Type of item (consumable, equipment, etc.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    EItemType ItemType;

    /** Icon for UI */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    TSoftObjectPtr<UTexture2D> Icon;

    /** Can this item stack? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    bool bIsStackable;

    /** Max stack size if stackable */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data", meta = (EditCondition = "bIsStackable"))
    int32 MaxStackSize;

    /** Item weight */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    float Weight;

    /** Item category for market and gameplay logic */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    EItemCategory Category = EItemCategory::General;

    // --- Equipment Properties ---
    /** Equipment slot (if equipment) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Equipment"))
    EEquipmentSlot EquipmentSlot;

    /** Protection value (if equipment) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Equipment"))
    float ProtectionValue;

    // --- Tool Properties ---
    /** Tool type (if tool) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Tool"))
    EHarvestToolType ToolType;

    /** Harvesting power (if tool) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Tool"))
    float HarvestingPower;

    // --- Consumable Effects ---
    /** Stat modifiers applied on use (if consumable) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Effects", meta = (EditCondition = "ItemType == EItemType::IT_Consumable"))
    TArray<FStatModifier> StatModifiers;

    /** Disease cured on use (if consumable) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Effects", meta = (EditCondition = "ItemType == EItemType::IT_Consumable"))
    FName DiseaseToCure;

    /** Status effects applied on use */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Effects")
    TArray<FName> StatusEffectsToApply;

    // --- Weapon Properties ---
    /** Weapon damage (if weapon) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Weapon"))
    float Damage;

    // --- General Properties ---
    /** Durability (if weapon, equipment, or tool) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data|Properties", meta = (EditCondition = "ItemType == EItemType::IT_Weapon || ItemType == EItemType::IT_Equipment || ItemType == EItemType::IT_Tool", ClampMin = "0.0", ClampMax = "100.0"))
    float Durability;

    /** Rarity for loot tables and UI */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    EItemRarity Rarity;

    /** Quality for crafting and item grading */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    FItemQuality Quality;

    /**
     * Default constructor initializes all members to safe defaults.
     */
    FItemData()
        : ItemID(NAME_None)
        , DisplayName()
        , Description()
        , ItemType(EItemType::IT_Generic)
        , Icon(nullptr)
        , bIsStackable(false)
        , MaxStackSize(1)
        , Weight(0.0f)
        , Category(EItemCategory::General)
        , EquipmentSlot(EEquipmentSlot::ES_Chest)
        , ProtectionValue(0.0f)
        , ToolType(EHarvestToolType::HTT_None)
        , HarvestingPower(0.0f)
        , StatModifiers()
        , DiseaseToCure(NAME_None)
        , StatusEffectsToApply()
        , Damage(0.0f)
        , Durability(100.0f)
        , Rarity(EItemRarity::Common)
        , Quality()
    {}
};