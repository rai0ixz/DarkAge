#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DAResponseOptionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResponseOptionClicked, int32, ResponseIndex);

/**
 * 
 */
UCLASS()
class DARKAGE_API UDAResponseOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ResponseButton;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	int32 ResponseIndex;

	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnResponseOptionClicked OnResponseOptionClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();
	
};