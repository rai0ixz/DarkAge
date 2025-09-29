#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/GlobalEventBus.h"
#include "DABTNode.generated.h"

class UDABTBlackboard;
class UDABTComposite;

/**
 * Status returned by behavior tree nodes
 */
UENUM(BlueprintType)
enum class EBTNodeStatus : uint8
{
    Success     UMETA(DisplayName = "Success"),
    Failure     UMETA(DisplayName = "Failure"),
    Running     UMETA(DisplayName = "Running"),
    Invalid     UMETA(DisplayName = "Invalid")
};

/**
 * Base class for all behavior tree nodes
 * 
 * Behavior Trees provide a hierarchical, modular way to create complex AI behaviors.
 * Each node represents a specific behavior or decision point.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DARKAGE_API UDABTNode : public UObject
{
    GENERATED_BODY()

public:
    UDABTNode();

    /**
     * Execute this node
     * @param Blackboard The blackboard containing shared data
     * @return The status of execution
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    virtual EBTNodeStatus Execute(UDABTBlackboard* Blackboard);

    /**
     * Called when the node is aborted (interrupted by higher priority behavior)
     * @param Blackboard The blackboard containing shared data
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    virtual void Abort(UDABTBlackboard* Blackboard);

    /**
     * Called when the node is initialized (before first execution)
     * @param Blackboard The blackboard containing shared data
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    virtual void Initialize(UDABTBlackboard* Blackboard);

    /**
     * Called when the node is cleaned up (after execution completes)
     * @param Blackboard The blackboard containing shared data
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    virtual void Cleanup(UDABTBlackboard* Blackboard);

    /**
     * Get the display name for this node (for debugging/editor)
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    virtual FString GetNodeName() const;

    /**
     * Get the description of what this node does
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    virtual FString GetNodeDescription() const;

    /**
     * Check if this node can be executed
     * @param Blackboard The blackboard containing shared data
     * @return True if the node can execute
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    virtual bool CanExecute(UDABTBlackboard* Blackboard) const;

    // --- Node Hierarchy ---

    /**
     * Set the parent node
     */
    void SetParent(UDABTComposite* InParent) { Parent = InParent; }

    /**
     * Get the parent node
     */
    UDABTComposite* GetParent() const { return Parent; }

    /**
     * Get the root node of the tree
     */
    UDABTNode* GetRoot() const;

    // --- Node Properties ---

    /**
     * Priority of this node (higher values = higher priority)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    int32 Priority;

    /**
     * Whether this node should be logged for debugging
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree|Debug")
    bool bLogExecution;

    /**
     * Custom tags for this node (useful for queries and debugging)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    TArray<FName> NodeTags;

protected:
    /**
     * The actual execution logic - override this in derived classes
     * @param Blackboard The blackboard containing shared data
     * @return The status of execution
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Behavior Tree")
    EBTNodeStatus ExecuteNode(UDABTBlackboard* Blackboard);

    /**
     * Native execution logic - override this for C++ nodes
     * @param Blackboard The blackboard containing shared data
     * @return The status of execution
     */
    virtual EBTNodeStatus ExecuteNodeNative(UDABTBlackboard* Blackboard);

    /**
     * Log a message for this node
     */
    void LogNode(const FString& Message, bool bIsError = false) const;

private:
    // Parent node in the tree hierarchy
    UPROPERTY()
    UDABTComposite* Parent;

    // Current execution status
    EBTNodeStatus CurrentStatus;

    // Whether the node has been initialized
    bool bInitialized;

    // Execution start time (for performance tracking)
    float ExecutionStartTime;
};

/**
 * Base class for composite nodes (nodes that have children)
 */
UCLASS(Abstract)
class DARKAGE_API UDABTComposite : public UDABTNode
{
    GENERATED_BODY()

public:
    UDABTComposite();

    /**
     * Add a child node
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void AddChild(UDABTNode* Child);

    /**
     * Remove a child node
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void RemoveChild(UDABTNode* Child);

    /**
     * Get all child nodes
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    const TArray<UDABTNode*>& GetChildren() const { return Children; }

    /**
     * Get child at specific index
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    UDABTNode* GetChild(int32 Index) const;

    /**
     * Get number of children
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    int32 GetChildCount() const { return Children.Num(); }

    // Override base class methods
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual void Cleanup(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;

protected:
    /**
     * Child nodes
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    TArray<UDABTNode*> Children;

    /**
     * Currently executing child index
     */
    int32 CurrentChildIndex;

    /**
     * Sort children by priority
     */
    void SortChildrenByPriority();
};

/**
 * Base class for leaf nodes (nodes that perform actual actions)
 */
UCLASS(Abstract)
class DARKAGE_API UDABTLeaf : public UDABTNode
{
    GENERATED_BODY()

public:
    UDABTLeaf();

    /**
     * The duration this action should take (0 = instant)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    float ActionDuration;

    /**
     * Whether this action can be interrupted
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    bool bCanBeInterrupted;

protected:
    // Time when action started
    float ActionStartTime;

    /**
     * Check if the action duration has elapsed
     */
    bool HasActionCompleted() const;
};

/**
 * Base class for decorator nodes (nodes that modify child behavior)
 */
UCLASS(Abstract)
class DARKAGE_API UDABTDecorator : public UDABTNode
{
    GENERATED_BODY()

public:
    UDABTDecorator();

    /**
     * Set the child node
     */
    UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
    void SetChild(UDABTNode* Child);

    /**
     * Get the child node
     */
    UFUNCTION(BlueprintPure, Category = "Behavior Tree")
    UDABTNode* GetChild() const { return Child; }

    // Override base class methods
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual void Cleanup(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;

protected:
    /**
     * The child node this decorator modifies
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
    UDABTNode* Child;

    /**
     * Check if the decorator condition is met
     * @param Blackboard The blackboard containing shared data
     * @return True if the condition is satisfied
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Behavior Tree")
    bool CheckCondition(UDABTBlackboard* Blackboard);

    /**
     * Native condition check - override this for C++ decorators
     */
    virtual bool CheckConditionNative(UDABTBlackboard* Blackboard);
};