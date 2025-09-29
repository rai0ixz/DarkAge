#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSystem.generated.h"

UENUM(BlueprintType)
enum class EMultiplayerMode : uint8
{
    SinglePlayer   UMETA(DisplayName = "Single Player"),
    LocalCoop      UMETA(DisplayName = "Local Co-op"),
    OnlineCoop     UMETA(DisplayName = "Online Co-op"),
    PvP            UMETA(DisplayName = "Player vs Player")
};

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    Leader         UMETA(DisplayName = "Party Leader"),
    Member         UMETA(DisplayName = "Party Member"),
    Observer       UMETA(DisplayName = "Observer")
};

USTRUCT(BlueprintType)
struct FPlayerSession
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString PlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    EPlayerRole Role;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    bool bIsConnected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    float LastUpdateTime;

    FPlayerSession()
    {
        Role = EPlayerRole::Member;
        bIsConnected = false;
        LastUpdateTime = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct FSharedQuest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> ParticipatingPlayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TMap<FString, int32> PlayerProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsActive;

    FSharedQuest()
    {
        bIsActive = false;
    }
};

UCLASS()
class DARKAGE_API UMultiplayerSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMultiplayerSystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Session management
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool CreateSession(const FString& SessionName, EMultiplayerMode Mode, int32 MaxPlayers = 4);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool JoinSession(const FString& SessionName);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void LeaveSession();

    UFUNCTION(BlueprintPure, Category = "Multiplayer")
    bool IsInSession() const { return bIsInSession; }

    // Player management
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool AddPlayer(const FString& PlayerID, const FString& PlayerName);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool RemovePlayer(const FString& PlayerID);

    UFUNCTION(BlueprintPure, Category = "Multiplayer")
    const TArray<FPlayerSession>& GetConnectedPlayers() const { return ConnectedPlayers; }

    // Shared content
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool CreateSharedQuest(const FString& QuestID, const FString& QuestTitle);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool JoinSharedQuest(const FString& QuestID, const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void UpdateQuestProgress(const FString& QuestID, const FString& PlayerID, int32 Progress);

    UFUNCTION(BlueprintPure, Category = "Multiplayer")
    const TArray<FSharedQuest>& GetSharedQuests() const { return SharedQuests; }

    // Synchronization
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void SyncPlayerState(const FString& PlayerID, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void SyncWorldState();

    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void BroadcastMessage(const FString& Message, const FString& SenderID);

    // Network events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, const FPlayerSession&, Player);
    UPROPERTY(BlueprintAssignable, Category = "Multiplayer Events")
    FOnPlayerJoined OnPlayerJoined;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, const FString&, PlayerID);
    UPROPERTY(BlueprintAssignable, Category = "Multiplayer Events")
    FOnPlayerLeft OnPlayerLeft;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMessageReceived, const FString&, Message, const FString&, SenderID);
    UPROPERTY(BlueprintAssignable, Category = "Multiplayer Events")
    FOnMessageReceived OnMessageReceived;

private:
    // Session data
    UPROPERTY()
    bool bIsInSession;

    UPROPERTY()
    FString CurrentSessionName;

    UPROPERTY()
    EMultiplayerMode CurrentMode;

    UPROPERTY()
    int32 MaxPlayers;

    // Player data
    UPROPERTY()
    TArray<FPlayerSession> ConnectedPlayers;

    // Shared content
    UPROPERTY()
    TArray<FSharedQuest> SharedQuests;

    // Network simulation (for single-player testing)
    UPROPERTY()
    TMap<FString, FString> PendingMessages;

    // Core functions
    void InitializeNetworkSimulation();
    void UpdatePlayerConnections(float DeltaTime);
    void ProcessPendingMessages();
    FPlayerSession* FindPlayer(const FString& PlayerID);
    FSharedQuest* FindSharedQuest(const FString& QuestID);
    void SimulateNetworkLatency();
    bool ValidateSessionSettings() const;
    void CleanupDisconnectedPlayers();
};