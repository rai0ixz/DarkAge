// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseClass/DACharacter.h"
#include "Components/InventoryComponent.h" // Assuming path based on your structure
#include "Components/StatlineComponent.h" // Assuming path based on your structure

// Sets default values
ADACharacter::ADACharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	StatlineComponent = CreateDefaultSubobject<UStatlineComponent>(TEXT("StatlineComponent"));
}

// Called when the game starts or when spawned
void ADACharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}