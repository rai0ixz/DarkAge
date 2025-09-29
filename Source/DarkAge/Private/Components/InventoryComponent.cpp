#include "Components/InventoryComponent.h"
#include "Core/DAGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/StatusEffectComponent.h"

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Items);
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::AddItem(const FItemData& Item, int32 Quantity)
{
	if (Quantity <= 0 || Item.ItemID == NAME_None)
	{
		return false;
	}

	// Handle stacking
	if (Item.bIsStackable)
	{
		for (FInventorySlot& Slot : Items)
		{
			if (Slot.Item.ItemID == Item.ItemID)
			{
				Slot.Quantity += Quantity;
				OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// Add to a new slot
	if (Items.Num() < 50) // Assuming a max slot limit
	{
		FInventorySlot NewSlot;
		NewSlot.Item = Item;
		NewSlot.Quantity = Quantity;
		Items.Add(NewSlot);
		OnInventoryUpdated.Broadcast();
		return true;
	}

	return false;
}

bool UInventoryComponent::RemoveItem(const FItemData& Item, int32 Quantity)
{
	if (Quantity <= 0 || Item.ItemID == NAME_None)
	{
		return false;
	}

	for (int32 i = Items.Num() - 1; i >= 0; --i)
	{
		if (Items[i].Item.ItemID == Item.ItemID)
		{
			if (Items[i].Quantity > Quantity)
			{
				Items[i].Quantity -= Quantity;
			}
			else
			{
				Items.RemoveAt(i);
			}
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasItem(const FItemData& Item, int32 Quantity) const
{
	for (const FInventorySlot& Slot : Items)
	{
		if (Slot.Item.ItemID == Item.ItemID && Slot.Quantity >= Quantity)
		{
			return true;
		}
	}
	return false;
}

void UInventoryComponent::UseItemBySlot(int32 SlotIndex)
{
    if (!Items.IsValidIndex(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("UseItemBySlot: Invalid SlotIndex %d."), SlotIndex);
        return;
    }

    FInventorySlot& Slot = Items[SlotIndex];
    if (UWorld* World = GetWorld())
    {
        UDAGameInstance* GameInstance = World->GetGameInstance<UDAGameInstance>();
        if (!GameInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("UseItemBySlot: Cannot find GameInstance."));
            return;
        }

        FItemData ItemData;
        if (GameInstance->GetItemData(Slot.Item.ItemID, ItemData))
        {
            UE_LOG(LogTemp, Log, TEXT("Used item: %s"), *ItemData.DisplayName.ToString());

            bool bItemUsed = false;

            switch (ItemData.ItemType)
            {
                case EItemType::IT_Consumable:
                {
                    UStatusEffectComponent* StatusEffectComp = GetOwner()->FindComponentByClass<UStatusEffectComponent>();
                    if (StatusEffectComp && ItemData.StatusEffectsToApply.Num() > 0)
                    {
                        for (const FName& EffectID : ItemData.StatusEffectsToApply)
                        {
                            FDataTableRowHandle EffectHandle;
                            // EffectHandle.DataTable = ...; // This would need to be loaded from a central place
                            EffectHandle.RowName = EffectID;
                            StatusEffectComp->ApplyEffect(EffectHandle);
                        }
                        bItemUsed = true;
                    }
                    break;
                }
                case EItemType::IT_Weapon:
                {
                    // UCharacterEquipmentMap* EquipmentComp = GetOwner()->FindComponentByClass<UCharacterEquipmentMap>();
                    // if (EquipmentComp)
                    // {
                    //     EquipmentComp->EquipItem(EEquipmentSlot::Weapon, ItemData);
                    //     bItemUsed = true;
                    // }
                    break;
                }
                case EItemType::IT_Tool:
                     // UCharacterEquipmentMap* EquipmentComp = GetOwner()->FindComponentByClass<UCharacterEquipmentMap>();
                    // if (EquipmentComp)
                    // {
                    //     EquipmentComp->EquipItem(EEquipmentSlot::Tool, ItemData);
                    //     bItemUsed = true;
                    // }
                    break;
                default:
                    UE_LOG(LogTemp, Warning, TEXT("Item %s is of a type that cannot be 'used' directly."), *ItemData.DisplayName.ToString());
                    break;
            }

            // If consumable and used successfully, remove one from the stack
            if (bItemUsed && ItemData.ItemType == EItemType::IT_Consumable)
            {
                Slot.Quantity--;
                if (Slot.Quantity <= 0)
                {
                    Items.RemoveAt(SlotIndex);
                }
                OnInventoryUpdated.Broadcast();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UseItemBySlot: Could not find item data for ItemID %s."), *Slot.Item.ItemID.ToString());
        }
    }
}

bool UInventoryComponent::CanAfford(int32 Amount) const
{
    return Currency >= Amount;
}

void UInventoryComponent::RemoveCurrency(int32 Amount)
{
    Currency -= Amount;
}
int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
    for (const FInventorySlot& Slot : Items)
    {
        if (Slot.Item.ItemID == ItemID)
        {
            return Slot.Quantity;
        }
    }
    return 0;
}

bool UInventoryComponent::AddInventoryItem(FName ItemID, int32 Quantity)
{
    // Create a basic FItemData with the ItemID
    FItemData ItemData;
    ItemData.ItemID = ItemID;
    ItemData.bIsStackable = true; // Assume stackable for debug purposes
    
    return AddItem(ItemData, Quantity);
}

void UInventoryComponent::PrintInventory()
{
    UE_LOG(LogTemp, Log, TEXT("=== Inventory Contents ==="));
    UE_LOG(LogTemp, Log, TEXT("Currency: %d"), Currency);
    
    if (Items.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Inventory is empty"));
    }
    else
    {
        for (int32 i = 0; i < Items.Num(); ++i)
        {
            const FInventorySlot& Slot = Items[i];
            UE_LOG(LogTemp, Log, TEXT("Slot %d: %s x%d"), i, *Slot.Item.ItemID.ToString(), Slot.Quantity);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("========================"));
}