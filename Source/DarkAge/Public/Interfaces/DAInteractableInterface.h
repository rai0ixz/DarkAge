#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "DAInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDAInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with by the player.
 */
class DARKAGE_API IDAInteractableInterface
{
	GENERATED_BODY()

public:

	/** Called when the player's interaction trace begins focusing on this object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void BeginFocus(AActor* InteractingActor);

	/** Called when the player's interaction trace stops focusing on this object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus(AActor* InteractingActor);

	/** Called when the player presses the interact key while focusing on this object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* InteractingActor);
};