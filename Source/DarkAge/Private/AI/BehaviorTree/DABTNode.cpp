// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/DABTNode.h"
#include "DarkAge.h"
#include "AI/BehaviorTree/BTBlackboard.h"

UDABTNode::UDABTNode()
{
    Priority = 0;
    bLogExecution = false;
    Parent = nullptr;
    CurrentStatus = EBTNodeStatus::Invalid;
    bInitialized = false;
    ExecutionStartTime = 0.0f;
}

EBTNodeStatus UDABTNode::Execute(UDABTBlackboard* Blackboard)
{
    if (bLogExecution)
    {
        LogNode(FString::Printf(TEXT("Executing Node: %s"), *GetNodeName()));
    }
    ExecutionStartTime = FPlatformTime::Seconds();
    CurrentStatus = ExecuteNode(Blackboard);
    return CurrentStatus;
}

void UDABTNode::Abort(UDABTBlackboard* Blackboard)
{
    if (bLogExecution)
    {
        LogNode(FString::Printf(TEXT("Aborting Node: %s"), *GetNodeName()));
    }
    CurrentStatus = EBTNodeStatus::Failure;
}

void UDABTNode::Initialize(UDABTBlackboard* Blackboard)
{
    if (bLogExecution)
    {
        LogNode(FString::Printf(TEXT("Initializing Node: %s"), *GetNodeName()));
    }
    bInitialized = true;
}

void UDABTNode::Cleanup(UDABTBlackboard* Blackboard)
{
    if (bLogExecution)
    {
        LogNode(FString::Printf(TEXT("Cleaning up Node: %s"), *GetNodeName()));
    }
    bInitialized = false;
}

FString UDABTNode::GetNodeName() const
{
    return GetClass()->GetName();
}

FString UDABTNode::GetNodeDescription() const
{
    return TEXT("Base BT Node");
}

bool UDABTNode::CanExecute(UDABTBlackboard* Blackboard) const
{
    return true;
}

UDABTNode* UDABTNode::GetRoot() const
{
    UDABTNode* Root = const_cast<UDABTNode*>(this);
    while (Root->GetParent() != nullptr)
    {
        Root = Root->GetParent();
    }
    return Root;
}

EBTNodeStatus UDABTNode::ExecuteNodeNative(UDABTBlackboard* Blackboard)
{
    return EBTNodeStatus::Invalid;
}

void UDABTNode::LogNode(const FString& Message, bool bIsError) const
{
    if (bIsError)
    {
        UE_LOG(LogTemp, Error, TEXT("[BTNode: %s] %s"), *GetNodeName(), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[BTNode: %s] %s"), *GetNodeName(), *Message);
    }
}

UDABTComposite::UDABTComposite()
{
    CurrentChildIndex = 0;
}

void UDABTComposite::AddChild(UDABTNode* Child)
{
    if (Child)
    {
        Children.Add(Child);
        Child->SetParent(this);
    }
}

void UDABTComposite::RemoveChild(UDABTNode* Child)
{
    if (Child)
    {
        Children.Remove(Child);
        Child->SetParent(nullptr);
    }
}

UDABTNode* UDABTComposite::GetChild(int32 Index) const
{
    if (Children.IsValidIndex(Index))
    {
        return Children[Index];
    }
    return nullptr;
}

void UDABTComposite::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    for (UDABTNode* Child : Children)
    {
        if (Child)
        {
            Child->Initialize(Blackboard);
        }
    }
    CurrentChildIndex = 0;
}

void UDABTComposite::Cleanup(UDABTBlackboard* Blackboard)
{
    Super::Cleanup(Blackboard);
    for (UDABTNode* Child : Children)
    {
        if (Child)
        {
            Child->Cleanup(Blackboard);
        }
    }
}

void UDABTComposite::Abort(UDABTBlackboard* Blackboard)
{
    Super::Abort(Blackboard);
    if (Children.IsValidIndex(CurrentChildIndex) && Children[CurrentChildIndex])
    {
        Children[CurrentChildIndex]->Abort(Blackboard);
    }
}

void UDABTComposite::SortChildrenByPriority()
{
    Children.Sort([](const UDABTNode& A, const UDABTNode& B) {
        return A.Priority > B.Priority;
    });
}

UDABTLeaf::UDABTLeaf()
{
    ActionDuration = 0.0f;
    bCanBeInterrupted = true;
    ActionStartTime = 0.0f;
}

bool UDABTLeaf::HasActionCompleted() const
{
    if (ActionDuration > 0)
    {
        return (FPlatformTime::Seconds() - ActionStartTime) >= ActionDuration;
    }
    return true;
}

UDABTDecorator::UDABTDecorator()
{
    Child = nullptr;
}

void UDABTDecorator::SetChild(UDABTNode* InChild)
{
    Child = InChild;
    if (Child)
    {
        Child->SetParent(GetParent());
    }
}

void UDABTDecorator::Initialize(UDABTBlackboard* Blackboard)
{
    Super::Initialize(Blackboard);
    if (Child)
    {
        Child->Initialize(Blackboard);
    }
}

void UDABTDecorator::Cleanup(UDABTBlackboard* Blackboard)
{
    Super::Cleanup(Blackboard);
    if (Child)
    {
        Child->Cleanup(Blackboard);
    }
}

void UDABTDecorator::Abort(UDABTBlackboard* Blackboard)
{
    Super::Abort(Blackboard);
    if (Child)
    {
        Child->Abort(Blackboard);
    }
}

bool UDABTDecorator::CheckConditionNative(UDABTBlackboard* Blackboard)
{
    return true;
}