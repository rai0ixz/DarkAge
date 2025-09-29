// Copyright (c) 2025 RaioCore
// Management and monitoring subsystem for DarkAge

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "DAManagementSubsystem.generated.h"

/**
 * System health status
 */
UENUM(BlueprintType)
enum class EDASystemHealth : uint8
{
    Healthy     UMETA(DisplayName = "Healthy"),
    Warning     UMETA(DisplayName = "Warning"),
    Critical    UMETA(DisplayName = "Critical"),
    Error       UMETA(DisplayName = "Error"),
    Unknown     UMETA(DisplayName = "Unknown")
};

/**
 * Performance metrics structure
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAPerformanceMetrics
{
    GENERATED_BODY()

    /** Current frame rate */
    UPROPERTY(BlueprintReadOnly)
    float CurrentFPS;

    /** Average frame rate over last minute */
    UPROPERTY(BlueprintReadOnly)
    float AverageFPS;

    /** Memory usage in MB */
    UPROPERTY(BlueprintReadOnly)
    float MemoryUsageMB;

    /** CPU usage percentage */
    UPROPERTY(BlueprintReadOnly)
    float CPUUsagePercent;

    /** GPU usage percentage */
    UPROPERTY(BlueprintReadOnly)
    float GPUUsagePercent;

    /** Network latency in ms */
    UPROPERTY(BlueprintReadOnly)
    float NetworkLatencyMS;

    /** Active NPC count */
    UPROPERTY(BlueprintReadOnly)
    int32 ActiveNPCCount;

    /** Active quest count */
    UPROPERTY(BlueprintReadOnly)
    int32 ActiveQuestCount;

    /** Economy simulation load */
    UPROPERTY(BlueprintReadOnly)
    float EconomySimulationLoad;

    FDAPerformanceMetrics()
        : CurrentFPS(0.0f)
        , AverageFPS(0.0f)
        , MemoryUsageMB(0.0f)
        , CPUUsagePercent(0.0f)
        , GPUUsagePercent(0.0f)
        , NetworkLatencyMS(0.0f)
        , ActiveNPCCount(0)
        , ActiveQuestCount(0)
        , EconomySimulationLoad(0.0f)
    {}
};

/**
 * System status information
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDASystemStatus
{
    GENERATED_BODY()

    /** System name */
    UPROPERTY(BlueprintReadOnly)
    FString SystemName;

    /** Health status */
    UPROPERTY(BlueprintReadOnly)
    EDASystemHealth Health;

    /** Status message */
    UPROPERTY(BlueprintReadOnly)
    FString StatusMessage;

    /** Last update time */
    UPROPERTY(BlueprintReadOnly)
    FDateTime LastUpdate;

    /** Additional metrics */
    UPROPERTY(BlueprintReadOnly)
    TMap<FString, FString> Metrics;

    FDASystemStatus()
        : Health(EDASystemHealth::Unknown)
        , LastUpdate(FDateTime::Now())
    {}
};

/**
 * Console command structure
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAConsoleCommand
{
    GENERATED_BODY()

    /** Command name */
    UPROPERTY(BlueprintReadOnly)
    FString CommandName;

    /** Command description */
    UPROPERTY(BlueprintReadOnly)
    FString Description;

    /** Command category */
    UPROPERTY(BlueprintReadOnly)
    FString Category;

    /** Usage example */
    UPROPERTY(BlueprintReadOnly)
    FString Usage;

    /** Whether command requires admin privileges */
    UPROPERTY(BlueprintReadOnly)
    bool bRequiresAdmin;

    FDAConsoleCommand()
        : bRequiresAdmin(false)
    {}
};

/**
 * Management and monitoring subsystem
 * Provides administrative interface and system monitoring
 */
UCLASS(BlueprintType)
class DARKAGE_API UDAManagementSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    /**
     * Get current performance metrics
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    FDAPerformanceMetrics GetPerformanceMetrics() const;

    /**
     * Get system status for all monitored systems
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    TArray<FDASystemStatus> GetSystemStatuses() const;

    /**
     * Get status for a specific system
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    bool GetSystemStatus(const FString& SystemName, FDASystemStatus& OutStatus) const;

    /**
     * Execute a console command
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    FString ExecuteCommand(const FString& Command);

    /**
     * Get available console commands
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    TArray<FDAConsoleCommand> GetAvailableCommands() const;

    /**
     * Register a new console command
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    void RegisterCommand(const FDAConsoleCommand& Command, const FString& ExecutionFunction);

    /**
     * Enable/disable performance monitoring
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    void SetPerformanceMonitoringEnabled(bool bEnabled);

    /**
     * Set performance monitoring interval
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    void SetMonitoringInterval(float IntervalSeconds);

    /**
     * Get system uptime
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    float GetSystemUptime() const;

    /**
     * Get memory statistics
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    TMap<FString, FString> GetMemoryStatistics() const;

    /**
     * Force garbage collection
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    void ForceGarbageCollection();

    /**
     * Get subsystem health summary
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    TMap<FString, EDASystemHealth> GetSubsystemHealthSummary() const;

    /**
     * Export system diagnostics to file
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    bool ExportDiagnostics(const FString& FilePath);

    /**
     * Get recent error logs
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    TArray<FString> GetRecentErrors(int32 Count = 10) const;

    /**
     * Clear all cached data
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    void ClearCaches();

    /**
     * Restart specific subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Management")
    bool RestartSubsystem(const FString& SubsystemName);

protected:
    /**
     * Update performance metrics
     */
    void UpdatePerformanceMetrics();

    /**
     * Update system statuses
     */
    void UpdateSystemStatuses();

    /**
     * Register default console commands
     */
    void RegisterDefaultCommands();

    /**
     * Execute built-in commands
     */
    FString ExecuteBuiltInCommand(const FString& Command, const TArray<FString>& Args);

private:
    /** Current performance metrics */
    UPROPERTY()
    FDAPerformanceMetrics CurrentMetrics;

    /** System statuses */
    UPROPERTY()
    TMap<FString, FDASystemStatus> SystemStatuses;

    /** Registered console commands */
    UPROPERTY()
    TMap<FString, FDAConsoleCommand> RegisteredCommands;

    /** Command execution functions */
    TMap<FString, TFunction<FString(const TArray<FString>&)>> CommandExecutors;

    /** Performance monitoring enabled */
    UPROPERTY()
    bool bPerformanceMonitoringEnabled;

    /** Monitoring update interval */
    UPROPERTY()
    float MonitoringInterval;

    /** Time since last monitoring update */
    float TimeSinceLastUpdate;

    /** System start time */
    FDateTime SystemStartTime;

    /** FPS history for averaging */
    TArray<float> FPSHistory;

    /** Maximum FPS history size */
    static constexpr int32 MaxFPSHistorySize = 60;
};

/**
 * Console command execution delegate
 */
DECLARE_DELEGATE_RetVal_OneParam(FString, FDACommandExecutor, const TArray<FString>&);

/**
 * Management console widget interface
 */
UCLASS(Abstract, Blueprintable)
class DARKAGE_API UDAManagementConsoleWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * Initialize the console widget
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Management Console")
    void InitializeConsole();

    /**
     * Update performance display
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Management Console")
    void UpdatePerformanceDisplay(const FDAPerformanceMetrics& Metrics);

    /**
     * Update system status display
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Management Console")
    void UpdateSystemStatusDisplay(const TArray<FDASystemStatus>& Statuses);

    /**
     * Add command output to console
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Management Console")
    void AddCommandOutput(const FString& Output);

    /**
     * Clear console output
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Management Console")
    void ClearConsoleOutput();

protected:
    /** Reference to management subsystem */
    UPROPERTY(BlueprintReadOnly, Category = "Management Console")
    UDAManagementSubsystem* ManagementSubsystem;

    /**
     * Execute console command
     */
    UFUNCTION(BlueprintCallable, Category = "Management Console")
    void ExecuteConsoleCommand(const FString& Command);

    /**
     * Get command suggestions
     */
    UFUNCTION(BlueprintCallable, Category = "Management Console")
    TArray<FString> GetCommandSuggestions(const FString& PartialCommand) const;

    /**
     * Refresh all displays
     */
    UFUNCTION(BlueprintCallable, Category = "Management Console")
    void RefreshDisplays();

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    /** Update timer */
    float UpdateTimer;

    /** Update interval for UI */
    static constexpr float UIUpdateInterval = 1.0f;
};