#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SecurityAuditSubsystem.generated.h"

UENUM(BlueprintType)
enum class ESecurityEventType : uint8
{
    None = 0,
    AdminAction = 1,
    AdminLogin = 2,
    AdminLogout = 3,
    CheatDetection = 4,
    SuspiciousActivity = 5,
    UnauthorizedAccess = 6,
    SystemCompromise = 7,
    NetworkAnomaly = 8,
    ConfigurationChange = 9
};

UENUM(BlueprintType)
enum class ESecuritySeverity : uint8
{
    Info = 0,
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4
};

USTRUCT(BlueprintType)
struct FSecurityAuditEvent
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    ESecurityEventType EventType;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    ESecuritySeverity Severity;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    FDateTime Timestamp;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    FString UserId;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    FString Description;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    FString IPAddress;
    
    FSecurityAuditEvent()
    {
        EventType = ESecurityEventType::None;
        Severity = ESecuritySeverity::Info;
        Timestamp = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FSecurityMetrics
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    int32 TotalEvents;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    int32 CriticalEvents;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    int32 CheatDetections;
    
    UPROPERTY(BlueprintReadOnly, Category = "Security")
    float AverageEventsPerHour;
    
    FSecurityMetrics()
    {
        TotalEvents = 0;
        CriticalEvents = 0;
        CheatDetections = 0;
        AverageEventsPerHour = 0.0f;
    }
};
    
/**
 * Security Audit Subsystem
 * Comprehensive security event logging and monitoring
 */
UCLASS(BlueprintType)
class DARKAGE_API USecurityAuditSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    USecurityAuditSubsystem();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void LogSecurityEvent(ESecurityEventType EventType, ESecuritySeverity Severity, 
        const FString& UserId, const FString& Description, 
        const FString& IPAddress = TEXT(""), const FString& AdditionalData = TEXT(""));

    /**
     * Get security metrics for monitoring dashboard
     * @return Current security metrics
     */
    UFUNCTION(BlueprintCallable)
    FSecurityMetrics GetSecurityMetrics() const;

    /**
     * Get recent security events
     * @param Hours - Number of hours to look back
     * @param MinSeverity - Minimum severity to include
     * @return Array of recent security events
     */
    UFUNCTION(BlueprintCallable)
    TArray<FSecurityAuditEvent> GetRecentEvents(int32 Hours = 24, ESecuritySeverity MinSeverity = ESecuritySeverity::Info);

    /**
     * Search security events
     * @param SearchTerm - Term to search for
     * @param EventType - Filter by event type (None = all types)
     * @param StartTime - Start of time range
     * @param EndTime - End of time range
     * @return Matching security events
     */
    UFUNCTION(BlueprintCallable)
    TArray<FSecurityAuditEvent> SearchSecurityEvents(const FString& SearchTerm,
        ESecurityEventType EventType = ESecurityEventType::None);
    
    /**
     * Search security events with time range
     * @param SearchTerm - Term to search for
     * @param EventType - Filter by event type (None = all types)
     * @param StartTime - Start of time range
     * @param EndTime - End of time range
     * @return Matching security events
     */
    UFUNCTION(BlueprintCallable)
    TArray<FSecurityAuditEvent> SearchSecurityEventsInTimeRange(const FString& SearchTerm,
        ESecurityEventType EventType, const FDateTime& StartTime, const FDateTime& EndTime);

    /**
     * Generate security report
     * @param Hours - Time period for report
     * @return Formatted security report
     */
    UFUNCTION(BlueprintCallable)
    FString GenerateSecurityReport(int32 Hours = 24);

    /**
     * Export audit log for compliance/forensics
     * @param StartTime - Start of export range
     * @param EndTime - End of export range
     * @param FilePath - Export file path
     * @return True if export successful
     */
    UFUNCTION(BlueprintCallable)
    bool ExportAuditLog(const FDateTime& StartTime, const FDateTime& EndTime, const FString& FilePath);

    /**
     * Set security alert thresholds
     * @param EventType - Event type to configure
     * @param Threshold - Number of events that trigger alert
     * @param TimeWindow - Time window in minutes
     */
    UFUNCTION(BlueprintCallable)
    void SetAlertThreshold(ESecurityEventType EventType, int32 Threshold, int32 TimeWindow);

    /**
     * Emergency security lockdown
     * - Disable all admin access
     * - Enable maximum security monitoring
     * - Send immediate alerts
     */
    UFUNCTION(BlueprintCallable)
    void EmergencyLockdown(const FString& Reason);

    /**
     * Clear security lockdown
     */
    UFUNCTION(BlueprintCallable)
    void ClearLockdown();

    /**
     * Check if system is in lockdown mode
     */
    UFUNCTION(BlueprintCallable)
    bool IsInLockdown() const;

protected:
    // Event storage
    UPROPERTY()
    TArray<FSecurityAuditEvent> SecurityEvents;
    
    // Metrics tracking
    UPROPERTY()
    FSecurityMetrics CurrentMetrics;
    
    // Alert thresholds
    UPROPERTY()
    TMap<ESecurityEventType, int32> AlertThresholds;
    
    UPROPERTY()
    TMap<ESecurityEventType, int32> AlertTimeWindows;
    
    // System state
    UPROPERTY()
    bool bIsInLockdown;
    
    UPROPERTY()
    FString LockdownReason;
    
    UPROPERTY()
    FDateTime LockdownTime;
    
    // Event processing
    void ProcessSecurityEvents();
    void CheckAlertThresholds();
    void SendSecurityAlert(const FString& AlertMessage, ESecuritySeverity Severity);
    void CleanupOldEvents();
    
    // File operations
    bool WriteEventToFile(const FSecurityAuditEvent& Event);
    bool LoadEventsFromFile();
    
    // Utility functions
    FString GenerateEventId();
    FString FormatEventForLog(const FSecurityAuditEvent& Event);
    FString GetSeverityString(ESecuritySeverity Severity);
    FString GetEventTypeString(ESecurityEventType EventType);
    
    // Timers
    FTimerHandle ProcessEventsTimer;
    FTimerHandle CleanupTimer;
    FTimerHandle MetricsUpdateTimer;
    
    // Configuration
    static constexpr int32 MAX_STORED_EVENTS = 100000;
    static constexpr int32 EVENT_RETENTION_DAYS = 90;
    static constexpr float PROCESS_INTERVAL = 1.0f;
    static constexpr float CLEANUP_INTERVAL = 3600.0f; // 1 hour
    static constexpr float METRICS_UPDATE_INTERVAL = 60.0f; // 1 minute
};
