#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/GlobalEventBus.h"
#include "WorldStateLock.generated.h"

/**
 * Lock types for different world operations
 */
UENUM(BlueprintType)
enum class EWorldLockType : uint8
{
    None            UMETA(DisplayName = "None"),
    Save            UMETA(DisplayName = "Save Operation"),
    Load            UMETA(DisplayName = "Load Operation"),
    EcosystemUpdate UMETA(DisplayName = "Ecosystem Update"),
    FactionUpdate   UMETA(DisplayName = "Faction Update"),
    QuestUpdate     UMETA(DisplayName = "Quest Update"),
    Custom          UMETA(DisplayName = "Custom Lock")
};

/**
 * Priority levels for lock requests
 */
UENUM(BlueprintType)
enum class ELockPriority : uint8
{
    Low         UMETA(DisplayName = "Low"),
    Normal      UMETA(DisplayName = "Normal"),
    High        UMETA(DisplayName = "High"),
    Critical    UMETA(DisplayName = "Critical")
};

/**
 * Structure representing a world state lock request
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FWorldLockRequest
{
    GENERATED_BODY()

    // Type of lock being requested
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    EWorldLockType LockType = EWorldLockType::None;

    // Priority of this lock request
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    ELockPriority Priority = ELockPriority::Normal;

    // Requester identifier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    FString RequesterID = TEXT("");

    // Description of what this lock is for
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    FString Description = TEXT("");

    // Timestamp when lock was requested
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    float RequestTime = 0.0f;

    // Maximum time this lock should be held (0 = indefinite)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    float MaxLockTime = 0.0f;

    // Unique lock ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    FGuid LockID;

    // Whether this lock blocks other operations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    bool bExclusive = true;

    FORCEINLINE FWorldLockRequest()
        : LockType(EWorldLockType::None)
        , Priority(ELockPriority::Normal)
        , RequesterID(TEXT(""))
        , Description(TEXT(""))
        , RequestTime(0.0f)
        , MaxLockTime(0.0f)
        , LockID()
        , bExclusive(true)
    {
    }

    FORCEINLINE FWorldLockRequest(EWorldLockType InLockType, const FString& InRequesterID, const FString& InDescription,
        ELockPriority InPriority = ELockPriority::Normal, bool bInExclusive = true, float InMaxLockTime = 0.0f)
        : LockType(InLockType)
        , Priority(InPriority)
        , RequesterID(InRequesterID)
        , Description(InDescription)
        , RequestTime(0.0f)
        , MaxLockTime(InMaxLockTime)
        , LockID(FGuid::NewGuid())
        , bExclusive(bInExclusive)
    {
    }
};

/**
 * Delegate for lock state changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWorldLockStateChanged, EWorldLockType, LockType, bool, bLocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorldLockRequested, const FWorldLockRequest&, Request, bool, bGranted, const FString&, Reason);

/**
 * World State Lock Subsystem
 * 
 * This system manages exclusive access to world state operations to prevent
 * data corruption during save/load operations and major world state changes.
 * 
 * Key Features:
 * - Prevents ecosystem updates during save operations
 * - Ensures atomic world state changes
 * - Priority-based lock queue
 * - Timeout protection
 * - Integration with Global Event Bus
 */
UCLASS()
class DARKAGE_API UWorldStateLock : public UGameInstanceSubsystem, public IGlobalEventListener
{
    GENERATED_BODY()

public:
    UWorldStateLock();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    // IGlobalEventListener interface
    virtual TArray<EGlobalEventType> GetListenedEventTypes() const override;
    virtual int32 GetListenerPriority() const override { return 100; } // High priority

    // Tick function
    void Tick(float DeltaTime);

    // --- Lock Management ---

    /**
     * Request a world state lock
     * @param Request The lock request details
     * @return True if lock was granted immediately, false if queued
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock")
    bool RequestLock(const FWorldLockRequest& Request);

    /**
     * Request a simple lock with basic parameters
     * @param LockType Type of lock to request
     * @param RequesterID ID of the requesting system
     * @param Description Description of the operation
     * @param Priority Priority of the request
     * @param bExclusive Whether this is an exclusive lock
     * @param MaxLockTime Maximum time to hold the lock (0 = indefinite)
     * @return Lock ID if successful, invalid GUID if failed
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock")
    FGuid RequestSimpleLock(EWorldLockType LockType, const FString& RequesterID, const FString& Description,
        ELockPriority Priority = ELockPriority::Normal, bool bExclusive = true, float MaxLockTime = 30.0f);

    /**
     * Release a previously acquired lock
     * @param LockID The ID of the lock to release
     * @return True if lock was successfully released
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock")
    bool ReleaseLock(const FGuid& LockID);

    /**
     * Release all locks held by a specific requester
     * @param RequesterID The ID of the requester
     * @return Number of locks released
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock")
    int32 ReleaseAllLocks(const FString& RequesterID);

    /**
     * Force release all locks (emergency use only)
     */
    UFUNCTION(BlueprintCallable, Exec, Category = "World Lock|Admin")
    void ForceReleaseAllLocks();

    // --- Lock Status Queries ---

    /**
     * Check if a specific lock type is currently active
     * @param LockType The type of lock to check
     * @return True if this lock type is active
     */
    UFUNCTION(BlueprintPure, Category = "World Lock")
    bool IsLocked(EWorldLockType LockType) const;

    /**
     * Check if any exclusive lock is currently active
     * @return True if any exclusive lock is active
     */
    UFUNCTION(BlueprintPure, Category = "World Lock")
    bool IsAnyExclusiveLockActive() const;

    /**
     * Get the current lock holder for a specific lock type
     * @param LockType The lock type to query
     * @return The requester ID of the current lock holder, empty if not locked
     */
    UFUNCTION(BlueprintPure, Category = "World Lock")
    FString GetLockHolder(EWorldLockType LockType) const;

    /**
     * Get all currently active locks
     * @return Array of active lock requests
     */
    UFUNCTION(BlueprintPure, Category = "World Lock")
    TArray<FWorldLockRequest> GetActiveLocks() const;

    /**
     * Get all queued lock requests
     * @return Array of queued lock requests
     */
    UFUNCTION(BlueprintPure, Category = "World Lock")
    TArray<FWorldLockRequest> GetQueuedLocks() const;

    // --- Convenience Functions ---

    /**
     * Request a save operation lock (high priority, exclusive)
     * @param RequesterID ID of the requesting system
     * @return Lock ID if successful
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock|Convenience")
    FGuid RequestSaveLock(const FString& RequesterID);

    /**
     * Request a load operation lock (critical priority, exclusive)
     * @param RequesterID ID of the requesting system
     * @return Lock ID if successful
     */
    UFUNCTION(BlueprintCallable, Category = "World Lock|Convenience")
    FGuid RequestLoadLock(const FString& RequesterID);

    /**
     * Check if it's safe to perform a save operation
     * @return True if no conflicting operations are active
     */
    UFUNCTION(BlueprintPure, Category = "World Lock|Convenience")
    bool IsSafeToSave() const;

    /**
     * Check if it's safe to perform ecosystem updates
     * @return True if no save/load operations are active
     */
    UFUNCTION(BlueprintPure, Category = "World Lock|Convenience")
    bool IsSafeForEcosystemUpdate() const;

    // --- Events ---

    UPROPERTY(BlueprintAssignable, Category = "World Lock|Events")
    FOnWorldLockStateChanged OnLockStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "World Lock|Events")
    FOnWorldLockRequested OnLockRequested;

    // --- Debug Functions ---

    UFUNCTION(BlueprintCallable, Exec, Category = "World Lock|Debug")
    void PrintLockStatus() const;

    UFUNCTION(BlueprintCallable, Exec, Category = "World Lock|Debug")
    void SetLockLogging(bool bEnabled);

    // Static helper to get the world state lock instance
    static UWorldStateLock* Get(const UObject* WorldContext);

protected:
    // Process the lock queue
    void ProcessLockQueue();

    // Check if a lock request can be granted
    bool CanGrantLock(const FWorldLockRequest& Request) const;

    // Grant a lock request
    void GrantLock(const FWorldLockRequest& Request);

    // Check for expired locks
    void CheckForExpiredLocks();

    // Sort lock queue by priority
    void SortLockQueue();

    // Handle global events
    UFUNCTION(BlueprintImplementableEvent, Category = "Global Events")
    void OnGlobalEventReceived(const FGlobalEvent& Event);

private:
    // Currently active locks
    TMap<EWorldLockType, FWorldLockRequest> ActiveLocks;

    // Queue of pending lock requests
    TArray<FWorldLockRequest> LockQueue;

    // Map of lock IDs to their requests (for easy lookup)
    TMap<FGuid, FWorldLockRequest> LockIDMap;

    // Debug settings
    bool bLogLockOperations;

    // Performance tracking
    float TotalLockTime;
    int32 TotalLocksGranted;
    int32 TotalLocksReleased;

    // Helper functions
    FString LockTypeToString(EWorldLockType LockType) const;
    FString PriorityToString(ELockPriority Priority) const;
    bool DoesLockConflict(const FWorldLockRequest& Request, const FWorldLockRequest& ExistingLock) const;
};