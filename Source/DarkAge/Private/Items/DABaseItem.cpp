#include "Items/DABaseItem.h"
#include "Core/DAGameInstance.h"
#include "Kismet/GameplayStatics.h"

ADABaseItem::ADABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADABaseItem::BeginPlay()
{
    Super::BeginPlay();
    InitializeFromDataTable();
}

void ADABaseItem::InitializeFromDataTable()
{
    FetchItemData();
}

#if WITH_EDITOR
void ADABaseItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADABaseItem, ItemID) ||
        PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADABaseItem, ItemDataTable))
    {
        FetchItemData();
    }
}
#endif

void ADABaseItem::FetchItemData()
{
    if (ItemDataTable)
    {
        FString ContextString(TEXT("Fetching Item Data"));
        FItemData* Row = ItemDataTable->FindRow<FItemData>(ItemID, ContextString);
        if (Row)
        {
            ItemData = *Row;
        }
    }
}