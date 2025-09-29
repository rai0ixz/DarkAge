#include "Core/NetworkManagerSubsystem.h"
#include "Components/StatlineComponent.h"
#include "Components/DAPlayerStateComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"

UNetworkManagerSubsystem::UNetworkManagerSubsystem()
    : MaxPlayersPerArea(50)
    , bNetworkLoggingEnabled(false)
    , bUpdateBatchingEnabled(true)
    , UpdateBatchSize(10)
    , bCompressionEnabled(true)
    , CompressionType(ENetworkCompressionType::Adaptive)
{
}

void UNetworkManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitializeDefaultSettings();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            NetworkUpdateTimerHandle,
            this,
            &UNetworkManagerSubsystem::ProcessNetworkUpdates,
            0.1f,
            true
        );
    }

    UE_LOG(LogTemp, Log, TEXT("NetworkManagerSubsystem initialized"));
}

void UNetworkManagerSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(NetworkUpdateTimerHandle);
    }

    RegisteredComponents.Empty();
    PlayerLatencyMap.Empty();
    PerformanceMetrics.Empty();

    UE_LOG(LogTemp, Log, TEXT("NetworkManagerSubsystem deinitialized"));

    Super::Deinitialize();
}

void UNetworkManagerSubsystem::RegisterNetworkComponent(UActorComponent* Component, ENetworkUpdateFrequency UpdateFrequency)
{
    if (!Component)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to register null component"));
        return;
    }

    RegisteredComponents.Add(Component, UpdateFrequency);

    UE_LOG(LogTemp, Log, TEXT("Registered component %s for network management with frequency %d"),
        *Component->GetName(), (int32)UpdateFrequency);
}

void UNetworkManagerSubsystem::UnregisterNetworkComponent(UActorComponent* Component)
{
    if (!Component)
    {
        return;
    }

    RegisteredComponents.Remove(Component);

    UE_LOG(LogTemp, Log, TEXT("Unregistered component %s from network management"),
        *Component->GetName());
}

bool UNetworkManagerSubsystem::IsComponentRegistered(UActorComponent* Component) const
{
    return RegisteredComponents.Contains(Component);
}

void UNetworkManagerSubsystem::ForceSynchronizeAll()
{
    for (auto& ComponentPair : RegisteredComponents)
    {
        if (UActorComponent* Component = ComponentPair.Key.Get())
        {
            SynchronizeComponent(Component);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Force synchronized %d network components"), RegisteredComponents.Num());
}

void UNetworkManagerSubsystem::SynchronizeComponent(UActorComponent* Component)
{
    if (!Component)
    {
        return;
    }

    if (UStatlineComponent* StatlineComp = Cast<UStatlineComponent>(Component))
    {
        HandleStatlineComponentUpdate(StatlineComp);
    }
    else if (UDAPlayerStateComponent* PlayerStateComp = Cast<UDAPlayerStateComponent>(Component))
    {
        HandlePlayerStateComponentUpdate(PlayerStateComp);
    }
}

float UNetworkManagerSubsystem::GetCurrentSyncLoad() const
{
    float TotalLoad = 0.0f;

    for (const auto& ComponentPair : RegisteredComponents)
    {
        if (ComponentPair.Key.IsValid())
        {
            float FrequencyMultiplier = UpdateFrequencyMultipliers.FindRef(ComponentPair.Value);
            TotalLoad += FrequencyMultiplier;
        }
    }

    return TotalLoad;
}

FNetworkLatencyData UNetworkManagerSubsystem::GetPlayerLatencyData(APlayerController* PlayerController) const
{
    if (const FNetworkLatencyData* LatencyData = PlayerLatencyMap.Find(PlayerController))
    {
        return *LatencyData;
    }

    return FNetworkLatencyData();
}

FVector UNetworkManagerSubsystem::PredictPlayerPosition(APawn* PlayerPawn, float DeltaTime) const
{
    if (!PlayerPawn)
    {
        return FVector::ZeroVector;
    }

    FVector CurrentLocation = PlayerPawn->GetActorLocation();
    FVector CurrentVelocity = PlayerPawn->GetVelocity();

    APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
    if (PC)
    {
        FNetworkLatencyData LatencyData = GetPlayerLatencyData(PC);
        float PredictionTime = LatencyData.CurrentPing * 0.001f;

        return CurrentLocation + (CurrentVelocity * PredictionTime);
    }

    return CurrentLocation + (CurrentVelocity * DeltaTime);
}

void UNetworkManagerSubsystem::ApplyLatencyCompensation(APlayerController* PlayerController, float CompensationAmount)
{
    if (!PlayerController)
    {
        return;
    }

    UE_LOG(LogTemp, VeryVerbose, TEXT("Applied latency compensation of %f ms to player %s"),
        CompensationAmount, *PlayerController->GetName());
}

bool UNetworkManagerSubsystem::ValidatePlayerAction(APlayerController* PlayerController, const FString& ActionType, const TMap<FString, float>& ActionData)
{
    if (!PlayerController)
    {
        return false;
    }

    if (!ValidateActionRate(PlayerController, ActionType, 0.1f))
    {
        ReportSuspiciousActivity(PlayerController, TEXT("Action Rate Limit Exceeded"), 0.6f);
        return false;
    }

    if (ActionType == TEXT("Movement"))
    {
        const float* Speed = ActionData.Find(TEXT("Speed"));
        const float* PosX = ActionData.Find(TEXT("PosX"));
        const float* PosY = ActionData.Find(TEXT("PosY"));
        const float* PosZ = ActionData.Find(TEXT("PosZ"));
        const float* DeltaTime = ActionData.Find(TEXT("DeltaTime"));

        if (Speed && *Speed > 1000.0f)
        {
            ReportSuspiciousActivity(PlayerController, TEXT("Excessive Movement Speed"), 0.8f);
            return false;
        }

        if (PosX && PosY && PosZ && DeltaTime)
        {
            FVector NewPosition(*PosX, *PosY, *PosZ);
            if (!ValidatePlayerMovement(PlayerController, NewPosition, *DeltaTime))
            {
                ReportSuspiciousActivity(PlayerController, TEXT("Impossible Movement/Teleportation"), 0.95f);
                return false;
            }
            UpdatePlayerPositionTracking(PlayerController, NewPosition);
        }
    }

    if (ActionType == TEXT("StaminaAction"))
    {
        const float* StaminaCost = ActionData.Find(TEXT("Cost"));
        const float* CurrentStamina = ActionData.Find(TEXT("Current"));
        const float* MaxStamina = ActionData.Find(TEXT("Max"));

        if (StaminaCost && CurrentStamina && *StaminaCost > *CurrentStamina)
        {
            UE_LOG(LogTemp, Warning, TEXT("Player %s attempted action with insufficient stamina"),
                *PlayerController->GetName());
            return false;
        }

        if (CurrentStamina && MaxStamina && *CurrentStamina > *MaxStamina * 1.1f)
        {
            ReportSuspiciousActivity(PlayerController, TEXT("Impossible Stamina Value"), 0.9f);
            return false;
        }
    }

    if (ActionType == TEXT("HealthAction"))
    {
        const float* HealthDelta = ActionData.Find(TEXT("Delta"));
        const float* CurrentHealth = ActionData.Find(TEXT("Current"));
        const float* MaxHealth = ActionData.Find(TEXT("Max"));

        if (CurrentHealth && MaxHealth && *CurrentHealth > *MaxHealth * 1.1f)
        {
            ReportSuspiciousActivity(PlayerController, TEXT("Impossible Health Value"), 0.9f);
            return false;
        }

        if (HealthDelta && *HealthDelta > 0.0f && MaxHealth && *HealthDelta > *MaxHealth * 0.5f)
        {
            ReportSuspiciousActivity(PlayerController, TEXT("Excessive Healing Amount"), 0.7f);
            return false;
        }
    }

    return true;
}

void UNetworkManagerSubsystem::ReportSuspiciousActivity(APlayerController* PlayerController, const FString& ActivityType, float SeverityLevel)
{
    if (!PlayerController)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Suspicious activity detected - Player: %s, Type: %s, Severity: %f"),
        *PlayerController->GetName(), *ActivityType, SeverityLevel);

    OnSuspiciousActivityDetected.Broadcast(PlayerController, ActivityType);

    if (SeverityLevel > 0.9f)
    {
        UE_LOG(LogTemp, Error, TEXT("High severity suspicious activity from player %s"),
            *PlayerController->GetName());
    }
}

void UNetworkManagerSubsystem::SetMaxPlayersPerArea(int32 MaxPlayers)
{
    MaxPlayersPerArea = FMath::Max(1, MaxPlayers);
    UE_LOG(LogTemp, Log, TEXT("Set max players per area to %d"), MaxPlayersPerArea);
}

int32 UNetworkManagerSubsystem::GetPlayersInArea(const FVector& AreaCenter, float AreaRadius) const
{
    int32 PlayerCount = 0;

    if (UWorld* World = GetGameInstance()->GetWorld())
    {
        for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            if (APlayerController* PC = Iterator->Get())
            {
                if (APawn* PlayerPawn = PC->GetPawn())
                {
                    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), AreaCenter);
                    if (Distance <= AreaRadius)
                    {
                        PlayerCount++;
                    }
                }
            }
        }
    }

    return PlayerCount;
}

void UNetworkManagerSubsystem::OptimizeNetworkUpdates()
{
    for (auto& ComponentPair : RegisteredComponents)
    {
        if (UActorComponent* Component = ComponentPair.Key.Get())
        {
            ENetworkUpdateFrequency OptimalFrequency = DetermineOptimalUpdateFrequency(Component);
            ComponentPair.Value = OptimalFrequency;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Optimized network updates for %d components"), RegisteredComponents.Num());
}

TMap<FString, float> UNetworkManagerSubsystem::GetNetworkPerformanceStats() const
{
    // Convert from FName to FString for the return value
    TMap<FString, float> StringMetrics;
    for (const auto& Pair : PerformanceMetrics)
    {
        StringMetrics.Add(Pair.Key.ToString(), Pair.Value);
    }
    return StringMetrics;
}

float UNetworkManagerSubsystem::GetCurrentBandwidthUsage() const
{
    return PerformanceMetrics.FindRef(TEXT("BandwidthUsage"));
}

void UNetworkManagerSubsystem::SetNetworkLoggingEnabled(bool bEnabled)
{
    bNetworkLoggingEnabled = bEnabled;
    UE_LOG(LogTemp, Log, TEXT("Network logging %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UNetworkManagerSubsystem::SetUpdateBatchingEnabled(bool bEnabled)
{
    bUpdateBatchingEnabled = bEnabled;
    UE_LOG(LogTemp, Log, TEXT("Update batching %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UNetworkManagerSubsystem::SetUpdateBatchSize(int32 BatchSize)
{
    UpdateBatchSize = FMath::Max(1, BatchSize);
    UE_LOG(LogTemp, Log, TEXT("Update batch size set to %d"), UpdateBatchSize);
}

void UNetworkManagerSubsystem::SetCompressionEnabled(bool bEnabled, ENetworkCompressionType NewCompressionType)
{
    bCompressionEnabled = bEnabled;
    CompressionType = NewCompressionType;

    UE_LOG(LogTemp, Log, TEXT("Network compression %s with type %d"),
        bEnabled ? TEXT("enabled") : TEXT("disabled"), (int32)NewCompressionType);
}

TMap<FString, float> UNetworkManagerSubsystem::GetCompressionStats() const
{
    return CompressionStats;
}

void UNetworkManagerSubsystem::ProcessNetworkUpdates()
{
    UpdatePlayerLatencyData();

    if (bUpdateBatchingEnabled)
    {
        ProcessBatchedUpdates();
    }

    CalculateNetworkOptimization();

    static float LastCleanupTime = 0.0f;
    if (UWorld* World = GetWorld())
    {
        float CurrentTime = World->GetTimeSeconds();
        if (CurrentTime - LastCleanupTime > 30.0f)
        {
            CleanupOldActionTimestamps();
            LastCleanupTime = CurrentTime;
        }
    }
    if (bNetworkLoggingEnabled)
    {
        LogNetworkPerformance();
    }
}

void UNetworkManagerSubsystem::UpdatePlayerLatencyData()
{
    if (UWorld* World = GetGameInstance()->GetWorld())
    {
        for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            if (APlayerController* PC = Iterator->Get())
            {
                FNetworkLatencyData& LatencyData = PlayerLatencyMap.FindOrAdd(PC);

                if (UNetConnection* NetConnection = PC->GetNetConnection())
                {
                    float OldPing = LatencyData.CurrentPing;
                    LatencyData.CurrentPing = NetConnection->AvgLag * 1000.0f;

                    LatencyData.AveragePing = (LatencyData.AveragePing * 0.9f) + (LatencyData.CurrentPing * 0.1f);

                    LatencyData.JitterMs = FMath::Abs(LatencyData.CurrentPing - OldPing);

                    LatencyData.PacketLoss = NetConnection->InPacketsLost / FMath::Max(1.0f, (float)NetConnection->InPackets);

                    if (FMath::Abs(LatencyData.CurrentPing - OldPing) > 20.0f)
                    {
                        OnPlayerLatencyChanged.Broadcast(PC, LatencyData);
                    }
                }
            }
        }
    }
}

void UNetworkManagerSubsystem::CalculateNetworkOptimization()
{
    float SyncLoad = GetCurrentSyncLoad();

    PerformanceMetrics.Add(TEXT("SyncLoad"), SyncLoad);
    PerformanceMetrics.Add(TEXT("RegisteredComponents"), RegisteredComponents.Num());
    PerformanceMetrics.Add(TEXT("ActivePlayers"), PlayerLatencyMap.Num());

    float BandwidthEstimate = SyncLoad * PlayerLatencyMap.Num() * 0.1f;
    PerformanceMetrics.Add(TEXT("BandwidthUsage"), BandwidthEstimate);

    if (SyncLoad > 100.0f)
    {
        OnNetworkPerformanceDegraded.Broadcast(SyncLoad);
    }
}

void UNetworkManagerSubsystem::LogNetworkPerformance()
{
    float SyncLoad = GetCurrentSyncLoad();
    float BandwidthUsage = GetCurrentBandwidthUsage();

    UE_LOG(LogTemp, Log, TEXT("Network Performance - Sync Load: %f, Bandwidth: %f KB/s, Players: %d"),
        SyncLoad, BandwidthUsage, PlayerLatencyMap.Num());
}

void UNetworkManagerSubsystem::InitializeDefaultSettings()
{
    UpdateFrequencyMultipliers.Add(ENetworkUpdateFrequency::Low, 1.0f);
    UpdateFrequencyMultipliers.Add(ENetworkUpdateFrequency::Medium, 5.0f);
    UpdateFrequencyMultipliers.Add(ENetworkUpdateFrequency::High, 10.0f);
    UpdateFrequencyMultipliers.Add(ENetworkUpdateFrequency::VeryHigh, 20.0f);
    UpdateFrequencyMultipliers.Add(ENetworkUpdateFrequency::Critical, 30.0f);

    PerformanceMetrics.Add(TEXT("SyncLoad"), 0.0f);
    PerformanceMetrics.Add(TEXT("BandwidthUsage"), 0.0f);
    PerformanceMetrics.Add(TEXT("RegisteredComponents"), 0.0f);
    PerformanceMetrics.Add(TEXT("ActivePlayers"), 0.0f);

    CompressionStats.Add(TEXT("CompressionRatio"), 1.0f);
    CompressionStats.Add(TEXT("UncompressedBytes"), 0.0f);
    CompressionStats.Add(TEXT("CompressedBytes"), 0.0f);
    CompressionStats.Add(TEXT("CompressionEfficiency"), 0.0f);
}

void UNetworkManagerSubsystem::ProcessBatchedUpdates()
{
    TArray<UActorComponent*> ComponentsToUpdate;

    for (auto& ComponentPair : RegisteredComponents)
    {
        if (UActorComponent* Component = ComponentPair.Key.Get())
        {
            ComponentsToUpdate.Add(Component);
        }
    }

    for (int32 i = 0; i < ComponentsToUpdate.Num(); i += UpdateBatchSize)
    {
        int32 BatchEnd = FMath::Min(i + UpdateBatchSize, ComponentsToUpdate.Num());

        for (int32 j = i; j < BatchEnd; j++)
        {
            SynchronizeComponent(ComponentsToUpdate[j]);
        }
    }
}

bool UNetworkManagerSubsystem::CompressUpdateData(const TArray<uint8>& UncompressedData, TArray<uint8>& CompressedData) const
{
    if (!bCompressionEnabled || UncompressedData.Num() == 0)
    {
        CompressedData = UncompressedData;
        return false;
    }

    switch (CompressionType)
    {
    case ENetworkCompressionType::Quantized:
        CompressedData = UncompressedData;
        CompressedData.SetNum(UncompressedData.Num() * 0.8f);
        break;

    case ENetworkCompressionType::Delta:
        CompressedData = UncompressedData;
        CompressedData.SetNum(UncompressedData.Num() * 0.6f);
        break;

    case ENetworkCompressionType::Adaptive:
        CompressedData = UncompressedData;
        CompressedData.SetNum(UncompressedData.Num() * 0.7f);
        break;

    default:
        CompressedData = UncompressedData;
        return false;
    }

    return true;
}

void UNetworkManagerSubsystem::UpdateCompressionStats(int32 UncompressedSize, int32 CompressedSize)
{
    if (UncompressedSize <= 0)
    {
        return;
    }

    float CompressionRatio = (float)CompressedSize / UncompressedSize;
    float CompressionEfficiency = (1.0f - CompressionRatio) * 100.0f;

    float CurrentUncompressed = CompressionStats.FindRef(TEXT("UncompressedBytes"));
    float CurrentCompressed = CompressionStats.FindRef(TEXT("CompressedBytes"));

    CompressionStats.Add(TEXT("UncompressedBytes"), CurrentUncompressed + UncompressedSize);
    CompressionStats.Add(TEXT("CompressedBytes"), CurrentCompressed + CompressedSize);
    CompressionStats.Add(TEXT("CompressionRatio"), CompressionRatio);
    CompressionStats.Add(TEXT("CompressionEfficiency"), CompressionEfficiency);
}

void UNetworkManagerSubsystem::HandleStatlineComponentUpdate(UStatlineComponent* StatlineComponent)
{
    if (!StatlineComponent)
    {
        return;
    }

    if (StatlineComponent->GetStatPercentage(FName("Health")) < 0.2f)
    {
        if (AActor* Owner = StatlineComponent->GetOwner())
        {
            Owner->ForceNetUpdate();
        }
    }
}

void UNetworkManagerSubsystem::HandlePlayerStateComponentUpdate(UDAPlayerStateComponent* PlayerStateComponent)
{
    if (!PlayerStateComponent)
    {
        return;
    }
}

ENetworkUpdateFrequency UNetworkManagerSubsystem::DetermineOptimalUpdateFrequency(UActorComponent* Component) const
{
    if (!Component)
    {
        return ENetworkUpdateFrequency::Low;
    }

    if (UStatlineComponent* StatlineComp = Cast<UStatlineComponent>(Component))
    {
        if (StatlineComp->GetStatPercentage(FName("Health")) < 0.5f)
        {
            return ENetworkUpdateFrequency::Critical;
        }
        return ENetworkUpdateFrequency::High;
    }
    else if (UDAPlayerStateComponent* PlayerStateComp = Cast<UDAPlayerStateComponent>(Component))
    {
        return ENetworkUpdateFrequency::Medium;
    }

    return ENetworkUpdateFrequency::Low;
}

bool UNetworkManagerSubsystem::ValidatePlayerMovement(APlayerController* PlayerController, const FVector& NewPosition, float DeltaTime)
{
    if (!PlayerController || DeltaTime <= 0.0f)
    {
        return false;
    }

    FVector* LastPosition = LastKnownPlayerPositions.Find(PlayerController);
    if (!LastPosition)
    {
        return true;
    }

    float Distance = FVector::Dist(*LastPosition, NewPosition);
    float Speed = Distance / DeltaTime;

    if (!IsMovementSpeedValid(*LastPosition, NewPosition, DeltaTime, 1200.0f))
    {
        return false;
    }

    if (Distance > 2000.0f && DeltaTime < 0.1f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Possible teleportation detected for player %s: Distance=%f, DeltaTime=%f"),
            *PlayerController->GetName(), Distance, DeltaTime);
        return false;
    }

    return true;
}

bool UNetworkManagerSubsystem::IsMovementSpeedValid(const FVector& OldPosition, const FVector& NewPosition, float DeltaTime, float MaxAllowedSpeed)
{
    if (DeltaTime <= 0.0f)
    {
        return false;
    }

    float Distance = FVector::Dist(OldPosition, NewPosition);
    float Speed = Distance / DeltaTime;

    return Speed <= MaxAllowedSpeed;
}

bool UNetworkManagerSubsystem::ValidateActionRate(APlayerController* PlayerController, const FString& ActionType, float MinTimeBetweenActions)
{
    if (!PlayerController)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    float CurrentTime = World->GetTimeSeconds();

    TArray<float>& ActionTimestamps = PlayerActionTimestamps.FindOrAdd(PlayerController);

    ActionTimestamps.RemoveAll([CurrentTime](float Timestamp) {
        return CurrentTime - Timestamp > 10.0f;
        });

    if (ActionTimestamps.Num() > 0)
    {
        float LastActionTime = ActionTimestamps.Last();
        if (CurrentTime - LastActionTime < MinTimeBetweenActions)
        {
            UE_LOG(LogTemp, Warning, TEXT("Action rate limit exceeded for player %s. Action: %s"),
                *PlayerController->GetName(), *ActionType);
            return false;
        }
    }

    ActionTimestamps.Add(CurrentTime);

    if (ActionTimestamps.Num() > 100)
    {
        ActionTimestamps.RemoveAt(0, ActionTimestamps.Num() - 100);
    }

    return true;
}

void UNetworkManagerSubsystem::UpdatePlayerPositionTracking(APlayerController* PlayerController, const FVector& NewPosition)
{
    if (!PlayerController)
    {
        return;
    }

    LastKnownPlayerPositions.FindOrAdd(PlayerController) = NewPosition;
    if (UWorld* World = GetWorld())
    {
        LastPositionUpdateTimes.FindOrAdd(PlayerController) = World->GetTimeSeconds();
    }
}

void UNetworkManagerSubsystem::CleanupOldActionTimestamps(float MaxAge)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();

    TArray<TWeakObjectPtr<APlayerController>> PlayersToRemove;

    for (auto& Pair : LastPositionUpdateTimes)
    {
        if (!Pair.Key.IsValid() || CurrentTime - Pair.Value > MaxAge)
        {
            PlayersToRemove.Add(Pair.Key);
        }
    }

    for (auto& Player : PlayersToRemove)
    {
        LastKnownPlayerPositions.Remove(Player);
        LastPositionUpdateTimes.Remove(Player);
        PlayerActionTimestamps.Remove(Player);
    }
}