// Copyright RaioCore. All Rights Reserved.

#include "Characters/DABaseNPC.h"
#include "Components/DialogueComponent.h"
#include "Components/InventoryComponent.h"

ADABaseNPC::ADABaseNPC()
{
    PrimaryActorTick.bCanEverTick = true;

    DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void ADABaseNPC::BeginPlay()
{
    Super::BeginPlay();
}