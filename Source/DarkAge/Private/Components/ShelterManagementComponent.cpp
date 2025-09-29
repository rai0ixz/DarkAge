// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/ShelterManagementComponent.h"

// Sets default values for this component's properties
UShelterManagementComponent::UShelterManagementComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

// Called when the game starts
void UShelterManagementComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

// Called every frame
void UShelterManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}