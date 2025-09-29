#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/BehaviorTree/DABTNode.h"
#include "AI/BehaviorTree/BTBlackboard.h"
#include "Core/GlobalEventBus.h"
#include "BTManager.generated.h"

/**
 * Behavior Tree execution state
 */
UENUM(BlueprintType)
enum class EBTExecutionState : uint8
{
    Inactive    UMETA(DisplayName = "Inactive"),
    Running     UMETA(DisplayName = "Running"),
    Paused      UMETA(DisplayName = "Paused"),
    Completed   UMETA(DisplayName = "Completed"),
    Aborted     UMETA(DisplayName = "Aborted")
};

/**
 * Behavior Tree Manager Component
 * 
 * This component manages the execution of behavior trees for AI characters.
 * It handles tree execution, blackboard management, and integration with
 * the global event system.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UDABTManager : public UActorComponent, public IGlobalEventListener
{
    GENERATED_BODY()

public:
    UDABTManager();

    // UActorComponent interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // IGlobalEventListener interface
    virtual TArray<EGlobalEventType> GetListenedEventTypes() const override;
    virtual int32 GetListenerPriority() const override { return 50; } // Medium priority

    // --- Behavior Tree Management ---

    /**
     * Start executing a behavior tree
     * @param RootNode The root node of the behavior tree
     * @return True if the tree was started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    bool StartBehaviorTree(UDABTNode* RootNode);

    /**
     * Stop the currently executing behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void StopBehaviorTree();

    /**
     * Pause the currently executing behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void PauseBehaviorTree();

    /**
     * Resume a paused behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void ResumeBehaviorTree();

    /**
     * Restart the behavior tree from the beginning
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void RestartBehaviorTree();

    /**
     * Get the current execution state
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    EBTExecutionState GetExecutionState() const { return ExecutionState; }

    /**
     * Get the current root node
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    UDABTNode* GetCurrentRootNode() const { return CurrentRootNode; }

    /**
     * Get the blackboard
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    UDABTBlackboard* GetBlackboard() const { return Blackboard; }

    // --- Blackboard Convenience Functions ---

    /**
     * Initialize the blackboard with common AI values
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Blackboard")
    void InitializeBlackboard();

    /**
     * Set the AI's target
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Blackboard")
    void SetTarget(AActor* Target);

    /**
     * Get the AI's target
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree|Blackboard")
    AActor* GetTarget() const;

    /**
     * Set the AI's patrol points
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Blackboard")
    void SetPatrolPoints(const TArray<FVector>& Points);

    /**
     * Get the AI's patrol points
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree|Blackboard")
    TArray<FVector> GetPatrolPoints() const;

    /**
     * Set the AI's home location
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Blackboard")
    void SetHomeLocation(const FVector& Location);

    /**
     * Get the AI's home location
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree|Blackboard")
    FVector GetHomeLocation() const;

    // --- Events ---

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBehaviorTreeStateChanged, EBTExecutionState, NewState);
    UPROPERTY(BlueprintAssignable, Category = "Behavior Tree|Events")
    FOnBehaviorTreeStateChanged OnStateChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBehaviorTreeCompleted, UDABTNode*, CompletedNode, EBTNodeStatus, FinalStatus);
    UPROPERTY(BlueprintAssignable, Category = "Behavior Tree|Events")
    FOnBehaviorTreeCompleted OnTreeCompleted;

    // --- Debug Functions ---

    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Debug")
    void SetDebugMode(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Behavior Tree|Debug")
    FString GetDebugInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Debug")
    void LogCurrentState() const;

protected:
    /**
     * The blackboard for this AI
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree")
    UDABTBlackboard* Blackboard;

    /**
     * Default behavior tree to start with
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    UDABTNode* DefaultBehaviorTree;

    /**
     * Whether to start the default behavior tree automatically
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    bool bAutoStartDefaultTree;

    /**
     * Whether to restart the tree when it completes
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    bool bLoopBehaviorTree;

    /**
     * Update frequency for the behavior tree (in seconds)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    float UpdateFrequency;

    /**
     * Whether debug mode is enabled
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree|Debug")
    bool bDebugMode;

    // Handle global events
    UFUNCTION(BlueprintImplementableEvent, Category = "Global Events")
    void OnGlobalEventReceived(const FGlobalEvent& Event);

private:
    // Current execution state
    EBTExecutionState ExecutionState;

    // Currently executing root node
    UPROPERTY()
    UDABTNode* CurrentRootNode;

    // Current node status
    EBTNodeStatus CurrentStatus;

    // Time since last update
    float TimeSinceLastUpdate;

    // References to other subsystems
    UPROPERTY()
    UGlobalEventBus* GlobalEventBus;

    // Execution statistics
    int32 TotalExecutions;
    float TotalExecutionTime;
    float LastExecutionTime;

    // Helper functions
    void RegisterWithEventBus();
    void UnregisterFromEventBus();
    void SetExecutionState(EBTExecutionState NewState);
    void UpdateBehaviorTree(float DeltaTime);
    void HandleTreeCompletion(EBTNodeStatus FinalStatus);

    // Common blackboard keys
    static const FName TargetActorKey;
    static const FName PatrolPointsKey;
    static const FName HomeLocationKey;
    static const FName CurrentPatrolIndexKey;
    static const FName AlertLevelKey;
    static const FName LastKnownPlayerLocationKey;
};

/**
 * Utility class for creating and managing behavior trees
 */
UCLASS(BlueprintType)
class DARKAGE_API UBTUtilities : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Create a simple patrol behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Utilities")
    static UDABTNode* CreatePatrolBehaviorTree(const TArray<FVector>& PatrolPoints);

    /**
     * Create a simple guard behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Utilities")
    static UDABTNode* CreateGuardBehaviorTree(const FVector& GuardLocation, float GuardRadius = 500.0f);

    /**
     * Create a simple follow behavior tree
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Utilities")
    static UDABTNode* CreateFollowBehaviorTree(AActor* TargetToFollow, float FollowDistance = 200.0f);

    /**
     * Create a behavior tree from a configuration
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree|Utilities")
    static UDABTNode* CreateBehaviorTreeFromConfig(const FString& ConfigString);
};