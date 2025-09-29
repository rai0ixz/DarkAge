// Copyright (c) 2024, Raio Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DAQuestItem.generated.h"

/**
 * Represents an item that is required for or related to a quest.
 */
UCLASS(Blueprintable, BlueprintType)
class DARKAGE_API UDAQuestItem : public UObject
{
	GENERATED_BODY()

public:
	UDAQuestItem();

	// The ID of this item, linking it to the item data table.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	// Quest ID this item is associated with
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestID;

	// Determines if this item should be removed from inventory upon quest completion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	bool bIsRemovedOnQuestCompletion;
};