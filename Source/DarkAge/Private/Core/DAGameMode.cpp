
#include "Core/DAGameMode.h"        
#include "UObject/ConstructorHelpers.h" 
#include "GameFramework/Pawn.h"           


ADAGameMode::ADAGameMode()
{

}

void ADAGameMode::StartPlay()
{
	Super::StartPlay();

	bGameStarted = true;
	bGameEnded = false;
	bVictory = false;

	if (UWorld* World = GetWorld())
	{
		GameStartTime = World->GetTimeSeconds();
	}

	// Broadcast start
	OnGameStarted.Broadcast();
}

void ADAGameMode::RestartGame()
{
	// Basic restart for custom game mode
	bGameStarted = false;
	bGameEnded = false;
	bVictory = false;
}