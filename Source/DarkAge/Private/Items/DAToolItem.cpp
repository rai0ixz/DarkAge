#include "Items/DAToolItem.h"
#include "World/DAResourceNode.h"

ADAToolItem::ADAToolItem()
{
    // Set default values for a tool item
    ItemData.ItemType = EItemType::IT_Tool;
}

void ADAToolItem::UseTool(class ADAResourceNode* ResourceNode, APawn* InstigatorPawn)
{
    if (ResourceNode)
    {
        ResourceNode->TakeHarvestDamage(ItemData.HarvestingPower, ItemData.ToolType, InstigatorPawn);
    }
}