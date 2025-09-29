#include "Components/DAInteractionComponent.h"
#include "Interfaces/DAInteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UDAInteractionComponent::UDAInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    InteractionDistance = 200.0f;
    FocusedActor = nullptr;
}

void UDAInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UDAInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    FindInteractable();
}

void UDAInteractionComponent::FindInteractable()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    if (!PlayerController) return;

    FVector Start;
    FRotator Rotation;
    PlayerController->GetPlayerViewPoint(Start, Rotation);
    FVector End = Start + Rotation.Vector() * InteractionDistance;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->Implements<UDAInteractableInterface>())
        {
            if (HitActor != FocusedActor)
            {
                if (FocusedActor)
                {
                    IDAInteractableInterface::Execute_EndFocus(FocusedActor, GetOwner());
                }
                IDAInteractableInterface::Execute_BeginFocus(HitActor, GetOwner());
                FocusedActor = HitActor;
            }
        }
        else if (FocusedActor)
        {
            IDAInteractableInterface::Execute_EndFocus(FocusedActor, GetOwner());
            FocusedActor = nullptr;
        }
    }
    else if (FocusedActor)
    {
        IDAInteractableInterface::Execute_EndFocus(FocusedActor, GetOwner());
        FocusedActor = nullptr;
    }
}

void UDAInteractionComponent::Interact()
{
    if (FocusedActor)
    {
        IDAInteractableInterface::Execute_OnInteract(FocusedActor, GetOwner());
    }
}