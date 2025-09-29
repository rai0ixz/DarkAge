#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DAResponseButton.generated.h"

/**
 * 
 */
UCLASS()
class DARKAGE_API UDAResponseButton : public UButton
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 ResponseIndex;
	
};