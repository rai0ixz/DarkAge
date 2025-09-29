// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/MultiplayerSocialSubsystem.h"

void UMultiplayerSocialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("UMultiplayerSocialSubsystem::Initialize"));

	// Example: Bind to session delegates
	// FCoreDelegates::OnSessionCreated.AddUObject(this, &UMultiplayerSocialSubsystem::OnSessionCreated);
}

void UMultiplayerSocialSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Log, TEXT("UMultiplayerSocialSubsystem::Deinitialize"));

	// Example: Clean up any bound delegates
	// FCoreDelegates::OnSessionCreated.RemoveAll(this);

	Super::Deinitialize();
}