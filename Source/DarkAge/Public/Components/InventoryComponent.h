#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DarkAge/Public/Data/ItemData.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FItemData Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Quantity = 0;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlot> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Currency;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FItemData& Item, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanAfford(int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveCurrency(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(const FItemData& Item, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const FItemData& Item, int32 Quantity) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItemBySlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintInventory();

	UFUNCTION()
	void OnRep_Items();
};