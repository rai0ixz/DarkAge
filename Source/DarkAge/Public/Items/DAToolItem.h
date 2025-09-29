#pragma once

#include "CoreMinimal.h"
#include "Items/DABaseItem.h"
#include "DAToolItem.generated.h"

UCLASS()
class DARKAGE_API ADAToolItem : public ADABaseItem
{
    GENERATED_BODY()

public:
    ADAToolItem();

    UFUNCTION(BlueprintCallable, Category = "Tool")
    void UseTool(class ADAResourceNode* ResourceNode, APawn* InstigatorPawn);
};