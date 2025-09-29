#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/DABTNode.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTLeaf_MoveTo.generated.h"

/**
 * Target location mode
 */
UENUM(BlueprintType)
enum class EMoveToTargetType : uint8
{
    FixedLocation       UMETA(DisplayName = "Fixed Location"),
    BlackboardKey       UMETA(DisplayName = "Blackboard Key"),
    TargetActor         UMETA(DisplayName = "Target Actor"),
    BlackboardActor     UMETA(DisplayName = "Blackboard Actor")
};

/**
 * Move To Target Location Behavior Tree Leaf Node
 *
 * This node moves the AI to a specified location using the navigation system.
 * It can target a specific location, an actor, or a blackboard key.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Move To"))
class DARKAGE_API UDABTLeaf_MoveTo : public UDABTLeaf
{
    GENERATED_BODY()

public:
    UDABTLeaf_MoveTo();

    // UDABTNode interface
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual EBTNodeStatus Execute(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeDescription() const override;

protected:

    /**
     * How to determine the target location
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    EMoveToTargetType TargetType;

    /**
     * Fixed location to move to (when TargetType is FixedLocation)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To", meta = (EditCondition = "TargetType == EMoveToTargetType::FixedLocation"))
    FVector TargetLocation;

    /**
     * Blackboard key containing the target location (when TargetType is BlackboardKey)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To", meta = (EditCondition = "TargetType == EMoveToTargetType::BlackboardKey"))
    FName LocationBlackboardKey;

    /**
     * Target actor to move to (when TargetType is TargetActor)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To", meta = (EditCondition = "TargetType == EMoveToTargetType::TargetActor"))
    AActor* TargetActor;

    /**
     * Blackboard key containing the target actor (when TargetType is BlackboardActor)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To", meta = (EditCondition = "TargetType == EMoveToTargetType::BlackboardActor"))
    FName ActorBlackboardKey;

    /**
     * Acceptable radius for reaching the target
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    float AcceptanceRadius;

    /**
     * Whether to stop on overlap with the target
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    bool bStopOnOverlap;

    /**
     * Whether to use pathfinding
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    bool bUsePathfinding;

    /**
     * Maximum time to spend trying to reach the target
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    float TimeoutDuration;

    /**
     * Whether to project the goal location to the navigation mesh
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move To")
    bool bProjectGoalOnNavmesh;

private:
    // Current movement request ID
    FAIRequestID MoveRequestID;
    
    // Time when movement started
    float MovementStartTime;
    
    // Cached AI controller
    UPROPERTY()
    AAIController* AIController;
    
    // Helper functions
    FVector GetTargetLocationFromSettings() const;
    bool IsValidTarget(const FVector& Target) const;
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};

/**
 * Wait Behavior Tree Leaf Node
 * 
 * This node makes the AI wait for a specified duration.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Wait"))
class DARKAGE_API UDABTLeaf_Wait : public UDABTLeaf
{
    GENERATED_BODY()

public:
    UDABTLeaf_Wait();

    // UBTNode interface
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual EBTNodeStatus Execute(UDABTBlackboard* Blackboard) override;
    virtual void Abort(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeDescription() const override;

protected:
    /**
     * Duration to wait (in seconds)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait")
    float WaitDuration;

    /**
     * Random deviation from the wait duration (±)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait")
    float RandomDeviation;

    /**
     * Blackboard key to read wait duration from (optional)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait")
    FName DurationBlackboardKey;

private:
    // Time when waiting started
    float WaitStartTime;
    
    // Actual duration for this execution (including random deviation)
    float ActualWaitDuration;
    
    // Helper functions
    float GetWaitDurationFromSettings() const;
};

/**
 * Types of conditions to check
 */
UENUM(BlueprintType)
enum class EConditionType : uint8
{
    BlackboardValue     UMETA(DisplayName = "Blackboard Value"),
    DistanceToTarget    UMETA(DisplayName = "Distance To Target"),
    HasTarget           UMETA(DisplayName = "Has Target"),
    HealthPercentage    UMETA(DisplayName = "Health Percentage"),
    CustomCondition     UMETA(DisplayName = "Custom Condition")
};

/**
 * Comparison operators
 */
UENUM(BlueprintType)
enum class EBTComparisonOperator : uint8
{
    Equal               UMETA(DisplayName = "Equal"),
    NotEqual            UMETA(DisplayName = "Not Equal"),
    Greater             UMETA(DisplayName = "Greater"),
    GreaterOrEqual      UMETA(DisplayName = "Greater Or Equal"),
    Less                UMETA(DisplayName = "Less"),
    LessOrEqual         UMETA(DisplayName = "Less Or Equal")
};

/**
 * Check Condition Behavior Tree Leaf Node
 *
 * This node checks various conditions and returns success or failure.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Check Condition"))
class DARKAGE_API UDABTLeaf_CheckCondition : public UDABTLeaf
{
    GENERATED_BODY()

public:
    UDABTLeaf_CheckCondition();

    // UBTNode interface
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual EBTNodeStatus Execute(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeDescription() const override;

protected:

    /**
     * Type of condition to check
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    EConditionType ConditionType;

    /**
     * Blackboard key to check (for BlackboardValue condition)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = "ConditionType == EConditionType::BlackboardValue"))
    FName BlackboardKey;

    /**
     * Comparison operator
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    EBTComparisonOperator ComparisonOperator;

    /**
     * Value to compare against
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    float ComparisonValue;

    /**
     * String value to compare against (for string comparisons)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FString ComparisonString;

    /**
     * Whether to invert the result
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    bool bInvertResult;

    /**
     * Custom condition function name (for CustomCondition type)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = "ConditionType == EConditionType::CustomCondition"))
    FName CustomConditionFunction;

private:
    // Helper functions
    bool EvaluateCondition() const;
    bool CompareValues(float Value1, float Value2, EBTComparisonOperator Operator) const;
    bool CompareStrings(const FString& String1, const FString& String2, EBTComparisonOperator Operator) const;
};

/**
 * Types of values to set
 */
UENUM(BlueprintType)
enum class EBTSetValueType : uint8
{
    Float               UMETA(DisplayName = "Float"),
    Int                 UMETA(DisplayName = "Integer"),
    Bool                UMETA(DisplayName = "Boolean"),
    String              UMETA(DisplayName = "String"),
    Vector              UMETA(DisplayName = "Vector"),
    Object              UMETA(DisplayName = "Object"),
    CopyFromKey         UMETA(DisplayName = "Copy From Another Key")
};

/**
 * Set Blackboard Value Behavior Tree Leaf Node
 *
 * This node sets a value in the blackboard.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Set Blackboard Value"))
class DARKAGE_API UDABTLeaf_SetBlackboardValue : public UDABTLeaf
{
    GENERATED_BODY()

public:
    UDABTLeaf_SetBlackboardValue();

    // UBTNode interface
    virtual void Initialize(UDABTBlackboard* Blackboard) override;
    virtual EBTNodeStatus Execute(UDABTBlackboard* Blackboard) override;
    virtual FString GetNodeDescription() const override;

protected:

    /**
     * Blackboard key to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName BlackboardKey;

    /**
     * Type of value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    EBTSetValueType ValueType;

    /**
     * Float value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::Float"))
    float FloatValue;

    /**
     * Integer value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::Int"))
    int32 IntValue;

    /**
     * Boolean value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::Bool"))
    bool BoolValue;

    /**
     * String value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::String"))
    FString StringValue;

    /**
     * Vector value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::Vector"))
    FVector VectorValue;

    /**
     * Object value to set
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::Object"))
    UObject* ObjectValue;

    /**
     * Source key to copy from (for CopyFromKey type)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBTSetValueType::CopyFromKey"))
    FName SourceBlackboardKey;
};