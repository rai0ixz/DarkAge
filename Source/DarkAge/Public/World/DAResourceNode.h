#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ResourceNodeData.h"
#include "Interfaces/DAInteractableInterface.h"
#include "DAResourceNode.generated.h"

/**
 * @brief Represents a resource node in the world (e.g., tree, rock, ore vein).
 *
 * Handles harvesting logic, health, respawn, and integration with inventory and tool systems.
 *
 * @see [API Doc](../../../docs/api/DAResourceNode.md)
 */
UCLASS()
class DARKAGE_API ADAResourceNode : public AActor, public IDAInteractableInterface
{
    GENERATED_BODY()

public:
    /**
     * Default constructor
     */
    ADAResourceNode();

    //~ Begin IDAInteractableInterface
    /**
     * Handles player interaction with the node
     */
    virtual void OnInteract_Implementation(AActor* InstigatorActor) override;
    //~ End IDAInteractableInterface

    /**
     * Applies harvesting damage from a tool
     */
    UFUNCTION(BlueprintCallable, Category = "Resource Node")
    void TakeHarvestDamage(float Damage, EHarvestToolType ToolType, APawn* InstigatorPawn);

protected:
    /**
     * Called when the game starts
     */
    virtual void BeginPlay() override;

    /**
     * DataTable reference for node configuration
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node")
    UDataTable* ResourceNodeDataTable;

    /**
     * Unique ID for this resource node type
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node")
    FName ResourceNodeID;

    /**
     * Data struct for node type, health, yields, etc.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Node")
    FResourceNodeData NodeData;

    /**
     * Current health of the node
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Node")
    float CurrentHealth;

private:
    /**
     * Initializes node data from the DataTable
     */
    void InitializeFromDataTable();

    /**
     * Respawns the node after a set time
     */
    void RespawnNode();
};