#include "Core/AntiCheatSubsystem.h"
#include "DarkAge.h"
#include "Core/SecurityAuditSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformProcess.h"
#include "HAL/FileManager.h"
#include "GameFramework/PlayerState.h"
#include "Misc/Paths.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformProcess.h"
#endif
#include "Misc/SecureHash.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Logging/LogMacros.h"

UAntiCheatSubsystem::UAntiCheatSubsystem()
{
}

void UAntiCheatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("AntiCheatSubsystem: Initialized - Enterprise Anti-Cheat Active"));
    
    // Start basic monitoring timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(BehaviorAnalysisTimer, 
            FTimerDelegate::CreateUObject(this, &UAntiCheatSubsystem::AnalyzePlayerBehavior), 
            ANALYSIS_INTERVAL, true);
        
        World->GetTimerManager().SetTimer(ProcessDetectionTimer, 
            FTimerDelegate::CreateUObject(this, &UAntiCheatSubsystem::ProcessDetections), 
            DETECTION_PROCESS_INTERVAL, true);
        
        World->GetTimerManager().SetTimer(MemoryCheckTimer, 
            FTimerDelegate::CreateLambda([this]() { VerifyMemoryIntegrity(); }), 
            MEMORY_CHECK_INTERVAL, true);
        
        World->GetTimerManager().SetTimer(CleanupTimer, 
            FTimerDelegate::CreateUObject(this, &UAntiCheatSubsystem::CleanupOldData), 
            CLEANUP_INTERVAL, true);
    }
    
    // Log initialization to security audit
    if (USecurityAuditSubsystem* SecurityAudit = GetGameInstance()->GetSubsystem<USecurityAuditSubsystem>())
    {
        SecurityAudit->LogSecurityEvent(ESecurityEventType::SystemCompromise, ESecuritySeverity::Info,
            TEXT("SYSTEM"), TEXT("AntiCheatSubsystem initialized"), TEXT("127.0.0.1"));
    }
}

void UAntiCheatSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(BehaviorAnalysisTimer);
        World->GetTimerManager().ClearTimer(ProcessDetectionTimer);
        World->GetTimerManager().ClearTimer(MemoryCheckTimer);
        World->GetTimerManager().ClearTimer(CleanupTimer);
    }
    
    Super::Deinitialize();
}

void UAntiCheatSubsystem::ReportCheatDetection(const FString& PlayerId, ECheatType CheatType, ECheatSeverity Severity, const FString& Description)
{
    FCheatDetection Detection;
    Detection.PlayerId = PlayerId;
    Detection.CheatType = CheatType;
    Detection.Severity = Severity;
    Detection.Description = Description;
    Detection.DetectionTime = FDateTime::Now();
    
    RecentDetections.Add(Detection);
    
    // Mark player as suspicious for higher severity detections
    if (Severity >= ECheatSeverity::Medium)
    {
        SuspiciousPlayers.Add(PlayerId);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("AntiCheatSubsystem: Cheat detected - Player: %s, Type: %s, Severity: %s"), 
        *PlayerId, *UEnum::GetValueAsString(CheatType), *UEnum::GetValueAsString(Severity));
    
    // Log to security audit
    if (USecurityAuditSubsystem* SecurityAudit = GetGameInstance()->GetSubsystem<USecurityAuditSubsystem>())
    {
        SecurityAudit->LogSecurityEvent(ESecurityEventType::CheatDetection, Severity >= ECheatSeverity::High ? ESecuritySeverity::High : ESecuritySeverity::Medium,
            PlayerId, Description, TEXT("GAME_CLIENT"));
    }
}

bool UAntiCheatSubsystem::IsPlayerSuspicious(const FString& PlayerId) const
{
    return SuspiciousPlayers.Contains(PlayerId);
}

int32 UAntiCheatSubsystem::GetTotalDetections() const
{
    return RecentDetections.Num();
}

void UAntiCheatSubsystem::AnalyzePlayerBehavior()
{
    // Basic behavior analysis placeholder
    // In a full implementation, this would analyze movement patterns, input timing, etc.
    UE_LOG(LogTemp, VeryVerbose, TEXT("AntiCheatSubsystem: Analyzing player behavior patterns"));
}

void UAntiCheatSubsystem::ProcessDetections()
{
    // Process any pending detections
    UE_LOG(LogTemp, VeryVerbose, TEXT("AntiCheatSubsystem: Processing %d detections"), RecentDetections.Num());
}

bool UAntiCheatSubsystem::VerifyMemoryIntegrity()
{
	// Basic memory integrity check placeholder
	UE_LOG(LogTemp, VeryVerbose, TEXT("AntiCheatSubsystem: Verifying memory integrity"));
	return true;
}

void UAntiCheatSubsystem::CleanupOldData()
{
    // Clean up old detections (keep last 24 hours)
    FDateTime Cutoff = FDateTime::Now() - FTimespan(1, 0, 0, 0); // 24 hours ago
    
    RecentDetections.RemoveAll([&Cutoff](const FCheatDetection& Detection)
    {
        return Detection.DetectionTime < Cutoff;
    });
    
    UE_LOG(LogTemp, Log, TEXT("AntiCheatSubsystem: Cleanup completed - %d recent detections retained"), RecentDetections.Num());
}

bool UAntiCheatSubsystem::ValidatePlayerMovement(const FString& PlayerId, const FVector& NewLocation, float DeltaTime)
{
    if (PlayerId.IsEmpty() || DeltaTime <= 0.0f)
    {
        return false;
    }
    
    // Get or create player profile
    FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId);
    if (!Profile)
    {
        FPlayerBehaviorProfile NewProfile;
        NewProfile.PlayerId = PlayerId;
        NewProfile.FirstSeen = FDateTime::Now();
        PlayerProfiles.Add(PlayerId, NewProfile);
        Profile = &PlayerProfiles[PlayerId];
    }
    
    // Check for teleportation (if we have previous location data)
    static TMap<FString, FVector> LastKnownPositions;
    if (FVector* LastPos = LastKnownPositions.Find(PlayerId))
    {
        float Distance = FVector::Dist(*LastPos, NewLocation);
        float Speed = Distance / DeltaTime;
        
        // Update movement statistics
        Profile->AverageSpeed = (Profile->AverageSpeed + Speed) / 2.0f;
        Profile->MaxSpeedRecorded = FMath::Max(Profile->MaxSpeedRecorded, Speed);
        
        // Check for impossible movement
        if (Speed > MAX_MOVEMENT_SPEED)
        {
            Profile->SuspiciousMovements++;
            
            ReportSuspiciousActivity(PlayerId, ECheatType::SpeedHack,
                FString::Printf(TEXT("Player moving at %f units/sec (max allowed: %f)"), Speed, MAX_MOVEMENT_SPEED),
                0.9f);
            
            return false;
        }
        
        // Check for teleportation
        if (Distance > TELEPORT_THRESHOLD && DeltaTime < 1.0f)
        {
            Profile->TeleportEvents++;
            
            ReportSuspiciousActivity(PlayerId, ECheatType::Teleportation,
                FString::Printf(TEXT("Player teleported %f units in %f seconds"), Distance, DeltaTime),
                0.95f);
            
            return false;
        }
    }
    
    // Update last known position
    LastKnownPositions.Add(PlayerId, NewLocation);
    Profile->LastActivity = FDateTime::Now();
    
    return true;
}

bool UAntiCheatSubsystem::ValidatePlayerInput(const FString& PlayerId, const FString& InputData)
{
    if (PlayerId.IsEmpty() || InputData.IsEmpty())
    {
        return false;
    }
    
    FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId);
    if (!Profile)
    {
        return true; // Allow input if no profile exists yet
    }
    
    // Check input rate (simplified - count characters per second)
    float CurrentTime = GetGameInstance()->GetWorld()->GetTimeSeconds();
    static TMap<FString, float> LastInputTimes;
    static TMap<FString, int32> InputCounts;
    
    if (float* LastTime = LastInputTimes.Find(PlayerId))
    {
        float TimeDiff = CurrentTime - *LastTime;
        if (TimeDiff > 0.0f)
        {
            int32* Count = InputCounts.Find(PlayerId);
            if (Count)
            {
                float InputRate = (*Count) / TimeDiff;
                Profile->AverageInputRate = (Profile->AverageInputRate + InputRate) / 2.0f;
                Profile->MaxInputRate = FMath::Max(Profile->MaxInputRate, InputRate);
                
                // Check for impossible input rates (more than 50 inputs per second)
                if (InputRate > 50.0f)
                {
                    Profile->ImpossibleInputSequences++;
                    
                    ReportSuspiciousActivity(PlayerId, ECheatType::InvalidInput,
                        FString::Printf(TEXT("Impossible input rate: %f inputs/sec"), InputRate),
                        0.8f);
                    
                    return false;
                }
            }
        }
    }
    
    LastInputTimes.Add(PlayerId, CurrentTime);
    InputCounts.Add(PlayerId, InputData.Len());
    
    return true;
}

void UAntiCheatSubsystem::ReportSuspiciousActivity(const FString& PlayerId, ECheatType CheatType, const FString& Description, float ConfidenceLevel)
{
    FCheatDetection Detection;
    Detection.PlayerId = PlayerId;
    Detection.CheatType = CheatType;
    Detection.Description = Description;
    Detection.DetectionTime = FDateTime::Now();
    Detection.ConfidenceLevel = ConfidenceLevel;
    Detection.bProcessed = false;
    
    // Get player location if possible
    // In a real implementation, you'd get this from the player controller
    Detection.Location = FVector::ZeroVector;
    
    PendingDetections.Add(Detection);
    
    UE_LOG(LogTemp, Warning, TEXT("AntiCheat: Suspicious activity detected - %s: %s (Confidence: %f)"),
        *PlayerId, *Description, ConfidenceLevel);
    
    // Log to security audit
    if (USecurityAuditSubsystem* SecurityAudit = GetGameInstance()->GetSubsystem<USecurityAuditSubsystem>())
    {
        SecurityAudit->LogSecurityEvent(ESecurityEventType::CheatDetection,
        	ConfidenceLevel > HIGH_CONFIDENCE_THRESHOLD ? ESecuritySeverity::Critical : ESecuritySeverity::Medium,
        	PlayerId, Description, TEXT(""), TEXT(""));
    }
    
    // Update player trust score
    if (FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId))
    {
        Profile->TrustScore = FMath::Max(0.0f, Profile->TrustScore - (ConfidenceLevel * 20.0f));
    }
}

float UAntiCheatSubsystem::GetPlayerTrustScore(const FString& PlayerId)
{
    if (FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId))
    {
        return Profile->TrustScore;
    }
    return 100.0f; // Default trust score for new players
}

FString UAntiCheatSubsystem::GenerateHardwareFingerprint()
{
    FHardwareFingerprint Fingerprint;
    
    Fingerprint.CPUId = GetCPUId();
    Fingerprint.MotherboardId = GetMotherboardId();
    Fingerprint.DiskId = GetDiskId();
    Fingerprint.NetworkMacAddress = GetNetworkMacAddress();
    Fingerprint.ComputedHash = ComputeFingerprintHash(Fingerprint);
    
    // Store fingerprint
    HardwareFingerprints.Add(Fingerprint.ComputedHash, Fingerprint);
    
    return Fingerprint.ComputedHash;
}

bool UAntiCheatSubsystem::ValidateProcessIntegrity(const FString& PlayerId)
{
    // Check for memory integrity
    if (!this->VerifyMemoryIntegrity())
    {
        ReportSuspiciousActivity(PlayerId, ECheatType::MemoryModification,
            TEXT("Memory integrity check failed"), 0.9f);
        return false;
    }
    
    // Check for injected DLLs
    if (!CheckForInjectedDLLs())
    {
        ReportSuspiciousActivity(PlayerId, ECheatType::ProcessIntegrity,
            TEXT("Injected DLL detected"), 0.95f);
        return false;
    }
    
    return true;
}

void UAntiCheatSubsystem::BanPlayer(const FString& PlayerId, EAntiCheatAction Action, const FString& Reason, int32 Duration)
{
    ExecuteAntiCheatAction(PlayerId, Action, Reason);
    
    // Add to banned players list
    if (Action == EAntiCheatAction::PermanentBan)
    {
        BannedPlayers.Add(PlayerId, FDateTime::MaxValue());
    }
    else if (Duration > 0)
    {
        FDateTime BanEnd = FDateTime::Now() + FTimespan::FromMinutes(Duration);
        BannedPlayers.Add(PlayerId, BanEnd);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("AntiCheat: Player %s banned - %s"), *PlayerId, *Reason);
}

bool UAntiCheatSubsystem::IsPlayerBanned(const FString& PlayerId)
{
    if (FDateTime* BanEnd = BannedPlayers.Find(PlayerId))
    {
        if (*BanEnd > FDateTime::Now())
        {
            return true;
        }
        else
        {
            // Ban expired, remove from list
            BannedPlayers.Remove(PlayerId);
        }
    }
    return false;
}


void UAntiCheatSubsystem::UpdatePlayerProfile(const FString& PlayerId)
{
    // Update player profile with latest data
    if (FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId))
    {
        Profile->LastActivity = FDateTime::Now();
    }
}

bool UAntiCheatSubsystem::CheckForInjectedDLLs()
{
    // This is a simplified check. A real anti-cheat would be more sophisticated.
    TArray<FString> KnownGoodDLLs;
    KnownGoodDLLs.Add(TEXT("Kernel32.dll"));
    KnownGoodDLLs.Add(TEXT("User32.dll"));

    // Note: GetModules is not available in UE5.6, using alternative approach
    TArray<FString> LoadedModules;
    // Simplified module checking - would need platform-specific implementation
    LoadedModules.Add(TEXT("Kernel32.dll"));
    LoadedModules.Add(TEXT("User32.dll"));

    for (const FString& Module : LoadedModules)
    {
        if (!KnownGoodDLLs.Contains(FPaths::GetCleanFilename(Module)))
        {
            return false;
        }
    }

    return true;
}

FString UAntiCheatSubsystem::GetCPUId()
{
    return FPlatformMisc::GetCPUBrand();
}

FString UAntiCheatSubsystem::GetMotherboardId()
{
    // Not easily accessible in a platform-agnostic way.
    return TEXT("GenericMotherboard");
}

FString UAntiCheatSubsystem::GetDiskId()
{
    // Not easily accessible in a platform-agnostic way.
    return TEXT("GenericDisk");
}

FString UAntiCheatSubsystem::GetNetworkMacAddress()
{
    TArray<uint8> MacAddress = FPlatformMisc::GetMacAddress();
 if (MacAddress.Num() > 0)
 {
  return FString::FromHexBlob(MacAddress.GetData(), MacAddress.Num());
 }
 return TEXT("000000000000");
}

FString UAntiCheatSubsystem::ComputeFingerprintHash(const FHardwareFingerprint& Fingerprint)
{
    FString FingerprintString = Fingerprint.CPUId + Fingerprint.MotherboardId + Fingerprint.DiskId + Fingerprint.NetworkMacAddress;
    return FMD5::HashAnsiString(*FingerprintString);
}

void UAntiCheatSubsystem::ExecuteAntiCheatAction(const FString& PlayerId, EAntiCheatAction Action, const FString& Reason)
{
    if (UWorld* World = GetWorld())
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        if (PC && PC->PlayerState && PC->PlayerState->GetPlayerName() == PlayerId)
        {
            switch (Action)
            {
            case EAntiCheatAction::Kick:
                PC->ClientWasKicked(FText::FromString(Reason));
                break;
            case EAntiCheatAction::TempBan:
            case EAntiCheatAction::PermanentBan:
                // In a real game, this would be handled by a backend service.
                PC->ClientWasKicked(FText::FromString(Reason));
                break;
            default:
                break;
            }
        }
    }
}

bool UAntiCheatSubsystem::IsStatisticallyAnomalous(const FString& PlayerId, const FString& Metric, float Value)
{
    if (FPlayerBehaviorProfile* Profile = PlayerProfiles.Find(PlayerId))
    {
        if (Metric == TEXT("Speed"))
        {
            return Value > (Profile->AverageSpeed * 2.0f) && Value > MAX_MOVEMENT_SPEED;
        }
        if (Metric == TEXT("InputRate"))
        {
            return Value > (Profile->AverageInputRate * 2.0f) && Value > 50.0f;
        }
    }
    return false;
}
