#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DADamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKAGE_API UDADamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SetDamageText(float Damage);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetWorldLocation(const FVector& WorldLocation);
};