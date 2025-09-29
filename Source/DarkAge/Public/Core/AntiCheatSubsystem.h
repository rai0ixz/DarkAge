#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/TimerHandle.h"
#include "AntiCheatSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECheatType : uint8
{
    Unknown,
    SpeedHack,
    MemoryModification,
    ProcessInjection,
    NetworkManipulation,
    InputBot,
    AssetModification,
    InvalidInput,
    Teleportation,
    StatisticalAnomaly,
    ProcessIntegrity
};

UENUM(BlueprintType)
enum class ECheatSeverity : uint8
{
    Low,
    Medium,

    High,
    Critical
};

UENUM(BlueprintType)
enum class EAntiCheatAction : uint8
{
    None,
    Flag,
    Kick,
    TempBan,
    PermanentBan
};

USTRUCT(BlueprintType)
struct FCheatDetection
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString PlayerId;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    ECheatType CheatType;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    ECheatSeverity Severity;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FDateTime DetectionTime;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float ConfidenceLevel;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    bool bProcessed;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FVector Location;


    FCheatDetection()
        : CheatType(ECheatType::Unknown)
        , Severity(ECheatSeverity::Low)
        , DetectionTime(FDateTime::Now())
        , ConfidenceLevel(0.0f)
        , bProcessed(false)
        , Location(FVector::ZeroVector)
    {
    }
};

USTRUCT(BlueprintType)
struct FPlayerBehaviorProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString PlayerId;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float TrustScore;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float AverageSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float MaxSpeedRecorded;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    int32 SuspiciousMovements;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    int32 TeleportEvents;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float AverageInputRate;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    float MaxInputRate;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    int32 ImpossibleInputSequences;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FDateTime FirstSeen;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FDateTime LastActivity;

    FPlayerBehaviorProfile()
        : TrustScore(100.0f)
        , AverageSpeed(0.0f)
        , MaxSpeedRecorded(0.0f)
        , SuspiciousMovements(0)
        , TeleportEvents(0)
        , AverageInputRate(0.0f)
        , MaxInputRate(0.0f)
        , ImpossibleInputSequences(0)
    {
    }
};

USTRUCT(BlueprintType)
struct FHardwareFingerprint
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString CPUId;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString MotherboardId;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString DiskId;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString NetworkMacAddress;

    UPROPERTY(BlueprintReadOnly, Category = "Anti-Cheat")
    FString ComputedHash;
};

/**
 * Anti-Cheat Subsystem
 * Real-time cheat detection and prevention system
 */
UCLASS(BlueprintType)
class DARKAGE_API UAntiCheatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UAntiCheatSubsystem();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Report a potential cheat detection */
    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void ReportCheatDetection(const FString& PlayerId, ECheatType CheatType, ECheatSeverity Severity, const FString& Description);

    /** Check if a player is flagged as suspicious */
    UFUNCTION(BlueprintPure, Category = "Anti-Cheat")
    bool IsPlayerSuspicious(const FString& PlayerId) const;

    /** Get detection statistics */
    UFUNCTION(BlueprintPure, Category = "Anti-Cheat")
    int32 GetTotalDetections() const;

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidatePlayerMovement(const FString& PlayerId, const FVector& NewLocation, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidatePlayerInput(const FString& PlayerId, const FString& InputData);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void ReportSuspiciousActivity(const FString& PlayerId, ECheatType CheatType, const FString& Description, float ConfidenceLevel);

    UFUNCTION(BlueprintPure, Category = "Anti-Cheat")
    float GetPlayerTrustScore(const FString& PlayerId);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    FString GenerateHardwareFingerprint();

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidateProcessIntegrity(const FString& PlayerId);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void BanPlayer(const FString& PlayerId, EAntiCheatAction Action, const FString& Reason, int32 Duration);

    UFUNCTION(BlueprintPure, Category = "Anti-Cheat")
    bool IsPlayerBanned(const FString& PlayerId);

protected:
    // Detection storage
    UPROPERTY()
    TArray<FCheatDetection> RecentDetections;

    UPROPERTY()
    TSet<FString> SuspiciousPlayers;

    UPROPERTY()
    TArray<FCheatDetection> PendingDetections;

    UPROPERTY()
    TMap<FString, FPlayerBehaviorProfile> PlayerProfiles;

    UPROPERTY()
    TMap<FString, FHardwareFingerprint> HardwareFingerprints;

    UPROPERTY()
    TMap<FString, FDateTime> BannedPlayers;

    // Timer handles for monitoring
    FTimerHandle BehaviorAnalysisTimer;
    FTimerHandle ProcessDetectionTimer;
    FTimerHandle MemoryCheckTimer;
    FTimerHandle CleanupTimer;

    // Monitoring intervals (in seconds)
    static constexpr float ANALYSIS_INTERVAL = 5.0f;
    static constexpr float DETECTION_PROCESS_INTERVAL = 1.0f;
    static constexpr float MEMORY_CHECK_INTERVAL = 10.0f;
    static constexpr float CLEANUP_INTERVAL = 300.0f; // 5 minutes
    static constexpr float MAX_MOVEMENT_SPEED = 1000.0f;
    static constexpr float TELEPORT_THRESHOLD = 5000.0f;
    static constexpr float HIGH_CONFIDENCE_THRESHOLD = 0.9f;
    static constexpr float LOW_TRUST_THRESHOLD = 20.0f;


    // Internal monitoring functions
    void AnalyzePlayerBehavior();
    void ProcessDetections();
    bool VerifyMemoryIntegrity();
    void CleanupOldData();
    bool CheckForInjectedDLLs();
    FString GetCPUId();
    FString GetMotherboardId();
    FString GetDiskId();
    FString GetNetworkMacAddress();
    FString ComputeFingerprintHash(const FHardwareFingerprint& Fingerprint);
    bool IsStatisticallyAnomalous(const FString& PlayerId, const FString& Metric, float Value);
    void ExecuteAntiCheatAction(const FString& PlayerId, EAntiCheatAction Action, const FString& Reason);
    void UpdatePlayerProfile(const FString& PlayerId);
};
