// Copyright (c) 2025 RaioCore
// Implementation of centralized logging subsystem for DarkAge

#include "Core/DALoggingSubsystem.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

// Define log categories
DEFINE_LOG_CATEGORY(LogDarkAge);
DEFINE_LOG_CATEGORY(LogDarkAgeEconomy);
DEFINE_LOG_CATEGORY(LogDarkAgeNPC);
DEFINE_LOG_CATEGORY(LogDarkAgeCrime);
DEFINE_LOG_CATEGORY(LogDarkAgePolitics);
DEFINE_LOG_CATEGORY(LogDarkAgeQuests);
DEFINE_LOG_CATEGORY(LogDarkAgeSurvival);
DEFINE_LOG_CATEGORY(LogDarkAgeWeather);
DEFINE_LOG_CATEGORY(LogDarkAgeAI);
DEFINE_LOG_CATEGORY(LogDarkAgeCombat);
DEFINE_LOG_CATEGORY(LogDarkAgeUI);
DEFINE_LOG_CATEGORY(LogDarkAgeNetwork);
DEFINE_LOG_CATEGORY(LogDarkAgeSecurity);
DEFINE_LOG_CATEGORY(LogDarkAgePerformance);

void UDALoggingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Initialize log file
    if (bFileLoggingEnabled)
    {
        FString LogDirectory = FPaths::ProjectLogDir() / TEXT("DarkAge");
        FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
        LogFilePath = LogDirectory / FString::Printf(TEXT("DarkAge_%s.log"), *Timestamp);

        // Ensure directory exists
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        if (!PlatformFile.DirectoryExists(*LogDirectory))
        {
            PlatformFile.CreateDirectoryTree(*LogDirectory);
        }

        // Open log file
        LogFileArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*LogFilePath, FILEWRITE_AllowRead));
        if (LogFileArchive.IsValid())
        {
            FString Header = FString::Printf(TEXT("=== DarkAge Log Started at %s ===\n"), *FDateTime::Now().ToString());
            LogFileArchive->Serialize(TCHAR_TO_UTF8(*Header), Header.Len());
            LogFileArchive->Flush();
        }
    }

    // Log initialization
    LogMessage(EDALogLevel::Info, EDALogCategory::General, TEXT("DarkAge Logging Subsystem initialized"));
    
    UE_LOG(LogDarkAge, Log, TEXT("DarkAge Logging Subsystem initialized with file logging %s"), 
        bFileLoggingEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UDALoggingSubsystem::Deinitialize()
{
    // Log shutdown
    LogMessage(EDALogLevel::Info, EDALogCategory::General, TEXT("DarkAge Logging Subsystem shutting down"));
    
    // Flush and close log file
    if (LogFileArchive.IsValid())
    {
        FString Footer = FString::Printf(TEXT("=== DarkAge Log Ended at %s ===\n"), *FDateTime::Now().ToString());
        LogFileArchive->Serialize(TCHAR_TO_UTF8(*Footer), Footer.Len());
        LogFileArchive->Flush();
        LogFileArchive->Close();
        LogFileArchive.Reset();
    }

    UE_LOG(LogDarkAge, Log, TEXT("DarkAge Logging Subsystem deinitialized"));
    
    Super::Deinitialize();
}

void UDALoggingSubsystem::LogMessage(EDALogLevel Level, EDALogCategory Category, const FString& Message, const FString& Context)
{
    LogMessageWithSource(Level, Category, Message, Context, TEXT(""), 0);
}

void UDALoggingSubsystem::LogMessageWithSource(EDALogLevel Level, EDALogCategory Category, const FString& Message, const FString& Context, const FString& Function, int32 Line)
{
    FScopeLock Lock(&LogCriticalSection);

    // Create log entry
    FDALogEntry LogEntry(Level, Category, Message, Context, Function, Line);

    // Add to history
    LogHistory.Add(LogEntry);
    TotalLogCount++;

    // Update statistics
    if (Level == EDALogLevel::Error || Level == EDALogLevel::Critical)
    {
        ErrorLogCount++;
    }
    else if (Level == EDALogLevel::Warning)
    {
        WarningLogCount++;
    }

    // Maintain history size
    if (LogHistory.Num() > MaxLogHistorySize)
    {
        LogHistory.RemoveAt(0, LogHistory.Num() - MaxLogHistorySize);
    }

    // Output to console if level is high enough
    if (Level >= ConsoleLogLevel)
    {
        const FLogCategoryBase* UECategory = GetUELogCategory(Category);
        ELogVerbosity::Type UEVerbosity = GetUELogVerbosity(Level);
        
        FString FormattedMessage = Context.IsEmpty() ? Message : FString::Printf(TEXT("[%s] %s"), *Context, *Message);
        
        // Use appropriate UE_LOG macro based on verbosity level
        switch (UEVerbosity)
        {
            case ELogVerbosity::VeryVerbose:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, VeryVerbose, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            case ELogVerbosity::Verbose:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Verbose, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Verbose, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            case ELogVerbosity::Log:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Log, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Log, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            case ELogVerbosity::Warning:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Warning, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            case ELogVerbosity::Error:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Error, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            case ELogVerbosity::Fatal:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Fatal, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Fatal, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
            default:
                if (Function.IsEmpty())
                {
                    UE_LOG(LogTemp, Log, TEXT("%s"), *FormattedMessage);
                }
                else
                {
                    UE_LOG(LogTemp, Log, TEXT("%s (%s:%d)"), *FormattedMessage, *Function, Line);
                }
                break;
        }
    }

    // Write to file if enabled and level is high enough
    if (bFileLoggingEnabled && Level >= FileLogLevel)
    {
        WriteToFile(LogEntry);
    }
}

TArray<FDALogEntry> UDALoggingSubsystem::GetRecentLogs(int32 Count, EDALogLevel MinLevel)
{
    FScopeLock Lock(&LogCriticalSection);
    
    TArray<FDALogEntry> FilteredLogs;
    
    // Filter by level and get most recent
    for (int32 i = FMath::Max(0, LogHistory.Num() - Count); i < LogHistory.Num(); i++)
    {
        if (LogHistory[i].Level >= MinLevel)
        {
            FilteredLogs.Add(LogHistory[i]);
        }
    }
    
    return FilteredLogs;
}

TArray<FDALogEntry> UDALoggingSubsystem::GetLogsByCategory(EDALogCategory Category, int32 Count)
{
    FScopeLock Lock(&LogCriticalSection);
    
    TArray<FDALogEntry> CategoryLogs;
    
    // Search backwards for most recent logs of this category
    for (int32 i = LogHistory.Num() - 1; i >= 0 && CategoryLogs.Num() < Count; i--)
    {
        if (LogHistory[i].Category == Category)
        {
            CategoryLogs.Insert(LogHistory[i], 0); // Insert at beginning to maintain chronological order
        }
    }
    
    return CategoryLogs;
}

void UDALoggingSubsystem::SetFileLoggingEnabled(bool bEnabled)
{
    FScopeLock Lock(&LogCriticalSection);
    
    if (bEnabled != bFileLoggingEnabled)
    {
        bFileLoggingEnabled = bEnabled;
        
        if (bEnabled && !LogFileArchive.IsValid())
        {
            // Re-initialize file logging
            FString LogDirectory = FPaths::ProjectLogDir() / TEXT("DarkAge");
            FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
            LogFilePath = LogDirectory / FString::Printf(TEXT("DarkAge_%s.log"), *Timestamp);
            
            IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
            if (!PlatformFile.DirectoryExists(*LogDirectory))
            {
                PlatformFile.CreateDirectoryTree(*LogDirectory);
            }
            
            LogFileArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*LogFilePath, FILEWRITE_AllowRead));
        }
        else if (!bEnabled && LogFileArchive.IsValid())
        {
            // Close file logging
            LogFileArchive->Close();
            LogFileArchive.Reset();
        }
        
        UE_LOG(LogDarkAge, Log, TEXT("File logging %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
    }
}

void UDALoggingSubsystem::SetConsoleLogLevel(EDALogLevel MinLevel)
{
    ConsoleLogLevel = MinLevel;
    UE_LOG(LogDarkAge, Log, TEXT("Console log level set to %d"), (int32)MinLevel);
}

void UDALoggingSubsystem::SetFileLogLevel(EDALogLevel MinLevel)
{
    FileLogLevel = MinLevel;
    UE_LOG(LogDarkAge, Log, TEXT("File log level set to %d"), (int32)MinLevel);
}

void UDALoggingSubsystem::FlushLogs()
{
    FScopeLock Lock(&LogCriticalSection);
    
    if (LogFileArchive.IsValid())
    {
        LogFileArchive->Flush();
    }
}

void UDALoggingSubsystem::GetLoggingStats(int32& TotalLogs, int32& ErrorCount, int32& WarningCount)
{
    FScopeLock Lock(&LogCriticalSection);
    
    TotalLogs = TotalLogCount;
    ErrorCount = ErrorLogCount;
    WarningCount = WarningLogCount;
}

void UDALoggingSubsystem::ClearOldLogs(int32 KeepCount)
{
    FScopeLock Lock(&LogCriticalSection);
    
    if (LogHistory.Num() > KeepCount)
    {
        LogHistory.RemoveAt(0, LogHistory.Num() - KeepCount);
    }
}

void UDALoggingSubsystem::WriteToFile(const FDALogEntry& LogEntry)
{
    if (LogFileArchive.IsValid())
    {
        FString FormattedEntry = FormatLogEntry(LogEntry);
        LogFileArchive->Serialize(TCHAR_TO_UTF8(*FormattedEntry), FormattedEntry.Len());
    }
}

const FLogCategoryBase* UDALoggingSubsystem::GetUELogCategory(EDALogCategory Category) const
{
    switch (Category)
    {
        case EDALogCategory::Economy: return &LogDarkAgeEconomy;
        case EDALogCategory::NPC: return &LogDarkAgeNPC;
        case EDALogCategory::Crime: return &LogDarkAgeCrime;
        case EDALogCategory::Politics: return &LogDarkAgePolitics;
        case EDALogCategory::Quests: return &LogDarkAgeQuests;
        case EDALogCategory::Survival: return &LogDarkAgeSurvival;
        case EDALogCategory::Weather: return &LogDarkAgeWeather;
        case EDALogCategory::AI: return &LogDarkAgeAI;
        case EDALogCategory::Combat: return &LogDarkAgeCombat;
        case EDALogCategory::UI: return &LogDarkAgeUI;
        case EDALogCategory::Network: return &LogDarkAgeNetwork;
        case EDALogCategory::Security: return &LogDarkAgeSecurity;
        case EDALogCategory::Performance: return &LogDarkAgePerformance;
        default: return &LogDarkAge;
    }
}

ELogVerbosity::Type UDALoggingSubsystem::GetUELogVerbosity(EDALogLevel Level) const
{
    switch (Level)
    {
        case EDALogLevel::Trace: return ELogVerbosity::VeryVerbose;
        case EDALogLevel::Debug: return ELogVerbosity::Verbose;
        case EDALogLevel::Info: return ELogVerbosity::Log;
        case EDALogLevel::Warning: return ELogVerbosity::Warning;
        case EDALogLevel::Error: return ELogVerbosity::Error;
        case EDALogLevel::Critical: return ELogVerbosity::Fatal;
        default: return ELogVerbosity::Log;
    }
}

FString UDALoggingSubsystem::FormatLogEntry(const FDALogEntry& LogEntry) const
{
    FString LevelString;
    switch (LogEntry.Level)
    {
        case EDALogLevel::Trace: LevelString = TEXT("TRACE"); break;
        case EDALogLevel::Debug: LevelString = TEXT("DEBUG"); break;
        case EDALogLevel::Info: LevelString = TEXT("INFO"); break;
        case EDALogLevel::Warning: LevelString = TEXT("WARN"); break;
        case EDALogLevel::Error: LevelString = TEXT("ERROR"); break;
        case EDALogLevel::Critical: LevelString = TEXT("CRIT"); break;
    }
    
    FString CategoryString = UEnum::GetValueAsString(LogEntry.Category);
    CategoryString = CategoryString.Replace(TEXT("EDALogCategory::"), TEXT(""));
    
    FString FormattedEntry = FString::Printf(TEXT("[%s] [%s] [%s] %s"),
        *LogEntry.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S")),
        *LevelString,
        *CategoryString,
        *LogEntry.Message
    );
    
    if (!LogEntry.Context.IsEmpty())
    {
        FormattedEntry += FString::Printf(TEXT(" [Context: %s]"), *LogEntry.Context);
    }
    
    if (!LogEntry.SourceFunction.IsEmpty())
    {
        FormattedEntry += FString::Printf(TEXT(" [%s:%d]"), *LogEntry.SourceFunction, LogEntry.SourceLine);
    }
    
    FormattedEntry += TEXT("\n");
    
    return FormattedEntry;
}