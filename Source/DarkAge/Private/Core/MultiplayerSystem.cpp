#include "Core/MultiplayerSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UMultiplayerSystem::UMultiplayerSystem()
	: bIsInSession(false)
	, CurrentMode(EMultiplayerMode::SinglePlayer)
	, MaxPlayers(4)
{
}

void UMultiplayerSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ConnectedPlayers.Reset();
	SharedQuests.Reset();
	PendingMessages.Reset();
	CurrentSessionName.Reset();
}

void UMultiplayerSystem::Deinitialize()
{
	ConnectedPlayers.Reset();
	SharedQuests.Reset();
	PendingMessages.Reset();
	Super::Deinitialize();
}

bool UMultiplayerSystem::CreateSession(const FString& SessionName, EMultiplayerMode Mode, int32 InMaxPlayers)
{
	if (bIsInSession)
	{
		return false;
	}
	CurrentSessionName = SessionName;
	CurrentMode = Mode;
	MaxPlayers = FMath::Max(1, InMaxPlayers);
	bIsInSession = true;
	ConnectedPlayers.Reset();
	return true;
}

bool UMultiplayerSystem::JoinSession(const FString& SessionName)
{
	// For now, allow join if a session name exists or matches
	if (!bIsInSession || CurrentSessionName == SessionName)
	{
		CurrentSessionName = SessionName;
		bIsInSession = true;
		return true;
	}
	return false;
}

void UMultiplayerSystem::LeaveSession()
{
	bIsInSession = false;
	CurrentSessionName.Reset();
	ConnectedPlayers.Reset();
	SharedQuests.Reset();
}

bool UMultiplayerSystem::AddPlayer(const FString& PlayerID, const FString& PlayerName)
{
	if (!bIsInSession)
	{
		return false;
	}

	if (ConnectedPlayers.Num() >= MaxPlayers)
	{
		return false;
	}

	if (FindPlayer(PlayerID) != nullptr)
	{
		return false; // already present
	}

	FPlayerSession NewPlayer;
	NewPlayer.PlayerID = PlayerID;
	NewPlayer.PlayerName = PlayerName;
	NewPlayer.Role = ConnectedPlayers.Num() == 0 ? EPlayerRole::Leader : EPlayerRole::Member;
	NewPlayer.Location = FVector::ZeroVector;
	NewPlayer.bIsConnected = true;
	NewPlayer.LastUpdateTime = 0.0f;

	ConnectedPlayers.Add(MoveTemp(NewPlayer));
	OnPlayerJoined.Broadcast(ConnectedPlayers.Last());

	return true;
}

bool UMultiplayerSystem::RemovePlayer(const FString& PlayerID)
{
	for (int32 i = 0; i < ConnectedPlayers.Num(); ++i)
	{
		if (ConnectedPlayers[i].PlayerID == PlayerID)
		{
			ConnectedPlayers.RemoveAt(i);
			OnPlayerLeft.Broadcast(PlayerID);
			return true;
		}
	}
	return false;
}

bool UMultiplayerSystem::CreateSharedQuest(const FString& QuestID, const FString& QuestTitle)
{
	if (!bIsInSession)
	{
		return false;
	}

	if (FindSharedQuest(QuestID) != nullptr)
	{
		return false;
	}

	FSharedQuest NewQuest;
	NewQuest.QuestID = QuestID;
	NewQuest.QuestTitle = QuestTitle;
	NewQuest.bIsActive = true;
	SharedQuests.Add(MoveTemp(NewQuest));
	return true;
}

bool UMultiplayerSystem::JoinSharedQuest(const FString& QuestID, const FString& PlayerID)
{
	if (FSharedQuest* Quest = FindSharedQuest(QuestID))
	{
		if (!Quest->ParticipatingPlayers.Contains(PlayerID))
		{
			Quest->ParticipatingPlayers.Add(PlayerID);
			Quest->PlayerProgress.FindOrAdd(PlayerID) = 0;
		}
		return true;
	}
	return false;
}

void UMultiplayerSystem::UpdateQuestProgress(const FString& QuestID, const FString& PlayerID, int32 Progress)
{
	if (FSharedQuest* Quest = FindSharedQuest(QuestID))
	{
		Quest->PlayerProgress.FindOrAdd(PlayerID) = Progress;
	}
}

void UMultiplayerSystem::SyncPlayerState(const FString& PlayerID, const FVector& Location)
{
	if (FPlayerSession* Player = FindPlayer(PlayerID))
	{
		Player->Location = Location;
		Player->LastUpdateTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	}
}

void UMultiplayerSystem::SyncWorldState()
{
	// Placeholder: world replication or snapshotting would occur here
}

void UMultiplayerSystem::BroadcastMessage(const FString& Message, const FString& SenderID)
{
	OnMessageReceived.Broadcast(Message, SenderID);
}

// --------- Private helpers ---------

FPlayerSession* UMultiplayerSystem::FindPlayer(const FString& PlayerID)
{
	for (FPlayerSession& P : ConnectedPlayers)
	{
		if (P.PlayerID == PlayerID)
		{
			return &P;
		}
	}
	return nullptr;
}

FSharedQuest* UMultiplayerSystem::FindSharedQuest(const FString& QuestID)
{
	for (FSharedQuest& Q : SharedQuests)
	{
		if (Q.QuestID == QuestID)
		{
			return &Q;
		}
	}
	return nullptr;
}