#pragma once

#include "CoreMinimal.h"
#include "Characters/DABaseNPC.h"
#include "Interfaces/DAInteractableInterface.h"
#include "DATavernKeeperNPC.generated.h"

UCLASS()
class DARKAGE_API ADATavernKeeperNPC : public ADABaseNPC, public IDAInteractableInterface
{
	GENERATED_BODY()

public:
	ADATavernKeeperNPC();

	//~ Begin IDAInteractableInterface
	virtual void BeginFocus_Implementation(AActor* InteractingActor) override;
	virtual void EndFocus_Implementation(AActor* InteractingActor) override;
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	//~ End IDAInteractableInterface
};