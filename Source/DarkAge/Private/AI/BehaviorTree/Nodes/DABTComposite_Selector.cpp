// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/Nodes/DABTComposite_Selector.h"
#include "DarkAge.h"
#include "AI/BehaviorTree/BTBlackboard.h"

UDABTComposite_Selector::UDABTComposite_Selector()
{
    bRandomizeOrder = false;
    bRememberLastSuccess = false;
    LastSuccessfulChildIndex = -1;
}

EBTNodeStatus UDABTComposite_Selector::ExecuteNodeNative(UDABTBlackboard* Blackboard)
{
    int32 ChildIndex = GetNextChildIndex();
    while (Children.IsValidIndex(ChildIndex))
    {
        UDABTNode* ChildNode = Children[ChildIndex];
        if (ChildNode)
        {
            EBTNodeStatus Status = ChildNode->Execute(Blackboard);
            if (Status == EBTNodeStatus::Success)
            {
                if (bRememberLastSuccess)
                {
                    LastSuccessfulChildIndex = ChildIndex;
                }
                return EBTNodeStatus::Success;
            }
            if (Status == EBTNodeStatus::Running)
            {
                return EBTNodeStatus::Running;
            }
        }
        CurrentChildIndex++;
        ChildIndex = GetNextChildIndex();
    }
    return EBTNodeStatus::Failure;
}

FString UDABTComposite_Selector::GetNodeName() const
{
    return TEXT("Selector");
}

FString UDABTComposite_Selector::GetNodeDescription() const
{
    return TEXT("Executes children until one succeeds.");
}

void UDABTComposite_Selector::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    if (bRandomizeOrder)
    {
        GenerateRandomizedOrder();
    }
    CurrentChildIndex = 0;
}

void UDABTComposite_Selector::Abort(UDABTBlackboard* Blackboard)
{
    Super::Abort(Blackboard);
}

void UDABTComposite_Selector::GenerateRandomizedOrder()
{
    RandomizedOrder.Empty();
    for (int32 i = 0; i < Children.Num(); ++i)
    {
        RandomizedOrder.Add(i);
    }
    for (int32 i = RandomizedOrder.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        RandomizedOrder.Swap(i, j);
    }
}

int32 UDABTComposite_Selector::GetNextChildIndex()
{
    if (bRandomizeOrder)
    {
        if (RandomizedOrder.IsValidIndex(CurrentChildIndex))
        {
            return RandomizedOrder[CurrentChildIndex];
        }
    }
    return CurrentChildIndex;
}

UDABTComposite_Sequence::UDABTComposite_Sequence()
{
    bResumeFromRunning = false;
}

EBTNodeStatus UDABTComposite_Sequence::ExecuteNodeNative(UDABTBlackboard* Blackboard)
{
    while (Children.IsValidIndex(CurrentChildIndex))
    {
        UDABTNode* ChildNode = Children[CurrentChildIndex];
        if (ChildNode)
        {
            EBTNodeStatus Status = ChildNode->Execute(Blackboard);
            if (Status == EBTNodeStatus::Failure)
            {
                return EBTNodeStatus::Failure;
            }
            if (Status == EBTNodeStatus::Running)
            {
                return EBTNodeStatus::Running;
            }
        }
        CurrentChildIndex++;
    }
    return EBTNodeStatus::Success;
}

FString UDABTComposite_Sequence::GetNodeName() const
{
    return TEXT("Sequence");
}

FString UDABTComposite_Sequence::GetNodeDescription() const
{
    return TEXT("Executes children until one fails.");
}

void UDABTComposite_Sequence::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    if (!bResumeFromRunning)
    {
        CurrentChildIndex = 0;
    }
}

void UDABTComposite_Sequence::Abort(UDABTBlackboard* Blackboard)
{
    Super::Abort(Blackboard);
}

UDABTComposite_Parallel::UDABTComposite_Parallel()
{
    SuccessPolicy = EParallelSuccessPolicy::RequireAll;
    FailurePolicy = EParallelFailurePolicy::RequireOne;
}

EBTNodeStatus UDABTComposite_Parallel::ExecuteNodeNative(UDABTBlackboard* Blackboard)
{
    for (int32 i = 0; i < Children.Num(); ++i)
    {
        UDABTNode* ChildNode = Children[i];
        if (ChildNode && (!ChildStatuses.IsValidIndex(i) || ChildStatuses[i] == EBTNodeStatus::Running))
        {
            EBTNodeStatus Status = ChildNode->Execute(Blackboard);
            if (!ChildStatuses.IsValidIndex(i))
            {
                ChildStatuses.Add(Status);
            }
            else
            {
                ChildStatuses[i] = Status;
            }
        }
    }
    return EvaluateParallelStatus();
}

FString UDABTComposite_Parallel::GetNodeName() const
{
    return TEXT("Parallel");
}

FString UDABTComposite_Parallel::GetNodeDescription() const
{
    return TEXT("Executes all children simultaneously.");
}

void UDABTComposite_Parallel::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    ChildStatuses.Init(EBTNodeStatus::Running, Children.Num());
}

void UDABTComposite_Parallel::Abort(UDABTBlackboard* Blackboard)
{
    Super::Abort(Blackboard);
    for (UDABTNode* Child : Children)
    {
        if (Child)
        {
            Child->Abort(Blackboard);
        }
    }
}

EBTNodeStatus UDABTComposite_Parallel::EvaluateParallelStatus()
{
    int32 SuccessCount = 0;
    int32 FailureCount = 0;
    for (EBTNodeStatus Status : ChildStatuses)
    {
        if (Status == EBTNodeStatus::Success) SuccessCount++;
        if (Status == EBTNodeStatus::Failure) FailureCount++;
    }

    if (FailurePolicy == EParallelFailurePolicy::RequireOne && FailureCount >= 1)
    {
        return EBTNodeStatus::Failure;
    }
    if (FailurePolicy == EParallelFailurePolicy::RequireAll && FailureCount >= Children.Num())
    {
        return EBTNodeStatus::Failure;
    }
    
    if (SuccessPolicy == EParallelSuccessPolicy::RequireOne && SuccessCount >= 1)
    {
        return EBTNodeStatus::Success;
    }
    if (SuccessPolicy == EParallelSuccessPolicy::RequireAll && SuccessCount >= Children.Num())
    {
        return EBTNodeStatus::Success;
    }

    return EBTNodeStatus::Running;
}