#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DAInteractionComponent.generated.h"

class IDAInteractableInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UDAInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDAInteractionComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void Interact();

protected:
    virtual void BeginPlay() override;

private:
    void FindInteractable();

    UPROPERTY(EditAnywhere, Category = "Interaction")
    float InteractionDistance;

    UPROPERTY()
    AActor* FocusedActor;
};