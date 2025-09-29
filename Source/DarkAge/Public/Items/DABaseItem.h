#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ItemData.h"
#include "DABaseItem.generated.h"

UCLASS()
class DARKAGE_API ADABaseItem : public AActor
{
    GENERATED_BODY()

public:
    ADABaseItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UDataTable* ItemDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID;

    UFUNCTION(BlueprintCallable, Category = "Item")
    void InitializeFromDataTable();

    UFUNCTION(BlueprintCallable, Category = "Item")
    const FItemData& GetItemData() const { return ItemData; }

    UFUNCTION(BlueprintImplementableEvent, Category = "Item")
    void OnUse(APawn* InstigatorPawn);

    UFUNCTION(BlueprintImplementableEvent, Category = "Item")
    void OnEquip(APawn* InstigatorPawn);

    UFUNCTION(BlueprintImplementableEvent, Category = "Item")
    void OnDrop(APawn* InstigatorPawn);

protected:
    virtual void BeginPlay() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // --- Knowledge System Integration ---
    // Principles this item is associated with (e.g., discovered during crafting/experiments)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knowledge")
    TArray<FName> KnownPrinciples;

private:
    void FetchItemData();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FItemData ItemData;
};