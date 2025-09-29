#pragma once

#include "CoreMinimal.h"
#include "UI/DAUserInterface.h"
#include "DAInventoryWidget.generated.h"

class UListView;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class DARKAGE_API UDAInventoryWidget : public UDAUserInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UListView> InventoryList;
	
private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
};
