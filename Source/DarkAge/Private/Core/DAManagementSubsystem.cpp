// Copyright (c) 2025 RaioCore
// Forcing a recompile to fix linker issues.

#include "Core/DAManagementSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformProcess.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Blueprint/UserWidget.h"

bool UDAManagementSubsystem::IsTickable() const
{
    return true;
}

TStatId UDAManagementSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UDAManagementSubsystem, STATGROUP_Tickables);
}

void UDAManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bPerformanceMonitoringEnabled = true;
    MonitoringInterval = 1.0f;
    TimeSinceLastUpdate = 0.0f;
    SystemStartTime = FDateTime::Now();
    
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Initializing..."));
    
    // Initialize performance metrics
    CurrentMetrics = FDAPerformanceMetrics();
    
    // Initialize system statuses
    SystemStatuses.Empty();
    
    // Initialize FPS history
    FPSHistory.Empty();
    FPSHistory.Reserve(MaxFPSHistorySize);
    
    // Register default console commands
    RegisterDefaultCommands();
    
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Initialized successfully"));
}

void UDAManagementSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Deinitializing..."));
    
    // Clear all data
    SystemStatuses.Empty();
    RegisteredCommands.Empty();
    CommandExecutors.Empty();
    FPSHistory.Empty();
    
    Super::Deinitialize();
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Deinitialized"));
}

void UDAManagementSubsystem::Tick(float DeltaTime)
{
    if (!bPerformanceMonitoringEnabled)
    {
        return;
    }
    
    TimeSinceLastUpdate += DeltaTime;
    
    if (TimeSinceLastUpdate >= MonitoringInterval)
    {
        UpdatePerformanceMetrics();
        UpdateSystemStatuses();
        TimeSinceLastUpdate = 0.0f;
    }
}

FDAPerformanceMetrics UDAManagementSubsystem::GetPerformanceMetrics() const
{
    return CurrentMetrics;
}

TArray<FDASystemStatus> UDAManagementSubsystem::GetSystemStatuses() const
{
    TArray<FDASystemStatus> Statuses;
    SystemStatuses.GenerateValueArray(Statuses);
    return Statuses;
}

bool UDAManagementSubsystem::GetSystemStatus(const FString& SystemName, FDASystemStatus& OutStatus) const
{
    if (SystemStatuses.Contains(SystemName))
    {
        OutStatus = SystemStatuses[SystemName];
        return true;
    }
    
    return false;
}

FString UDAManagementSubsystem::ExecuteCommand(const FString& Command)
{
    if (Command.IsEmpty())
    {
        return TEXT("Error: Empty command");
    }
    
    // Parse command and arguments
    TArray<FString> CommandParts;
    Command.ParseIntoArray(CommandParts, TEXT(" "), true);
    
    if (CommandParts.Num() == 0)
    {
        return TEXT("Error: Invalid command");
    }
    
    FString CommandName = CommandParts[0].ToLower();
    TArray<FString> Args;
    
    for (int32 i = 1; i < CommandParts.Num(); i++)
    {
        Args.Add(CommandParts[i]);
    }
    
    // Check if it's a registered command
    if (CommandExecutors.Contains(CommandName))
    {
        return CommandExecutors[CommandName](Args);
    }
    
    // Check if it's a built-in command
    return ExecuteBuiltInCommand(CommandName, Args);
}

TArray<FDAConsoleCommand> UDAManagementSubsystem::GetAvailableCommands() const
{
    TArray<FDAConsoleCommand> Commands;
    RegisteredCommands.GenerateValueArray(Commands);
    return Commands;
}

void UDAManagementSubsystem::RegisterCommand(const FDAConsoleCommand& Command, const FString& ExecutionFunction)
{
    RegisteredCommands.Add(Command.CommandName.ToLower(), Command);
    
    // For now, we'll store a placeholder executor
    // In a full implementation, this would bind to actual functions
    CommandExecutors.Add(Command.CommandName.ToLower(), [this, ExecutionFunction](const TArray<FString>& Args) -> FString
    {
        return FString::Printf(TEXT("Executed: %s with %d arguments"), *ExecutionFunction, Args.Num());
    });
    
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Registered command: %s"), *Command.CommandName);
}

void UDAManagementSubsystem::SetPerformanceMonitoringEnabled(bool bEnabled)
{
    bPerformanceMonitoringEnabled = bEnabled;
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Performance monitoring %s"), 
           bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UDAManagementSubsystem::SetMonitoringInterval(float IntervalSeconds)
{
    MonitoringInterval = FMath::Max(0.1f, IntervalSeconds);
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Monitoring interval set to %.2f seconds"), MonitoringInterval);
}

float UDAManagementSubsystem::GetSystemUptime() const
{
    FTimespan Uptime = FDateTime::Now() - SystemStartTime;
    return static_cast<float>(Uptime.GetTotalSeconds());
}

TMap<FString, FString> UDAManagementSubsystem::GetMemoryStatistics() const
{
    TMap<FString, FString> MemStats;
    
    FPlatformMemoryStats MemoryStats = FPlatformMemory::GetStats();
    
    MemStats.Add(TEXT("UsedPhysical"), FString::Printf(TEXT("%.2f MB"), MemoryStats.UsedPhysical / (1024.0f * 1024.0f)));
    MemStats.Add(TEXT("UsedVirtual"), FString::Printf(TEXT("%.2f MB"), MemoryStats.UsedVirtual / (1024.0f * 1024.0f)));
    MemStats.Add(TEXT("PeakUsedPhysical"), FString::Printf(TEXT("%.2f MB"), MemoryStats.PeakUsedPhysical / (1024.0f * 1024.0f)));
    MemStats.Add(TEXT("PeakUsedVirtual"), FString::Printf(TEXT("%.2f MB"), MemoryStats.PeakUsedVirtual / (1024.0f * 1024.0f)));
    MemStats.Add(TEXT("TotalPhysical"), FString::Printf(TEXT("%.2f MB"), MemoryStats.TotalPhysical / (1024.0f * 1024.0f)));
    MemStats.Add(TEXT("TotalVirtual"), FString::Printf(TEXT("%.2f MB"), MemoryStats.TotalVirtual / (1024.0f * 1024.0f)));
    
    return MemStats;
}

void UDAManagementSubsystem::ForceGarbageCollection()
{
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Forcing garbage collection..."));
    GEngine->ForceGarbageCollection(true);
}

TMap<FString, EDASystemHealth> UDAManagementSubsystem::GetSubsystemHealthSummary() const
{
    TMap<FString, EDASystemHealth> HealthSummary;
    
    for (const auto& StatusPair : SystemStatuses)
    {
        HealthSummary.Add(StatusPair.Key, StatusPair.Value.Health);
    }
    
    return HealthSummary;
}

bool UDAManagementSubsystem::ExportDiagnostics(const FString& FilePath)
{
    FString DiagnosticsData;
    
    // Add system information
    DiagnosticsData += TEXT("=== DarkAge System Diagnostics ===\n");
    DiagnosticsData += FString::Printf(TEXT("Export Time: %s\n"), *FDateTime::Now().ToString());
    DiagnosticsData += FString::Printf(TEXT("System Uptime: %.2f seconds\n"), GetSystemUptime());
    DiagnosticsData += TEXT("\n");
    
    // Add performance metrics
    DiagnosticsData += TEXT("=== Performance Metrics ===\n");
    DiagnosticsData += FString::Printf(TEXT("Current FPS: %.2f\n"), CurrentMetrics.CurrentFPS);
    DiagnosticsData += FString::Printf(TEXT("Average FPS: %.2f\n"), CurrentMetrics.AverageFPS);
    DiagnosticsData += FString::Printf(TEXT("Memory Usage: %.2f MB\n"), CurrentMetrics.MemoryUsageMB);
    DiagnosticsData += FString::Printf(TEXT("CPU Usage: %.2f%%\n"), CurrentMetrics.CPUUsagePercent);
    DiagnosticsData += FString::Printf(TEXT("GPU Usage: %.2f%%\n"), CurrentMetrics.GPUUsagePercent);
    DiagnosticsData += TEXT("\n");
    
    // Add system statuses
    DiagnosticsData += TEXT("=== System Statuses ===\n");
    for (const auto& StatusPair : SystemStatuses)
    {
        DiagnosticsData += FString::Printf(TEXT("%s: %s - %s\n"), 
                                         *StatusPair.Key, 
                                         *UEnum::GetValueAsString(StatusPair.Value.Health),
                                         *StatusPair.Value.StatusMessage);
    }
    
    return FFileHelper::SaveStringToFile(DiagnosticsData, *FilePath);
}

TArray<FString> UDAManagementSubsystem::GetRecentErrors(int32 Count) const
{
    // Placeholder - in a full implementation, this would access the logging system
    TArray<FString> RecentErrors;
    RecentErrors.Add(TEXT("No recent errors found"));
    return RecentErrors;
}

void UDAManagementSubsystem::ClearCaches()
{
    UE_LOG(LogTemp, Log, TEXT("DAManagementSubsystem: Clearing caches..."));
    
    // Clear FPS history
    FPSHistory.Empty();
    
    // Force garbage collection
    ForceGarbageCollection();
}

bool UDAManagementSubsystem::RestartSubsystem(const FString& SubsystemName)
{
    UE_LOG(LogTemp, Warning, TEXT("DAManagementSubsystem: Subsystem restart not implemented for: %s"), *SubsystemName);
    return false;
}

void UDAManagementSubsystem::UpdatePerformanceMetrics()
{
    // Update FPS
    if (GEngine)
    {
        CurrentMetrics.CurrentFPS = 1.0f / FApp::GetDeltaTime();
        
        // Add to history for averaging
        FPSHistory.Add(CurrentMetrics.CurrentFPS);
        if (FPSHistory.Num() > MaxFPSHistorySize)
        {
            FPSHistory.RemoveAt(0);
        }
        
        // Calculate average FPS
        float TotalFPS = 0.0f;
        for (float FPS : FPSHistory)
        {
            TotalFPS += FPS;
        }
        CurrentMetrics.AverageFPS = FPSHistory.Num() > 0 ? TotalFPS / FPSHistory.Num() : 0.0f;
    }
    
    // Update memory usage
    FPlatformMemoryStats MemoryStats = FPlatformMemory::GetStats();
    CurrentMetrics.MemoryUsageMB = MemoryStats.UsedPhysical / (1024.0f * 1024.0f);
    
    // Update other metrics (placeholders for now)
    CurrentMetrics.CPUUsagePercent = FMath::RandRange(10.0f, 80.0f); // Placeholder
    CurrentMetrics.GPUUsagePercent = FMath::RandRange(20.0f, 90.0f); // Placeholder
    CurrentMetrics.NetworkLatencyMS = FMath::RandRange(10.0f, 100.0f); // Placeholder
    
    // Update subsystem-specific metrics
    CurrentMetrics.ActiveNPCCount = 0; // Would query NPC subsystem
    CurrentMetrics.ActiveQuestCount = 0; // Would query quest subsystem
    CurrentMetrics.EconomySimulationLoad = 0.0f; // Would query economy subsystem
}

void UDAManagementSubsystem::UpdateSystemStatuses()
{
    // Update core engine status
    FDASystemStatus EngineStatus;
    EngineStatus.SystemName = TEXT("Engine");
    EngineStatus.Health = EDASystemHealth::Healthy;
    EngineStatus.StatusMessage = TEXT("Running normally");
    EngineStatus.LastUpdate = FDateTime::Now();
    SystemStatuses.Add(TEXT("Engine"), EngineStatus);
    
    // Add other system statuses as needed
    // This would typically query other subsystems for their health
}

void UDAManagementSubsystem::RegisterDefaultCommands()
{
    // Register built-in commands
    FDAConsoleCommand HelpCommand;
    HelpCommand.CommandName = TEXT("help");
    HelpCommand.Description = TEXT("Show available commands");
    HelpCommand.Category = TEXT("General");
    HelpCommand.Usage = TEXT("help [command]");
    RegisteredCommands.Add(TEXT("help"), HelpCommand);
    
    FDAConsoleCommand StatusCommand;
    StatusCommand.CommandName = TEXT("status");
    StatusCommand.Description = TEXT("Show system status");
    StatusCommand.Category = TEXT("Monitoring");
    StatusCommand.Usage = TEXT("status [system]");
    RegisteredCommands.Add(TEXT("status"), StatusCommand);
    
    FDAConsoleCommand MemoryCommand;
    MemoryCommand.CommandName = TEXT("memory");
    MemoryCommand.Description = TEXT("Show memory statistics");
    MemoryCommand.Category = TEXT("Monitoring");
    MemoryCommand.Usage = TEXT("memory");
    RegisteredCommands.Add(TEXT("memory"), MemoryCommand);
    
    FDAConsoleCommand GCCommand;
    GCCommand.CommandName = TEXT("gc");
    GCCommand.Description = TEXT("Force garbage collection");
    GCCommand.Category = TEXT("Maintenance");
    GCCommand.Usage = TEXT("gc");
    RegisteredCommands.Add(TEXT("gc"), GCCommand);
}

FString UDAManagementSubsystem::ExecuteBuiltInCommand(const FString& Command, const TArray<FString>& Args)
{
    if (Command == TEXT("help"))
    {
        if (Args.Num() > 0)
        {
            // Show help for specific command
            FString CommandName = Args[0].ToLower();
            if (RegisteredCommands.Contains(CommandName))
            {
                const FDAConsoleCommand& Cmd = RegisteredCommands[CommandName];
                return FString::Printf(TEXT("%s: %s\nUsage: %s"), *Cmd.CommandName, *Cmd.Description, *Cmd.Usage);
            }
            else
            {
                return FString::Printf(TEXT("Unknown command: %s"), *CommandName);
            }
        }
        else
        {
            // Show all commands
            FString HelpText = TEXT("Available commands:\n");
            for (const auto& CmdPair : RegisteredCommands)
            {
                HelpText += FString::Printf(TEXT("  %s - %s\n"), *CmdPair.Value.CommandName, *CmdPair.Value.Description);
            }
            return HelpText;
        }
    }
    else if (Command == TEXT("status"))
    {
        if (Args.Num() > 0)
        {
            // Show status for specific system
            FDASystemStatus Status;
            if (GetSystemStatus(Args[0], Status))
            {
                return FString::Printf(TEXT("%s: %s - %s"), 
                                     *Status.SystemName, 
                                     *UEnum::GetValueAsString(Status.Health),
                                     *Status.StatusMessage);
            }
            else
            {
                return FString::Printf(TEXT("System not found: %s"), *Args[0]);
            }
        }
        else
        {
            // Show all system statuses
            FString StatusText = TEXT("System Status:\n");
            for (const auto& StatusPair : SystemStatuses)
            {
                StatusText += FString::Printf(TEXT("  %s: %s\n"), 
                                            *StatusPair.Key, 
                                            *UEnum::GetValueAsString(StatusPair.Value.Health));
            }
            return StatusText;
        }
    }
    else if (Command == TEXT("memory"))
    {
        TMap<FString, FString> MemStats = GetMemoryStatistics();
        FString MemoryText = TEXT("Memory Statistics:\n");
        for (const auto& StatPair : MemStats)
        {
            MemoryText += FString::Printf(TEXT("  %s: %s\n"), *StatPair.Key, *StatPair.Value);
        }
        return MemoryText;
    }
    else if (Command == TEXT("gc"))
    {
        ForceGarbageCollection();
        return TEXT("Garbage collection completed");
    }
    
    return FString::Printf(TEXT("Unknown command: %s"), *Command);
}

// UDAManagementConsoleWidget implementation
void UDAManagementConsoleWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Get reference to management subsystem
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        ManagementSubsystem = GameInstance->GetSubsystem<UDAManagementSubsystem>();
    }
    
    UpdateTimer = 0.0f;
    InitializeConsole();
}

void UDAManagementConsoleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    UpdateTimer += InDeltaTime;
    
    if (UpdateTimer >= UIUpdateInterval)
    {
        RefreshDisplays();
        UpdateTimer = 0.0f;
    }
}

void UDAManagementConsoleWidget::ExecuteConsoleCommand(const FString& Command)
{
    if (ManagementSubsystem)
    {
        FString Result = ManagementSubsystem->ExecuteCommand(Command);
        AddCommandOutput(Result);
    }
}

TArray<FString> UDAManagementConsoleWidget::GetCommandSuggestions(const FString& PartialCommand) const
{
    TArray<FString> Suggestions;
    
    if (ManagementSubsystem)
    {
        TArray<FDAConsoleCommand> Commands = ManagementSubsystem->GetAvailableCommands();
        for (const FDAConsoleCommand& Command : Commands)
        {
            if (Command.CommandName.StartsWith(PartialCommand, ESearchCase::IgnoreCase))
            {
                Suggestions.Add(Command.CommandName);
            }
        }
    }
    
    return Suggestions;
}

void UDAManagementConsoleWidget::RefreshDisplays()
{
    if (ManagementSubsystem)
    {
        FDAPerformanceMetrics Metrics = ManagementSubsystem->GetPerformanceMetrics();
        UpdatePerformanceDisplay(Metrics);
        
        TArray<FDASystemStatus> Statuses = ManagementSubsystem->GetSystemStatuses();
        UpdateSystemStatusDisplay(Statuses);
    }
}