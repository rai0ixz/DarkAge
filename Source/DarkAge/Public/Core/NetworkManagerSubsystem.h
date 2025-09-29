#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "NetworkManagerSubsystem.generated.h"

// Forward declarations for components
class UStatlineComponent;
class UDAPlayerStateComponent;

UENUM(BlueprintType)
enum class ENetworkUpdateFrequency : uint8
{
    Low,
    Medium,
    High,
    VeryHigh,
    Critical
};

UENUM(BlueprintType)
enum class ENetworkCompressionType : uint8
{
    None,
    Quantized,
    Delta,
    Adaptive
};

USTRUCT(BlueprintType)
struct DARKAGE_API FNetworkLatencyData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Network Latency")
    float CurrentPing = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Latency")
    float AveragePing = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Latency")
    float JitterMs = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Latency")
    float PacketLoss = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkPerformanceDegraded, float, CurrentLoad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerLatencyChanged, APlayerController*, PlayerController, const FNetworkLatencyData&, NewLatencyData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSuspiciousActivityDetected, APlayerController*, PlayerController, const FString&, ActivityType);

UCLASS(Config = Game, DefaultConfig)
class DARKAGE_API UNetworkManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UNetworkManagerSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Network Manager")
    void RegisterNetworkComponent(UActorComponent* Component, ENetworkUpdateFrequency UpdateFrequency = ENetworkUpdateFrequency::Medium);

    UFUNCTION(BlueprintCallable, Category = "Network Manager")
    void UnregisterNetworkComponent(UActorComponent* Component);

    UFUNCTION(BlueprintPure, Category = "Network Manager")
    bool IsComponentRegistered(UActorComponent* Component) const;

    UFUNCTION(BlueprintCallable, Category = "Network Manager")
    void ForceSynchronizeAll();

    UFUNCTION(BlueprintCallable, Category = "Network Manager")
    void SynchronizeComponent(UActorComponent* Component);

    UFUNCTION(BlueprintPure, Category = "Network Manager|Performance")
    float GetCurrentSyncLoad() const;

    UFUNCTION(BlueprintPure, Category = "Network Manager|Performance")
    FNetworkLatencyData GetPlayerLatencyData(APlayerController* PlayerController) const;

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Anti-Cheat")
    FVector PredictPlayerPosition(APawn* PlayerPawn, float DeltaTime) const;

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Anti-Cheat")
    void ApplyLatencyCompensation(APlayerController* PlayerController, float CompensationAmount);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Anti-Cheat")
    bool ValidatePlayerAction(APlayerController* PlayerController, const FString& ActionType, const TMap<FString, float>& ActionData);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Anti-Cheat")
    void ReportSuspiciousActivity(APlayerController* PlayerController, const FString& ActivityType, float SeverityLevel);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Zoning")
    void SetMaxPlayersPerArea(int32 MaxPlayers);

    UFUNCTION(BlueprintPure, Category = "Network Manager|Zoning")
    int32 GetPlayersInArea(const FVector& AreaCenter, float AreaRadius) const;

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Optimization")
    void OptimizeNetworkUpdates();

    UFUNCTION(BlueprintPure, Category = "Network Manager|Performance")
    TMap<FString, float> GetNetworkPerformanceStats() const;

    UFUNCTION(BlueprintPure, Category = "Network Manager|Performance")
    float GetCurrentBandwidthUsage() const;

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Settings")
    void SetNetworkLoggingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Settings")
    void SetUpdateBatchingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Settings")
    void SetUpdateBatchSize(int32 BatchSize);

    UFUNCTION(BlueprintCallable, Category = "Network Manager|Settings")
    void SetCompressionEnabled(bool bEnabled, ENetworkCompressionType NewCompressionType);

    UFUNCTION(BlueprintPure, Category = "Network Manager|Performance")
    TMap<FString, float> GetCompressionStats() const;

public:
    UPROPERTY(BlueprintAssignable, Category = "Network Manager|Events")
    FOnNetworkPerformanceDegraded OnNetworkPerformanceDegraded;

    UPROPERTY(BlueprintAssignable, Category = "Network Manager|Events")
    FOnPlayerLatencyChanged OnPlayerLatencyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Network Manager|Events")
    FOnSuspiciousActivityDetected OnSuspiciousActivityDetected;

protected:
    UPROPERTY(Config)
    int32 MaxPlayersPerArea;

    UPROPERTY(Config)
    bool bNetworkLoggingEnabled;

    UPROPERTY(Config)
    bool bUpdateBatchingEnabled;

    UPROPERTY(Config)
    int32 UpdateBatchSize;

    UPROPERTY(Config)
    bool bCompressionEnabled;

    UPROPERTY(Config)
    ENetworkCompressionType CompressionType;

private:
    FTimerHandle NetworkUpdateTimerHandle;

    TMap<TWeakObjectPtr<UActorComponent>, ENetworkUpdateFrequency> RegisteredComponents;
    TMap<TWeakObjectPtr<APlayerController>, FNetworkLatencyData> PlayerLatencyMap;
    TMap<FName, float> PerformanceMetrics;
    TMap<ENetworkUpdateFrequency, float> UpdateFrequencyMultipliers;
    TMap<FString, float> CompressionStats;

    TMap<TWeakObjectPtr<APlayerController>, FVector> LastKnownPlayerPositions;
    TMap<TWeakObjectPtr<APlayerController>, float> LastPositionUpdateTimes;
    TMap<TWeakObjectPtr<APlayerController>, TArray<float>> PlayerActionTimestamps;

    void ProcessNetworkUpdates();
    void UpdatePlayerLatencyData();
    void CalculateNetworkOptimization();
    void LogNetworkPerformance();
    void InitializeDefaultSettings();
    void ProcessBatchedUpdates();
    bool CompressUpdateData(const TArray<uint8>& UncompressedData, TArray<uint8>& CompressedData) const;
    void UpdateCompressionStats(int32 UncompressedSize, int32 CompressedSize);
    void HandleStatlineComponentUpdate(UStatlineComponent* StatlineComponent);
    void HandlePlayerStateComponentUpdate(UDAPlayerStateComponent* PlayerStateComponent);
    ENetworkUpdateFrequency DetermineOptimalUpdateFrequency(UActorComponent* Component) const;

    bool ValidatePlayerMovement(APlayerController* PlayerController, const FVector& NewPosition, float DeltaTime);
    bool IsMovementSpeedValid(const FVector& OldPosition, const FVector& NewPosition, float DeltaTime, float MaxAllowedSpeed);
    bool ValidateActionRate(APlayerController* PlayerController, const FString& ActionType, float MinTimeBetweenActions = 0.1f);
    void UpdatePlayerPositionTracking(APlayerController* PlayerController, const FVector& NewPosition);
    void CleanupOldActionTimestamps(float MaxAge = 60.0f);
};