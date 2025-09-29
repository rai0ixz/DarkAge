#include "Core/WorldStateLock.h"
#include "Core/GlobalEventBus.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UWorldStateLock::UWorldStateLock()
    : bLogLockOperations(false)
    , TotalLockTime(0.0f)
    , TotalLocksGranted(0)
    , TotalLocksReleased(0)
{
}

void UWorldStateLock::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Initialized"));
    
    // Clear any existing data
    ActiveLocks.Empty();
    LockQueue.Empty();
    LockIDMap.Empty();
    
    // Register with the Global Event Bus
    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        TArray<EGlobalEventType> ListenedEvents = {
            EGlobalEventType::WorldStateChanged,
            EGlobalEventType::WorldSaveRequested,
            EGlobalEventType::WorldLoadRequested
        };
        EventBus->RegisterListener(ListenerInterface, ListenedEvents);
        
        UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Registered with Global Event Bus"));
    }
    
    bLogLockOperations = true; // Enable logging by default in development
}

void UWorldStateLock::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Deinitializing"));
    
    // Release all active locks
    ForceReleaseAllLocks();
    
    // Unregister from Global Event Bus
    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        EventBus->UnregisterListener(ListenerInterface);
    }
    
    Super::Deinitialize();
}

TArray<EGlobalEventType> UWorldStateLock::GetListenedEventTypes() const
{
    return {
        EGlobalEventType::WorldSaveRequested,
        EGlobalEventType::WorldLoadRequested,
        EGlobalEventType::WorldSaveCompleted,
        EGlobalEventType::WorldLoadCompleted
    };
}

void UWorldStateLock::Tick(float DeltaTime)
{
    // Process the lock queue
    ProcessLockQueue();
    
    // Check for expired locks
    CheckForExpiredLocks();
}

bool UWorldStateLock::RequestLock(const FWorldLockRequest& Request)
{
    FWorldLockRequest TimestampedRequest = Request;
    TimestampedRequest.RequestTime = GetWorld() ? GetWorld()->GetTimeSeconds() : FPlatformTime::Seconds();
    
    if (bLogLockOperations)
    {
        UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Lock requested - Type: %s, Requester: %s, Description: %s"), 
            *LockTypeToString(TimestampedRequest.LockType),
            *TimestampedRequest.RequesterID,
            *TimestampedRequest.Description);
    }
    
    // Check if we can grant the lock immediately
    if (CanGrantLock(TimestampedRequest))
    {
        GrantLock(TimestampedRequest);
        OnLockRequested.Broadcast(TimestampedRequest, true, TEXT("Lock granted immediately"));
        return true;
    }
    else
    {
        // Add to queue
        LockQueue.Add(TimestampedRequest);
        LockIDMap.Add(TimestampedRequest.LockID, TimestampedRequest);
        SortLockQueue();
        
        OnLockRequested.Broadcast(TimestampedRequest, false, TEXT("Lock queued"));
        
        if (bLogLockOperations)
        {
            UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Lock queued - %s"), 
                *TimestampedRequest.LockID.ToString());
        }
        
        return false;
    }
}

FGuid UWorldStateLock::RequestSimpleLock(EWorldLockType LockType, const FString& RequesterID, const FString& Description,
    ELockPriority Priority, bool bExclusive, float MaxLockTime)
{
    FWorldLockRequest Request(LockType, RequesterID, Description, Priority, bExclusive, MaxLockTime);
    
    if (RequestLock(Request))
    {
        return Request.LockID;
    }
    
    // Return the lock ID even if queued, so caller can track it
    return Request.LockID;
}

bool UWorldStateLock::ReleaseLock(const FGuid& LockID)
{
    // Check if it's an active lock
    for (auto& ActiveLockPair : ActiveLocks)
    {
        if (ActiveLockPair.Value.LockID == LockID)
        {
            FWorldLockRequest ReleasedLock = ActiveLockPair.Value;
            ActiveLocks.Remove(ActiveLockPair.Key);
            LockIDMap.Remove(LockID);
            
            TotalLocksReleased++;
            
            if (bLogLockOperations)
            {
                UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Lock released - Type: %s, Requester: %s"), 
                    *LockTypeToString(ReleasedLock.LockType),
                    *ReleasedLock.RequesterID);
            }
            
            // Broadcast event
            OnLockStateChanged.Broadcast(ReleasedLock.LockType, false);
            
            // Broadcast global event
            if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
            {
                TMap<FString, FString> EventData;
                EventData.Add(TEXT("LockType"), LockTypeToString(ReleasedLock.LockType));
                EventData.Add(TEXT("RequesterID"), ReleasedLock.RequesterID);
                EventData.Add(TEXT("LockID"), LockID.ToString());
                
                EventBus->BroadcastSimpleEvent(EGlobalEventType::WorldStateChanged, 
                    TEXT("WorldStateLock"), TEXT(""), EventData, EEventPriority::Normal, false);
            }
            
            return true;
        }
    }
    
    // Check if it's in the queue
    for (int32 i = LockQueue.Num() - 1; i >= 0; --i)
    {
        if (LockQueue[i].LockID == LockID)
        {
            FWorldLockRequest RemovedRequest = LockQueue[i];
            LockQueue.RemoveAt(i);
            LockIDMap.Remove(LockID);
            
            if (bLogLockOperations)
            {
                UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Queued lock removed - Type: %s, Requester: %s"), 
                    *LockTypeToString(RemovedRequest.LockType),
                    *RemovedRequest.RequesterID);
            }
            
            return true;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("WorldStateLock: Attempted to release unknown lock ID: %s"), 
        *LockID.ToString());
    return false;
}

int32 UWorldStateLock::ReleaseAllLocks(const FString& RequesterID)
{
    int32 ReleasedCount = 0;
    
    // Release active locks
    TArray<EWorldLockType> LocksToRemove;
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        if (ActiveLockPair.Value.RequesterID == RequesterID)
        {
            LocksToRemove.Add(ActiveLockPair.Key);
        }
    }
    
    for (EWorldLockType LockType : LocksToRemove)
    {
        if (FWorldLockRequest* Lock = ActiveLocks.Find(LockType))
        {
            ReleaseLock(Lock->LockID);
            ReleasedCount++;
        }
    }
    
    // Remove queued locks
    for (int32 i = LockQueue.Num() - 1; i >= 0; --i)
    {
        if (LockQueue[i].RequesterID == RequesterID)
        {
            LockIDMap.Remove(LockQueue[i].LockID);
            LockQueue.RemoveAt(i);
            ReleasedCount++;
        }
    }
    
    if (bLogLockOperations && ReleasedCount > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Released %d locks for requester: %s"), 
            ReleasedCount, *RequesterID);
    }
    
    return ReleasedCount;
}

void UWorldStateLock::ForceReleaseAllLocks()
{
    int32 ActiveCount = ActiveLocks.Num();
    int32 QueuedCount = LockQueue.Num();
    
    ActiveLocks.Empty();
    LockQueue.Empty();
    LockIDMap.Empty();
    
    UE_LOG(LogTemp, Warning, TEXT("WorldStateLock: Force released %d active locks and %d queued locks"), 
        ActiveCount, QueuedCount);
    
    // Broadcast events for all lock types
    for (int32 i = 0; i < static_cast<int32>(EWorldLockType::Custom) + 1; ++i)
    {
        OnLockStateChanged.Broadcast(static_cast<EWorldLockType>(i), false);
    }
}

bool UWorldStateLock::IsLocked(EWorldLockType LockType) const
{
    return ActiveLocks.Contains(LockType);
}

bool UWorldStateLock::IsAnyExclusiveLockActive() const
{
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        if (ActiveLockPair.Value.bExclusive)
        {
            return true;
        }
    }
    return false;
}

FString UWorldStateLock::GetLockHolder(EWorldLockType LockType) const
{
    if (const FWorldLockRequest* Lock = ActiveLocks.Find(LockType))
    {
        return Lock->RequesterID;
    }
    return FString();
}

TArray<FWorldLockRequest> UWorldStateLock::GetActiveLocks() const
{
    TArray<FWorldLockRequest> Result;
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        Result.Add(ActiveLockPair.Value);
    }
    return Result;
}

TArray<FWorldLockRequest> UWorldStateLock::GetQueuedLocks() const
{
    return LockQueue;
}

FGuid UWorldStateLock::RequestSaveLock(const FString& RequesterID)
{
    return RequestSimpleLock(EWorldLockType::Save, RequesterID, 
        TEXT("Save operation"), ELockPriority::High, true, 60.0f);
}

FGuid UWorldStateLock::RequestLoadLock(const FString& RequesterID)
{
    return RequestSimpleLock(EWorldLockType::Load, RequesterID, 
        TEXT("Load operation"), ELockPriority::Critical, true, 120.0f);
}

bool UWorldStateLock::IsSafeToSave() const
{
    // Safe to save if no ecosystem updates or other save/load operations are active
    return !IsLocked(EWorldLockType::EcosystemUpdate) && 
           !IsLocked(EWorldLockType::Load) &&
           !IsLocked(EWorldLockType::Save);
}

bool UWorldStateLock::IsSafeForEcosystemUpdate() const
{
    // Safe for ecosystem updates if no save/load operations are active
    return !IsLocked(EWorldLockType::Save) && 
           !IsLocked(EWorldLockType::Load);
}

void UWorldStateLock::PrintLockStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("=== World State Lock Status ==="));
    UE_LOG(LogTemp, Log, TEXT("Active Locks: %d"), ActiveLocks.Num());
    
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        const FWorldLockRequest& Lock = ActiveLockPair.Value;
        float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : FPlatformTime::Seconds();
        float LockDuration = CurrentTime - Lock.RequestTime;
        
        UE_LOG(LogTemp, Log, TEXT("  %s: %s (%s) - Duration: %.2fs"), 
            *LockTypeToString(Lock.LockType),
            *Lock.RequesterID,
            *Lock.Description,
            LockDuration);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Queued Locks: %d"), LockQueue.Num());
    
    for (const FWorldLockRequest& QueuedLock : LockQueue)
    {
        UE_LOG(LogTemp, Log, TEXT("  %s: %s (%s) - Priority: %s"), 
            *LockTypeToString(QueuedLock.LockType),
            *QueuedLock.RequesterID,
            *QueuedLock.Description,
            *PriorityToString(QueuedLock.Priority));
    }
    
    UE_LOG(LogTemp, Log, TEXT("Statistics:"));
    UE_LOG(LogTemp, Log, TEXT("  Total Locks Granted: %d"), TotalLocksGranted);
    UE_LOG(LogTemp, Log, TEXT("  Total Locks Released: %d"), TotalLocksReleased);
    UE_LOG(LogTemp, Log, TEXT("  Total Lock Time: %.2f seconds"), TotalLockTime);
    UE_LOG(LogTemp, Log, TEXT("=== End Lock Status ==="));
}

void UWorldStateLock::SetLockLogging(bool bEnabled)
{
    bLogLockOperations = bEnabled;
    UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Lock logging %s"), 
        bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

UWorldStateLock* UWorldStateLock::Get(const UObject* WorldContext)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UWorldStateLock>();
        }
    }
    return nullptr;
}

void UWorldStateLock::ProcessLockQueue()
{
    if (LockQueue.Num() == 0)
    {
        return;
    }
    
    // Process locks in priority order
    for (int32 i = LockQueue.Num() - 1; i >= 0; --i)
    {
        const FWorldLockRequest& Request = LockQueue[i];
        
        if (CanGrantLock(Request))
        {
            // Grant the lock
            GrantLock(Request);
            LockQueue.RemoveAt(i);
            
            if (bLogLockOperations)
            {
                UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Queued lock granted - Type: %s, Requester: %s"), 
                    *LockTypeToString(Request.LockType),
                    *Request.RequesterID);
            }
        }
    }
}

bool UWorldStateLock::CanGrantLock(const FWorldLockRequest& Request) const
{
    // Check if this lock type is already active
    if (ActiveLocks.Contains(Request.LockType))
    {
        return false;
    }
    
    // Check for conflicts with existing locks
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        if (DoesLockConflict(Request, ActiveLockPair.Value))
        {
            return false;
        }
    }
    
    return true;
}

void UWorldStateLock::GrantLock(const FWorldLockRequest& Request)
{
    ActiveLocks.Add(Request.LockType, Request);
    LockIDMap.Add(Request.LockID, Request);
    TotalLocksGranted++;
    
    if (bLogLockOperations)
    {
        UE_LOG(LogTemp, Log, TEXT("WorldStateLock: Lock granted - Type: %s, Requester: %s"), 
            *LockTypeToString(Request.LockType),
            *Request.RequesterID);
    }
    
    // Broadcast event
    OnLockStateChanged.Broadcast(Request.LockType, true);
    
    // Broadcast global event
    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
    {
        TMap<FString, FString> EventData;
        EventData.Add(TEXT("LockType"), LockTypeToString(Request.LockType));
        EventData.Add(TEXT("RequesterID"), Request.RequesterID);
        EventData.Add(TEXT("LockID"), Request.LockID.ToString());
        
        EventBus->BroadcastSimpleEvent(EGlobalEventType::WorldStateChanged, 
            TEXT("WorldStateLock"), TEXT(""), EventData, EEventPriority::Normal, false);
    }
}

void UWorldStateLock::CheckForExpiredLocks()
{
    float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : FPlatformTime::Seconds();
    
    TArray<EWorldLockType> ExpiredLocks;
    
    for (const auto& ActiveLockPair : ActiveLocks)
    {
        const FWorldLockRequest& Lock = ActiveLockPair.Value;
        
        if (Lock.MaxLockTime > 0.0f)
        {
            float LockDuration = CurrentTime - Lock.RequestTime;
            if (LockDuration > Lock.MaxLockTime)
            {
                ExpiredLocks.Add(Lock.LockType);
                
                UE_LOG(LogTemp, Warning, TEXT("WorldStateLock: Lock expired - Type: %s, Requester: %s, Duration: %.2fs"), 
                    *LockTypeToString(Lock.LockType),
                    *Lock.RequesterID,
                    LockDuration);
            }
        }
    }
    
    // Release expired locks
    for (EWorldLockType LockType : ExpiredLocks)
    {
        if (FWorldLockRequest* Lock = ActiveLocks.Find(LockType))
        {
            ReleaseLock(Lock->LockID);
        }
    }
}

void UWorldStateLock::SortLockQueue()
{
    LockQueue.Sort([](const FWorldLockRequest& A, const FWorldLockRequest& B)
    {
        // Higher priority values are processed first
        if (A.Priority != B.Priority)
        {
            return static_cast<int32>(A.Priority) > static_cast<int32>(B.Priority);
        }
        
        // If same priority, earlier requests are processed first
        return A.RequestTime < B.RequestTime;
    });
}


FString UWorldStateLock::LockTypeToString(EWorldLockType LockType) const
{
    switch (LockType)
    {
        case EWorldLockType::None: return TEXT("None");
        case EWorldLockType::Save: return TEXT("Save");
        case EWorldLockType::Load: return TEXT("Load");
        case EWorldLockType::EcosystemUpdate: return TEXT("EcosystemUpdate");
        case EWorldLockType::FactionUpdate: return TEXT("FactionUpdate");
        case EWorldLockType::QuestUpdate: return TEXT("QuestUpdate");
        case EWorldLockType::Custom: return TEXT("Custom");
        default: return TEXT("Unknown");
    }
}

FString UWorldStateLock::PriorityToString(ELockPriority Priority) const
{
    switch (Priority)
    {
        case ELockPriority::Low: return TEXT("Low");
        case ELockPriority::Normal: return TEXT("Normal");
        case ELockPriority::High: return TEXT("High");
        case ELockPriority::Critical: return TEXT("Critical");
        default: return TEXT("Unknown");
    }
}

bool UWorldStateLock::DoesLockConflict(const FWorldLockRequest& Request, const FWorldLockRequest& ExistingLock) const
{
    // If either lock is exclusive, they conflict
    if (Request.bExclusive || ExistingLock.bExclusive)
    {
        // Define specific conflict rules
        switch (Request.LockType)
        {
            case EWorldLockType::Save:
                // Save conflicts with ecosystem updates and other save/load operations
                return ExistingLock.LockType == EWorldLockType::EcosystemUpdate ||
                       ExistingLock.LockType == EWorldLockType::Load ||
                       ExistingLock.LockType == EWorldLockType::Save;
                
            case EWorldLockType::Load:
                // Load conflicts with everything
                return true;
                
            case EWorldLockType::EcosystemUpdate:
                // Ecosystem updates conflict with save/load operations
                return ExistingLock.LockType == EWorldLockType::Save ||
                       ExistingLock.LockType == EWorldLockType::Load;
                
            default:
                // Other locks only conflict if they're the same type
                return Request.LockType == ExistingLock.LockType;
        }
    }
    
    return false;
}