#pragma once

#include "CoreMinimal.h"
#include "Characters/DABaseNPC.h"
#include "Interfaces/DAInteractableInterface.h"
#include "DALandownerNPC.generated.h"

UCLASS()
class DARKAGE_API ADALandownerNPC : public ADABaseNPC, public IDAInteractableInterface
{
	GENERATED_BODY()

public:
	ADALandownerNPC();

	//~ Begin IDAInteractableInterface
	virtual void BeginFocus_Implementation(AActor* InteractingActor) override;
	virtual void EndFocus_Implementation(AActor* InteractingActor) override;
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	//~ End IDAInteractableInterface

protected:

	/* The list of items this vendor is willing to buy from the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	TArray<FName> BuyableItemIDs;

	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void BeginTrade(APlayerController* PlayerController);

private:
	UPROPERTY()
	class UDAUserInterface* TradeWidget;
};