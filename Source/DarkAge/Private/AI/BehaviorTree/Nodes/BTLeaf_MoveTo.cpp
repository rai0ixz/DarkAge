#include "AI/BehaviorTree/Nodes/BTLeaf_MoveTo.h"
#include "AI/BehaviorTree/DABTNode.h"
#include "AI/BehaviorTree/BTBlackboard.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

// --- UDABTLeaf_MoveTo Implementation ---

UDABTLeaf_MoveTo::UDABTLeaf_MoveTo()
{
    // NodeName will be set via GetNodeName() override
    
    // Default settings
    TargetType = EMoveToTargetType::BlackboardKey;
    TargetLocation = FVector::ZeroVector;
    LocationBlackboardKey = TEXT("TargetLocation");
    TargetActor = nullptr;
    ActorBlackboardKey = TEXT("TargetActor");
    AcceptanceRadius = 50.0f;
    bStopOnOverlap = true;
    bUsePathfinding = true;
    TimeoutDuration = 10.0f;
    bProjectGoalOnNavmesh = true;
    
    // Initialize runtime variables
    MoveRequestID = FAIRequestID::InvalidRequest;
    MovementStartTime = 0.0f;
    AIController = nullptr;
}

void UDABTLeaf_MoveTo::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    
    // Get AI controller from blackboard owner
    if (Blackboard)
    {
        if (AActor* Owner = Blackboard->GetOwner())
        {
            if (APawn* Pawn = Cast<APawn>(Owner))
            {
                AIController = Cast<AAIController>(Pawn->GetController());
            }
        }
    }
    
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("BTLeaf_MoveTo: Owner must have an AIController"));
    }
}

EBTNodeStatus UDABTLeaf_MoveTo::Execute(UDABTBlackboard* Blackboard)
{
    if (!AIController)
    {
        return EBTNodeStatus::Failure;
    }
    
    // If we're not currently moving, start a new move
    if (MoveRequestID == FAIRequestID::InvalidRequest)
    {
        FVector TargetLoc = GetTargetLocationFromSettings();
        
        if (!IsValidTarget(TargetLoc))
        {
            UE_LOG(LogTemp, Warning, TEXT("BTLeaf_MoveTo: Invalid target location"));
            return EBTNodeStatus::Failure;
        }
        
        // Create move request
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(TargetLoc);
        MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
        // Note: SetStopOnOverlap and SetProjectGoalOnNavigation are not available in UE5
        // These would need to be handled differently or removed
        MoveRequest.SetUsePathfinding(bUsePathfinding);
        
        // Start the move
        FPathFollowingRequestResult RequestResult = AIController->MoveTo(MoveRequest);
        MoveRequestID = RequestResult.MoveId;
        if (UWorld* World = GetWorld())
        {
            MovementStartTime = World->GetTimeSeconds();
        }
        
        if (RequestResult.Code == EPathFollowingRequestResult::Failed)
        {
            UE_LOG(LogTemp, Warning, TEXT("BTLeaf_MoveTo: Failed to start movement"));
            return EBTNodeStatus::Failure;
        }
        
        // Bind to completion delegate
        if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
        {
            PathFollowingComp->OnRequestFinished.AddUObject(this, &UDABTLeaf_MoveTo::OnMoveCompleted);
        }
    }
    
    // Check for timeout
    if (UWorld* World = GetWorld())
    {
        if (TimeoutDuration > 0.0f &&
            World->GetTimeSeconds() - MovementStartTime > TimeoutDuration)
        {
            UE_LOG(LogTemp, Warning, TEXT("BTLeaf_MoveTo: Movement timed out"));
            Abort(Blackboard);
            return EBTNodeStatus::Failure;
        }
    }
    
    // Check movement status
    if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
    {
        EPathFollowingStatus::Type Status = PathFollowingComp->GetStatus();
        
        switch (Status)
        {
            case EPathFollowingStatus::Moving:
                return EBTNodeStatus::Running;
                
            case EPathFollowingStatus::Idle:
                // Movement completed successfully
                MoveRequestID = FAIRequestID::InvalidRequest;
                return EBTNodeStatus::Success;
                
            case EPathFollowingStatus::Waiting:
                return EBTNodeStatus::Running;
                
            case EPathFollowingStatus::Paused:
                return EBTNodeStatus::Running;
                
            default:
                // Movement failed
                MoveRequestID = FAIRequestID::InvalidRequest;
                return EBTNodeStatus::Failure;
        }
    }
    
    return EBTNodeStatus::Running;
}

void UDABTLeaf_MoveTo::Abort(UDABTBlackboard* Blackboard)
{
    if (AIController && MoveRequestID != FAIRequestID::InvalidRequest)
    {
        AIController->StopMovement();
        
        // Unbind from completion delegate
        if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
        {
            PathFollowingComp->OnRequestFinished.RemoveAll(this);
        }
        
        MoveRequestID = FAIRequestID::InvalidRequest;
    }
}

FString UDABTLeaf_MoveTo::GetNodeDescription() const
{
    FString Description = TEXT("Move To: ");
    
    switch (TargetType)
    {
        case EMoveToTargetType::FixedLocation:
            Description += FString::Printf(TEXT("Location %s"), *TargetLocation.ToString());
            break;
            
        case EMoveToTargetType::BlackboardKey:
            Description += FString::Printf(TEXT("BB Key '%s'"), *LocationBlackboardKey.ToString());
            break;
            
        case EMoveToTargetType::TargetActor:
            Description += FString::Printf(TEXT("Actor %s"), 
                TargetActor ? *TargetActor->GetName() : TEXT("None"));
            break;
            
        case EMoveToTargetType::BlackboardActor:
            Description += FString::Printf(TEXT("BB Actor '%s'"), *ActorBlackboardKey.ToString());
            break;
    }
    
    Description += FString::Printf(TEXT(" (Radius: %.1f)"), AcceptanceRadius);
    
    return Description;
}

FVector UDABTLeaf_MoveTo::GetTargetLocationFromSettings() const
{
    switch (TargetType)
    {
        case EMoveToTargetType::FixedLocation:
            return TargetLocation;
            
        case EMoveToTargetType::BlackboardKey:
            if (AIController && AIController->GetBlackboardComponent())
            {
                return AIController->GetBlackboardComponent()->GetValueAsVector(LocationBlackboardKey);
            }
            break;
            
        case EMoveToTargetType::TargetActor:
            if (TargetActor)
            {
                return TargetActor->GetActorLocation();
            }
            break;
            
        case EMoveToTargetType::BlackboardActor:
            if (AIController && AIController->GetBlackboardComponent())
            {
                if (AActor* Actor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(ActorBlackboardKey)))
                {
                    return Actor->GetActorLocation();
                }
            }
            break;
    }
    
    return FVector::ZeroVector;
}

bool UDABTLeaf_MoveTo::IsValidTarget(const FVector& Target) const
{
    // Check if the target is not zero vector (unless that's intentional)
    if (Target.IsZero() && TargetType != EMoveToTargetType::FixedLocation)
    {
        return false;
    }
    
    // Additional validation could be added here
    // For example, checking if the target is within navigation bounds
    
    return true;
}

void UDABTLeaf_MoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (RequestID == MoveRequestID)
    {
        MoveRequestID = FAIRequestID::InvalidRequest;
        
        // Unbind from completion delegate
        if (AIController)
        {
            if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
            {
                PathFollowingComp->OnRequestFinished.RemoveAll(this);
            }
        }
    }
}

// --- UDABTLeaf_Wait Implementation ---

UDABTLeaf_Wait::UDABTLeaf_Wait()
{
    // NodeName will be set via GetNodeName() override
    
    // Default settings
    WaitDuration = 1.0f;
    RandomDeviation = 0.0f;
    DurationBlackboardKey = NAME_None;
    
    // Initialize runtime variables
    WaitStartTime = 0.0f;
    ActualWaitDuration = 0.0f;
}

void UDABTLeaf_Wait::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
}

EBTNodeStatus UDABTLeaf_Wait::Execute(UDABTBlackboard* Blackboard)
{
    // If we're just starting, calculate the actual wait duration
    if (UWorld* World = GetWorld())
    {
        if (WaitStartTime == 0.0f)
        {
            WaitStartTime = World->GetTimeSeconds();
            ActualWaitDuration = GetWaitDurationFromSettings();

            // Apply random deviation
            if (RandomDeviation > 0.0f)
            {
                float Deviation = FMath::RandRange(-RandomDeviation, RandomDeviation);
                ActualWaitDuration = FMath::Max(0.0f, ActualWaitDuration + Deviation);
            }
        }

        // Check if wait time has elapsed
        float ElapsedTime = World->GetTimeSeconds() - WaitStartTime;
        if (ElapsedTime >= ActualWaitDuration)
        {
            // Reset for next execution
            WaitStartTime = 0.0f;
            return EBTNodeStatus::Success;
        }
    }

    return EBTNodeStatus::Running;
}
        

void UDABTLeaf_Wait::Abort(UDABTBlackboard* Blackboard)
{
    // Reset wait timer
    WaitStartTime = 0.0f;
}

FString UDABTLeaf_Wait::GetNodeDescription() const
{
    FString Description = FString::Printf(TEXT("Wait: %.1fs"), WaitDuration);
    
    if (RandomDeviation > 0.0f)
    {
        Description += FString::Printf(TEXT(" (±%.1fs)"), RandomDeviation);
    }
    
    if (DurationBlackboardKey != NAME_None)
    {
        Description += FString::Printf(TEXT(" [BB: %s]"), *DurationBlackboardKey.ToString());
    }
    
    return Description;
}

float UDABTLeaf_Wait::GetWaitDurationFromSettings() const
{
    // TODO: Implement proper blackboard access when UDABTBlackboard is fully functional
    // For now, return the default duration
    return WaitDuration;
}

// --- UDABTLeaf_CheckCondition Implementation ---

UDABTLeaf_CheckCondition::UDABTLeaf_CheckCondition()
{
    // NodeName will be set via GetNodeName() override
    
    // Default settings
    ConditionType = EConditionType::BlackboardValue;
    BlackboardKey = NAME_None;
    ComparisonOperator = EBTComparisonOperator::Equal;
    ComparisonValue = 0.0f;
    ComparisonString = TEXT("");
    bInvertResult = false;
    CustomConditionFunction = NAME_None;
}

void UDABTLeaf_CheckCondition::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
}

EBTNodeStatus UDABTLeaf_CheckCondition::Execute(UDABTBlackboard* Blackboard)
{
    bool ConditionResult = EvaluateCondition();
    
    // Apply inversion if requested
    if (bInvertResult)
    {
        ConditionResult = !ConditionResult;
    }
    
    return ConditionResult ? EBTNodeStatus::Success : EBTNodeStatus::Failure;
}

FString UDABTLeaf_CheckCondition::GetNodeDescription() const
{
    FString Description = TEXT("Check: ");
    
    switch (ConditionType)
    {
        case EConditionType::BlackboardValue:
            Description += FString::Printf(TEXT("BB['%s'] %s %.1f"), 
                *BlackboardKey.ToString(),
                *UEnum::GetValueAsString(ComparisonOperator),
                ComparisonValue);
            break;
            
        case EConditionType::DistanceToTarget:
            Description += FString::Printf(TEXT("Distance %s %.1f"), 
                *UEnum::GetValueAsString(ComparisonOperator),
                ComparisonValue);
            break;
            
        case EConditionType::HasTarget:
            Description += TEXT("Has Target");
            break;
            
        case EConditionType::HealthPercentage:
            Description += FString::Printf(TEXT("Health %s %.1f%%"), 
                *UEnum::GetValueAsString(ComparisonOperator),
                ComparisonValue);
            break;
            
        case EConditionType::CustomCondition:
            Description += FString::Printf(TEXT("Custom: %s"), *CustomConditionFunction.ToString());
            break;
    }
    
    if (bInvertResult)
    {
        Description = TEXT("NOT (") + Description + TEXT(")");
    }
    
    return Description;
}

bool UDABTLeaf_CheckCondition::EvaluateCondition() const
{
    // TODO: Implement proper condition evaluation when UDABTBlackboard is fully functional
    // For now, return a simple default
    return true;
}

bool UDABTLeaf_CheckCondition::CompareValues(float Value1, float Value2, EBTComparisonOperator Operator) const
{
    switch (Operator)
    {
        case EBTComparisonOperator::Equal:
            return FMath::IsNearlyEqual(Value1, Value2, 0.01f);
            
        case EBTComparisonOperator::NotEqual:
            return !FMath::IsNearlyEqual(Value1, Value2, 0.01f);
            
        case EBTComparisonOperator::Greater:
            return Value1 > Value2;
            
        case EBTComparisonOperator::GreaterOrEqual:
            return Value1 >= Value2;
            
        case EBTComparisonOperator::Less:
            return Value1 < Value2;
            
        case EBTComparisonOperator::LessOrEqual:
            return Value1 <= Value2;
    }
    
    return false;
}

bool UDABTLeaf_CheckCondition::CompareStrings(const FString& String1, const FString& String2, EBTComparisonOperator Operator) const
{
    switch (Operator)
    {
        case EBTComparisonOperator::Equal:
            return String1.Equals(String2);
            
        case EBTComparisonOperator::NotEqual:
            return !String1.Equals(String2);
            
        default:
            // Other operators don't make sense for strings
            return false;
    }
}

// --- UDABTLeaf_SetBlackboardValue Implementation ---

UDABTLeaf_SetBlackboardValue::UDABTLeaf_SetBlackboardValue()
{
    // NodeName will be set via GetNodeName() override
    
    // Default settings
    BlackboardKey = NAME_None;
    ValueType = EBTSetValueType::Float;
    FloatValue = 0.0f;
    IntValue = 0;
    BoolValue = false;
    StringValue = TEXT("");
    VectorValue = FVector::ZeroVector;
    ObjectValue = nullptr;
    SourceBlackboardKey = NAME_None;
}

void UDABTLeaf_SetBlackboardValue::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
}

EBTNodeStatus UDABTLeaf_SetBlackboardValue::Execute(UDABTBlackboard* Blackboard)
{
    if (!Blackboard || BlackboardKey == NAME_None)
    {
        return EBTNodeStatus::Failure;
    }
    
    // TODO: Implement proper blackboard value setting when UDABTBlackboard methods are available
    // For now, return success to avoid compilation errors
    UE_LOG(LogTemp, Log, TEXT("BTLeaf_SetBlackboardValue: Setting value for key '%s' - implementation pending"), *BlackboardKey.ToString());
    
    return EBTNodeStatus::Success;
}

FString UDABTLeaf_SetBlackboardValue::GetNodeDescription() const
{
    FString Description = FString::Printf(TEXT("Set BB['%s'] = "), *BlackboardKey.ToString());
    
    switch (ValueType)
    {
        case EBTSetValueType::Float:
            Description += FString::Printf(TEXT("%.2f"), FloatValue);
            break;
            
        case EBTSetValueType::Int:
            Description += FString::Printf(TEXT("%d"), IntValue);
            break;
            
        case EBTSetValueType::Bool:
            Description += BoolValue ? TEXT("true") : TEXT("false");
            break;
            
        case EBTSetValueType::String:
            Description += FString::Printf(TEXT("\"%s\""), *StringValue);
            break;
            
        case EBTSetValueType::Vector:
            Description += VectorValue.ToString();
            break;
            
        case EBTSetValueType::Object:
            Description += ObjectValue ? ObjectValue->GetName() : TEXT("null");
            break;
            
        case EBTSetValueType::CopyFromKey:
            Description += FString::Printf(TEXT("BB['%s']"), *SourceBlackboardKey.ToString());
            break;
    }
    
    return Description;
}