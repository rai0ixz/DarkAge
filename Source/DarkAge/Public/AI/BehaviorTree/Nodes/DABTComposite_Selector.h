#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/DABTNode.h"
#include "DABTComposite_Selector.generated.h"

/**
 * Selector Composite Node
 * 
 * Executes children in order until one succeeds.
 * Returns Success if any child succeeds.
 * Returns Failure if all children fail.
 * 
 * This is the "OR" node of behavior trees - useful for trying multiple approaches
 * to achieve a goal until one works.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Selector"))
class DARKAGE_API UDABTComposite_Selector : public UDABTComposite
{
    GENERATED_BODY()

public:
    UDABTComposite_Selector();

    // Override base class methods
    virtual EBTNodeStatus ExecuteNodeNative(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeName() const override;
    virtual FString GetNodeDescription() const override;
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;

protected:
    /**
     * Whether to randomize the order of children
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
    bool bRandomizeOrder;

    /**
     * Whether to remember the last successful child and try it first next time
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
    bool bRememberLastSuccess;

private:
    // Index of the last child that succeeded
    int32 LastSuccessfulChildIndex;

    // Randomized order of children (if bRandomizeOrder is true)
    TArray<int32> RandomizedOrder;

    // Generate randomized order
    void GenerateRandomizedOrder();

    // Get the next child to execute
    int32 GetNextChildIndex();
};

/**
 * Sequence Composite Node
 * 
 * Executes children in order until one fails.
 * Returns Success if all children succeed.
 * Returns Failure if any child fails.
 * 
 * This is the "AND" node of behavior trees - useful for executing a series
 * of actions that must all succeed.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Sequence"))
class DARKAGE_API UDABTComposite_Sequence : public UDABTComposite
{
    GENERATED_BODY()

public:
    UDABTComposite_Sequence();

    // Override base class methods
    virtual EBTNodeStatus ExecuteNodeNative(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeName() const override;
    virtual FString GetNodeDescription() const override;
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;

protected:
    /**
     * Whether to continue from the last running child or restart from the beginning
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
    bool bResumeFromRunning;
};

/**
 * Policy for when to return Success
 */
UENUM(BlueprintType)
enum class EParallelSuccessPolicy : uint8
{
    RequireOne      UMETA(DisplayName = "Require One"),
    RequireAll      UMETA(DisplayName = "Require All")
};

/**
 * Policy for when to return Failure
 */
UENUM(BlueprintType)
enum class EParallelFailurePolicy : uint8
{
    RequireOne      UMETA(DisplayName = "Require One"),
    RequireAll      UMETA(DisplayName = "Require All")
};

/**
 * Parallel Composite Node
 *
 * Executes all children simultaneously.
 * Returns based on the specified policy.
 *
 * Useful for behaviors that need to happen at the same time,
 * like moving while watching for enemies.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Parallel"))
class DARKAGE_API UDABTComposite_Parallel : public UDABTComposite
{
    GENERATED_BODY()

public:

    UDABTComposite_Parallel();

    // Override base class methods
    virtual EBTNodeStatus ExecuteNodeNative(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeName() const override;
    virtual FString GetNodeDescription() const override;
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;

protected:
    /**
     * Policy for when to return Success
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parallel")
    EParallelSuccessPolicy SuccessPolicy;

    /**
     * Policy for when to return Failure
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parallel")
    EParallelFailurePolicy FailurePolicy;

private:
    // Status of each child
    TArray<EBTNodeStatus> ChildStatuses;

    // Check if we should return based on current child statuses
    EBTNodeStatus EvaluateParallelStatus();
};