#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DAGameMode.generated.h" // Make sure this is the EXACT name of your .h file

// Forward declarations
class ADAPlayerCharacter;
class UAdvancedSurvivalSubsystem;
class UDynamicQuestSubsystem;
class UFactionManagerSubsystem;
class UDAUIManager;

// Game progression states
UENUM(BlueprintType)
enum class EGameProgressionState : uint8
{
    Tutorial     UMETA(DisplayName = "Tutorial"),
    EarlyGame    UMETA(DisplayName = "Early Game"),
    MidGame      UMETA(DisplayName = "Mid Game"),
    LateGame     UMETA(DisplayName = "Late Game"),
    EndGame      UMETA(DisplayName = "End Game"),
    Victory      UMETA(DisplayName = "Victory"),
    Defeat       UMETA(DisplayName = "Defeat")
};

UCLASS()
class DARKAGE_API ADAGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADAGameMode();

    virtual void StartPlay() override;

    // Game state management
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void RestartGame();

    UFUNCTION(BlueprintPure, Category = "Game Mode")
    EGameProgressionState GetCurrentGameState() const { return CurrentGameState; }

    UFUNCTION(BlueprintPure, Category = "Game Mode")
    bool IsGameEnded() const { return bGameEnded; }

    UFUNCTION(BlueprintPure, Category = "Game Mode")
    bool IsVictory() const { return bVictory; }

    // Game events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameStarted OnGameStarted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameProgressionChanged, EGameProgressionState, NewState);
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameProgressionChanged OnGameProgressionChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameEnded, bool, bVictory, const FString&, EndMessage);
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameEnded OnGameEnded;

protected:
    // Game state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    EGameProgressionState CurrentGameState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    bool bGameStarted;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    bool bGameEnded;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    bool bVictory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    float GameStartTime;

    // Game progression thresholds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Progression")
    float EarlyGameThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Progression")
    float MidGameThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Progression")
    float LateGameThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Progression")
    float EndGameThreshold;

private:
    // Timer handles
    FTimerHandle GameLoopTimerHandle;
    FTimerHandle ProgressionTimerHandle;

    // Core functions
    void InitializeGameSystems();
    void StartGameLoop();
    void GameLoopUpdate();
    void CheckGameProgression();
    void HandleProgressionEvent(EGameProgressionState NewState);
    void CheckWinLoseConditions();
    void HandleDynamicEvents();
    void UpdateNPCBehaviors();
    void UpdateGameUI();

    // End game functions
    void EndGame(bool bVictory, const FString& EndMessage);
    void HandleVictory();
    void HandleDefeat();

    // Utility functions
    FString GetRandomEventType() const;
    FVector GetRandomEventLocation() const;
    void ShowGameMessage(const FString& Message);
};