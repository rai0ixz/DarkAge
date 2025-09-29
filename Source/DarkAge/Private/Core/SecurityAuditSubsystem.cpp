#include "Core/SecurityAuditSubsystem.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "TimerManager.h"

USecurityAuditSubsystem::USecurityAuditSubsystem()
{
}

void USecurityAuditSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadEventsFromFile();
    UE_LOG(LogTemp, Log, TEXT("SecurityAuditSubsystem: Initialized - Enterprise Security Audit Active. Loaded %d events from log."), SecurityEvents.Num());

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(ProcessEventsTimer, this, &USecurityAuditSubsystem::ProcessSecurityEvents, PROCESS_INTERVAL, true);
        World->GetTimerManager().SetTimer(CleanupTimer, this, &USecurityAuditSubsystem::CleanupOldEvents, CLEANUP_INTERVAL, true);
    }
}

void USecurityAuditSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ProcessEventsTimer);
        World->GetTimerManager().ClearTimer(CleanupTimer);
    }
    Super::Deinitialize();
}

void USecurityAuditSubsystem::LogSecurityEvent(ESecurityEventType EventType, ESecuritySeverity Severity, const FString& UserId, const FString& Description, const FString& IPAddress, const FString& AdditionalData)
{
    FSecurityAuditEvent Event;
    Event.EventType = EventType;
    Event.Severity = Severity;
    Event.UserId = UserId;
    Event.Description = Description;
    Event.IPAddress = IPAddress;
    Event.Timestamp = FDateTime::Now();

    SecurityEvents.Add(Event);

    if (SecurityEvents.Num() > MAX_STORED_EVENTS)
    {
        SecurityEvents.RemoveAt(0);
    }

    WriteEventToFile(Event);
}

FSecurityMetrics USecurityAuditSubsystem::GetSecurityMetrics() const
{
    return CurrentMetrics;
}

TArray<FSecurityAuditEvent> USecurityAuditSubsystem::GetRecentEvents(int32 Hours, ESecuritySeverity MinSeverity)
{
    TArray<FSecurityAuditEvent> RecentEvents;
    FDateTime CutoffTime = FDateTime::Now() - FTimespan::FromHours(Hours);

    for (const FSecurityAuditEvent& Event : SecurityEvents)
    {
        if (Event.Timestamp >= CutoffTime && Event.Severity >= MinSeverity)
        {
            RecentEvents.Add(Event);
        }
    }

    return RecentEvents;
}

TArray<FSecurityAuditEvent> USecurityAuditSubsystem::SearchSecurityEvents(const FString& SearchTerm, ESecurityEventType EventType)
{
    TArray<FSecurityAuditEvent> FoundEvents;

    for (const FSecurityAuditEvent& Event : SecurityEvents)
    {
        if (EventType == ESecurityEventType::None || Event.EventType == EventType)
        {
            if (Event.Description.Contains(SearchTerm) || Event.UserId.Contains(SearchTerm))
            {
                FoundEvents.Add(Event);
            }
        }
    }

    return FoundEvents;
}

TArray<FSecurityAuditEvent> USecurityAuditSubsystem::SearchSecurityEventsInTimeRange(const FString& SearchTerm, ESecurityEventType EventType, const FDateTime& StartTime, const FDateTime& EndTime)
{
    TArray<FSecurityAuditEvent> FoundEvents;

    for (const FSecurityAuditEvent& Event : SecurityEvents)
    {
        if (Event.Timestamp >= StartTime && Event.Timestamp <= EndTime)
        {
            if (EventType == ESecurityEventType::None || Event.EventType == EventType)
            {
                if (Event.Description.Contains(SearchTerm) || Event.UserId.Contains(SearchTerm))
                {
                    FoundEvents.Add(Event);
                }
            }
        }
    }

    return FoundEvents;
}

FString USecurityAuditSubsystem::GenerateSecurityReport(int32 Hours)
{
    FString Report;
    Report += TEXT("Security Report\n");
    Report += FString::Printf(TEXT("Generated: %s\n"), *FDateTime::Now().ToString());
    Report += FString::Printf(TEXT("Period: Last %d hours\n"), Hours);
    Report += TEXT("\n--- Metrics ---\n");
    Report += FString::Printf(TEXT("Total Events: %d\n"), CurrentMetrics.TotalEvents);
    Report += FString::Printf(TEXT("Critical Events: %d\n"), CurrentMetrics.CriticalEvents);
    Report += FString::Printf(TEXT("Cheat Detections: %d\n"), CurrentMetrics.CheatDetections);
    Report += FString::Printf(TEXT("Average Events Per Hour: %.2f\n"), CurrentMetrics.AverageEventsPerHour);
    Report += TEXT("\n--- High Severity Events ---\n");

    TArray<FSecurityAuditEvent> RecentEvents = GetRecentEvents(Hours, ESecuritySeverity::High);
    for (const FSecurityAuditEvent& Event : RecentEvents)
    {
        Report += FormatEventForLog(Event) + TEXT("\n");
    }

    return Report;
}

bool USecurityAuditSubsystem::ExportAuditLog(const FDateTime& StartTime, const FDateTime& EndTime, const FString& FilePath)
{
    TArray<FString> Lines;
    for (const FSecurityAuditEvent& Event : SecurityEvents)
    {
        if (Event.Timestamp >= StartTime && Event.Timestamp <= EndTime)
        {
            Lines.Add(FormatEventForLog(Event));
        }
    }

    return FFileHelper::SaveStringArrayToFile(Lines, *FilePath);
}

void USecurityAuditSubsystem::SetAlertThreshold(ESecurityEventType EventType, int32 Threshold, int32 TimeWindow)
{
    AlertThresholds.Add(EventType, Threshold);
    AlertTimeWindows.Add(EventType, TimeWindow);
}

void USecurityAuditSubsystem::EmergencyLockdown(const FString& Reason)
{
    bIsInLockdown = true;
    LockdownReason = Reason;
    LockdownTime = FDateTime::Now();
    LogSecurityEvent(ESecurityEventType::SystemCompromise, ESecuritySeverity::Critical, TEXT("SYSTEM"), FString::Printf(TEXT("Emergency lockdown initiated: %s"), *Reason));
}

void USecurityAuditSubsystem::ClearLockdown()
{
    bIsInLockdown = false;
    LogSecurityEvent(ESecurityEventType::SystemCompromise, ESecuritySeverity::Info, TEXT("SYSTEM"), TEXT("Emergency lockdown cleared."));
}

bool USecurityAuditSubsystem::IsInLockdown() const
{
    return bIsInLockdown;
}

void USecurityAuditSubsystem::ProcessSecurityEvents()
{
    CheckAlertThresholds();
}

void USecurityAuditSubsystem::CheckAlertThresholds()
{
    for (auto const& [EventType, Threshold] : AlertThresholds)
    {
        int32 TimeWindow = AlertTimeWindows.Contains(EventType) ? AlertTimeWindows[EventType] : 60;
        TArray<FSecurityAuditEvent> RecentEvents = GetRecentEvents(TimeWindow / 60, ESecuritySeverity::Info);

        int32 EventCount = 0;
        for (const FSecurityAuditEvent& Event : RecentEvents)
        {
            if (Event.EventType == EventType)
            {
                EventCount++;
            }
        }

        if (EventCount >= Threshold)
        {
            SendSecurityAlert(FString::Printf(TEXT("Threshold exceeded for %s"), *GetEventTypeString(EventType)), ESecuritySeverity::High);
        }
    }
}

void USecurityAuditSubsystem::SendSecurityAlert(const FString& AlertMessage, ESecuritySeverity Severity)
{
    UE_LOG(LogTemp, Warning, TEXT("SECURITY ALERT [%s]: %s"), *GetSeverityString(Severity), *AlertMessage);
}

void USecurityAuditSubsystem::CleanupOldEvents()
{
    FDateTime Cutoff = FDateTime::Now() - FTimespan(EVENT_RETENTION_DAYS, 0, 0, 0);
    SecurityEvents.RemoveAll([&Cutoff](const FSecurityAuditEvent& Event)
    {
        return Event.Timestamp < Cutoff;
    });
}

bool USecurityAuditSubsystem::WriteEventToFile(const FSecurityAuditEvent& Event)
{
    FString LogLine = FormatEventForLog(Event) + TEXT("\n");
    FString FilePath = FPaths::ProjectLogDir() + TEXT("SecurityAudit.log");
    return FFileHelper::SaveStringToFile(LogLine, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

bool USecurityAuditSubsystem::LoadEventsFromFile()
{
    FString FilePath = FPaths::ProjectLogDir() + TEXT("SecurityAudit.log");
    TArray<FString> Lines;
    if (FFileHelper::LoadFileToStringArray(Lines, *FilePath))
    {
        for (const FString& Line : Lines)
        {
            // This is a simplified parser. A real implementation would be more robust.
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT("] ["), true);
            if (Parts.Num() >= 4)
            {
                FSecurityAuditEvent Event;
                FDateTime::Parse(Parts[0].RightChop(1), Event.Timestamp);
                Event.EventType = static_cast<ESecurityEventType>(FCString::Atoi(*(FindObject<UEnum>(nullptr, TEXT("ESecurityEventType"), true)->GetNameStringByValue(FCString::Atoi(*Parts[1])))));
                Event.Severity = static_cast<ESecuritySeverity>(FCString::Atoi(*(FindObject<UEnum>(nullptr, TEXT("ESecuritySeverity"), true)->GetNameStringByValue(FCString::Atoi(*Parts[2])))));
                Event.Description = Parts[3].LeftChop(1);
                SecurityEvents.Add(Event);
            }
        }
        return true;
    }
    return false;
}

FString USecurityAuditSubsystem::GenerateEventId()
{
    return FGuid::NewGuid().ToString();
}

FString USecurityAuditSubsystem::FormatEventForLog(const FSecurityAuditEvent& Event)
{
    return FString::Printf(TEXT("[%s] [%s] [%s] %s"), *Event.Timestamp.ToString(), *GetEventTypeString(Event.EventType), *GetSeverityString(Event.Severity), *Event.Description);
}

FString USecurityAuditSubsystem::GetSeverityString(ESecuritySeverity Severity)
{
    switch (Severity)
    {
        case ESecuritySeverity::Info: return TEXT("Info");
        case ESecuritySeverity::Low: return TEXT("Low");
        case ESecuritySeverity::Medium: return TEXT("Medium");
        case ESecuritySeverity::High: return TEXT("High");
        case ESecuritySeverity::Critical: return TEXT("Critical");
        default: return TEXT("Unknown");
    }
}

FString USecurityAuditSubsystem::GetEventTypeString(ESecurityEventType EventType)
{
    const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("ESecurityEventType"), true);
    if (!EnumPtr) return FString("Unknown");
    return EnumPtr->GetNameStringByValue((int64)EventType);
}
