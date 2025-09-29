#include "UI/SubWidgets/DAInventoryWidget.h"

#include "Components/InventoryComponent.h"
#include "Components/ListView.h"
#include "UI/DAInventoryItemData.h"
#include "Items/DABaseItem.h"

void UDAInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
    InventoryComponent = InInventoryComponent;
}

void UDAInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshInventory();
}

void UDAInventoryWidget::RefreshInventory()
{
    if (InventoryComponent && InventoryList)
    {
        InventoryList->ClearListItems();
        const TArray<FInventorySlot>& Items = InventoryComponent->Items;
        for (const FInventorySlot& InventorySlot : Items)
        {
            UDAInventoryItemData* NewItemData = NewObject<UDAInventoryItemData>(this);
            NewItemData->ItemData.ItemID = InventorySlot.Item.ItemID.ToString();
            NewItemData->ItemData.Quantity = InventorySlot.Quantity;
            InventoryList->AddItem(NewItemData);
        }
    }
}