// Copyright (c) 2025 RaioCore
// Centralized logging subsystem for DarkAge

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "HAL/FileManager.h"
#include "Misc/DateTime.h"
#include "DALoggingSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDarkAge, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeEconomy, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeNPC, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeCrime, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgePolitics, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeQuests, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeSurvival, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeWeather, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeCombat, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeNetwork, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgeSecurity, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDarkAgePerformance, Log, All);

/**
 * Log severity levels for structured logging
 */
UENUM(BlueprintType)
enum class EDALogLevel : uint8
{
    Trace       UMETA(DisplayName = "Trace"),
    Debug       UMETA(DisplayName = "Debug"),
    Info        UMETA(DisplayName = "Info"),
    Warning     UMETA(DisplayName = "Warning"),
    Error       UMETA(DisplayName = "Error"),
    Critical    UMETA(DisplayName = "Critical")
};

/**
 * Log categories for organized logging
 */
UENUM(BlueprintType)
enum class EDALogCategory : uint8
{
    General     UMETA(DisplayName = "General"),
    Economy     UMETA(DisplayName = "Economy"),
    NPC         UMETA(DisplayName = "NPC"),
    Crime       UMETA(DisplayName = "Crime"),
    Politics    UMETA(DisplayName = "Politics"),
    Quests      UMETA(DisplayName = "Quests"),
    Survival    UMETA(DisplayName = "Survival"),
    Weather     UMETA(DisplayName = "Weather"),
    AI          UMETA(DisplayName = "AI"),
    Combat      UMETA(DisplayName = "Combat"),
    UI          UMETA(DisplayName = "UI"),
    Network     UMETA(DisplayName = "Network"),
    Security    UMETA(DisplayName = "Security"),
    Performance UMETA(DisplayName = "Performance")
};

/**
 * Structured log entry for advanced logging features
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDALogEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadOnly)
    EDALogLevel Level;

    UPROPERTY(BlueprintReadOnly)
    EDALogCategory Category;

    UPROPERTY(BlueprintReadOnly)
    FString Message;

    UPROPERTY(BlueprintReadOnly)
    FString Context;

    UPROPERTY(BlueprintReadOnly)
    FString SourceFunction;

    UPROPERTY(BlueprintReadOnly)
    int32 SourceLine;

    FDALogEntry()
        : Timestamp(FDateTime::Now())
        , Level(EDALogLevel::Info)
        , Category(EDALogCategory::General)
        , SourceLine(0)
    {}

    FDALogEntry(EDALogLevel InLevel, EDALogCategory InCategory, const FString& InMessage, const FString& InContext = TEXT(""), const FString& InFunction = TEXT(""), int32 InLine = 0)
        : Timestamp(FDateTime::Now())
        , Level(InLevel)
        , Category(InCategory)
        , Message(InMessage)
        , Context(InContext)
        , SourceFunction(InFunction)
        , SourceLine(InLine)
    {}
};

/**
 * Centralized logging subsystem for DarkAge
 * Provides structured logging, file output, and performance monitoring
 */
UCLASS()
class DARKAGE_API UDALoggingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Log a message with specified level and category
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void LogMessage(EDALogLevel Level, EDALogCategory Category, const FString& Message, const FString& Context = TEXT(""));

    /**
     * Log with source information (for C++ use)
     */
    void LogMessageWithSource(EDALogLevel Level, EDALogCategory Category, const FString& Message, const FString& Context = TEXT(""), const FString& Function = TEXT(""), int32 Line = 0);

    /**
     * Get recent log entries for debugging
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    TArray<FDALogEntry> GetRecentLogs(int32 Count = 100, EDALogLevel MinLevel = EDALogLevel::Debug);

    /**
     * Get logs by category
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    TArray<FDALogEntry> GetLogsByCategory(EDALogCategory Category, int32 Count = 50);

    /**
     * Enable/disable file logging
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void SetFileLoggingEnabled(bool bEnabled);

    /**
     * Set minimum log level for console output
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void SetConsoleLogLevel(EDALogLevel MinLevel);

    /**
     * Set minimum log level for file output
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void SetFileLogLevel(EDALogLevel MinLevel);

    /**
     * Flush all pending log entries to file
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void FlushLogs();

    /**
     * Get logging statistics
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void GetLoggingStats(int32& TotalLogs, int32& ErrorCount, int32& WarningCount);

    /**
     * Clear log history (keep only recent entries)
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Logging")
    void ClearOldLogs(int32 KeepCount = 1000);

protected:
    /**
     * Write log entry to file
     */
    void WriteToFile(const FDALogEntry& LogEntry);

    /**
     * Get the appropriate UE log category for our category
     */
    const FLogCategoryBase* GetUELogCategory(EDALogCategory Category) const;

    /**
     * Convert our log level to UE log verbosity
     */
    ELogVerbosity::Type GetUELogVerbosity(EDALogLevel Level) const;

    /**
     * Format log entry for file output
     */
    FString FormatLogEntry(const FDALogEntry& LogEntry) const;

private:
    /** All log entries (circular buffer) */
    UPROPERTY()
    TArray<FDALogEntry> LogHistory;

    /** Maximum number of log entries to keep in memory */
    UPROPERTY()
    int32 MaxLogHistorySize = 5000;

    /** Current log file handle */
    TUniquePtr<FArchive> LogFileArchive;

    /** Log file path */
    FString LogFilePath;

    /** Whether file logging is enabled */
    UPROPERTY()
    bool bFileLoggingEnabled = true;

    /** Minimum log level for console output */
    UPROPERTY()
    EDALogLevel ConsoleLogLevel = EDALogLevel::Info;

    /** Minimum log level for file output */
    UPROPERTY()
    EDALogLevel FileLogLevel = EDALogLevel::Debug;

    /** Statistics */
    UPROPERTY()
    int32 TotalLogCount = 0;

    UPROPERTY()
    int32 ErrorLogCount = 0;

    UPROPERTY()
    int32 WarningLogCount = 0;

    /** Critical section for thread safety */
    mutable FCriticalSection LogCriticalSection;
};

/**
 * Convenience macros for logging
 */
#define DA_LOG(Level, Category, Format, ...) \
    if (UDALoggingSubsystem* LogSubsystem = GEngine && GEngine->GetGameInstance() ? GEngine->GetGameInstance()->GetSubsystem<UDALoggingSubsystem>() : nullptr) \
    { \
        LogSubsystem->LogMessageWithSource(Level, Category, FString::Printf(Format, ##__VA_ARGS__), TEXT(""), TEXT(__FUNCTION__), __LINE__); \
    }

#define DA_LOG_TRACE(Category, Format, ...) DA_LOG(EDALogLevel::Trace, Category, Format, ##__VA_ARGS__)
#define DA_LOG_DEBUG(Category, Format, ...) DA_LOG(EDALogLevel::Debug, Category, Format, ##__VA_ARGS__)
#define DA_LOG_INFO(Category, Format, ...) DA_LOG(EDALogLevel::Info, Category, Format, ##__VA_ARGS__)
#define DA_LOG_WARNING(Category, Format, ...) DA_LOG(EDALogLevel::Warning, Category, Format, ##__VA_ARGS__)
#define DA_LOG_ERROR(Category, Format, ...) DA_LOG(EDALogLevel::Error, Category, Format, ##__VA_ARGS__)
#define DA_LOG_CRITICAL(Category, Format, ...) DA_LOG(EDALogLevel::Critical, Category, Format, ##__VA_ARGS__)

// Category-specific convenience macros
#define DA_LOG_ECONOMY(Level, Format, ...) DA_LOG(Level, EDALogCategory::Economy, Format, ##__VA_ARGS__)
#define DA_LOG_NPC(Level, Format, ...) DA_LOG(Level, EDALogCategory::NPC, Format, ##__VA_ARGS__)
#define DA_LOG_CRIME(Level, Format, ...) DA_LOG(Level, EDALogCategory::Crime, Format, ##__VA_ARGS__)
#define DA_LOG_POLITICS(Level, Format, ...) DA_LOG(Level, EDALogCategory::Politics, Format, ##__VA_ARGS__)
#define DA_LOG_QUESTS(Level, Format, ...) DA_LOG(Level, EDALogCategory::Quests, Format, ##__VA_ARGS__)
#define DA_LOG_SURVIVAL(Level, Format, ...) DA_LOG(Level, EDALogCategory::Survival, Format, ##__VA_ARGS__)
#define DA_LOG_WEATHER(Level, Format, ...) DA_LOG(Level, EDALogCategory::Weather, Format, ##__VA_ARGS__)
#define DA_LOG_AI(Level, Format, ...) DA_LOG(Level, EDALogCategory::AI, Format, ##__VA_ARGS__)
#define DA_LOG_COMBAT(Level, Format, ...) DA_LOG(Level, EDALogCategory::Combat, Format, ##__VA_ARGS__)
#define DA_LOG_UI(Level, Format, ...) DA_LOG(Level, EDALogCategory::UI, Format, ##__VA_ARGS__)
#define DA_LOG_NETWORK(Level, Format, ...) DA_LOG(Level, EDALogCategory::Network, Format, ##__VA_ARGS__)
#define DA_LOG_SECURITY(Level, Format, ...) DA_LOG(Level, EDALogCategory::Security, Format, ##__VA_ARGS__)
#define DA_LOG_PERFORMANCE(Level, Format, ...) DA_LOG(Level, EDALogCategory::Performance, Format, ##__VA_ARGS__)