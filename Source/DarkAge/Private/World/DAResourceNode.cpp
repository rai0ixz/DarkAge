#include "World/DAResourceNode.h"
#include "Items/DAToolItem.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/DAGameInstance.h"

ADAResourceNode::ADAResourceNode()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentHealth = 0.f;
}

void ADAResourceNode::BeginPlay()
{
    Super::BeginPlay();
    InitializeFromDataTable();
}

void ADAResourceNode::InitializeFromDataTable()
{
    if (ResourceNodeDataTable)
    {
        FString ContextString(TEXT("Fetching Resource Node Data"));
        FResourceNodeData* Row = ResourceNodeDataTable->FindRow<FResourceNodeData>(ResourceNodeID, ContextString);
        if (Row)
        {
            NodeData = *Row;
            CurrentHealth = NodeData.NodeHealth;
        }
    }
}

void ADAResourceNode::OnInteract_Implementation(AActor* InstigatorActor)
{
    // This is a basic implementation. A real implementation would get the character's currently equipped item.
    // For now, we assume the interaction logic will find a suitable tool in the inventory and call TakeHarvestDamage.
    APawn* InstigatorPawn = Cast<APawn>(InstigatorActor);
    if (!InstigatorPawn)
    {
        return;
    }
    
    // Example: Find a tool and use it.
    // ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(InstigatorPawn);
    // if(PlayerCharacter)
    // {
    //      ADAToolItem* Tool = PlayerCharacter->GetEquippedTool();
    //      if(Tool)
    //      {
    //          TakeHarvestDamage(Tool->ItemData.HarvestingPower, Tool->ItemData.ToolType, InstigatorPawn);
    //      }
    // }
}

void ADAResourceNode::TakeHarvestDamage(float Damage, EHarvestToolType ToolType, APawn* InstigatorPawn)
{
    if (ToolType != NodeData.RequiredToolType || Damage < NodeData.RequiredHarvestingPower)
    {
        // Tool is not suitable, maybe play a "thud" sound.
        return;
    }

    CurrentHealth -= Damage;

    if (CurrentHealth <= 0.f)
    {
        ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(InstigatorPawn);
        if (PlayerCharacter && PlayerCharacter->GetInventoryComponent())
        {
            for (const FResourceYield& Yield : NodeData.ResourceYields)
            {
                FItemData ItemToAdd;
                UDAGameInstance* GameInstance = Cast<UDAGameInstance>(GetGameInstance());
                if (GameInstance)
                {
                    GameInstance->GetItemData(Yield.ItemID, ItemToAdd);
                    PlayerCharacter->GetInventoryComponent()->AddItem(ItemToAdd, Yield.Quantity);
                }
            }
        }

        // Hide the node and disable collision
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);

        // Set a timer to respawn the node
        FTimerHandle RespawnTimer;
        GetWorldTimerManager().SetTimer(RespawnTimer, this, &ADAResourceNode::RespawnNode, NodeData.RespawnTime, false);
    }
}

void ADAResourceNode::RespawnNode()
{
    // Reset health and make the node visible and interactable again
    CurrentHealth = NodeData.NodeHealth;
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}