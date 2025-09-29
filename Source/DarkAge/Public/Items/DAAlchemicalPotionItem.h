#pragma once

#include "CoreMinimal.h"
#include "Items/DAConsumableItem.h"
#include "DAAlchemicalPotionItem.generated.h"

/**
 * A specialized consumable item that applies one or more status effects to the consumer.
 * The specific effects are defined in the item's data table entry.
 */
UCLASS()
class DARKAGE_API ADAAlchemicalPotionItem : public ADAConsumableItem
{
	GENERATED_BODY()

public:
	/** Overrides the base Consume function to apply status effects from data. */
	virtual void Consume(APawn* InstigatorPawn) override;
};