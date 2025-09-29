#include "AI/BehaviorTree/BTManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Static blackboard key definitions
const FName UDABTManager::TargetActorKey = TEXT("TargetActor");
const FName UDABTManager::PatrolPointsKey = TEXT("PatrolPoints");
const FName UDABTManager::HomeLocationKey = TEXT("HomeLocation");
const FName UDABTManager::CurrentPatrolIndexKey = TEXT("CurrentPatrolIndex");
const FName UDABTManager::AlertLevelKey = TEXT("AlertLevel");
const FName UDABTManager::LastKnownPlayerLocationKey = TEXT("LastKnownPlayerLocation");

UDABTManager::UDABTManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    
    // Initialize default values
    ExecutionState = EBTExecutionState::Inactive;
    CurrentRootNode = nullptr;
    CurrentStatus = EBTNodeStatus::Invalid;
    TimeSinceLastUpdate = 0.0f;
    UpdateFrequency = 0.1f; // 10 times per second by default
    bAutoStartDefaultTree = true;
    bLoopBehaviorTree = true;
    bDebugMode = false;
    
    // Statistics
    TotalExecutions = 0;
    TotalExecutionTime = 0.0f;
    LastExecutionTime = 0.0f;
    
    // Create blackboard
    Blackboard = CreateDefaultSubobject<UDABTBlackboard>(TEXT("Blackboard"));
}

void UDABTManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Get reference to global event bus
    if (UWorld* World = GetWorld())
    {
        GlobalEventBus = UGlobalEventBus::Get(World);
        RegisterWithEventBus();
    }
    
    // Initialize blackboard with common values
    InitializeBlackboard();
    
    // Auto-start default behavior tree if configured
    if (bAutoStartDefaultTree && DefaultBehaviorTree)
    {
        StartBehaviorTree(DefaultBehaviorTree);
    }
}

void UDABTManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stop any running behavior tree
    StopBehaviorTree();
    
    // Unregister from event bus
    UnregisterFromEventBus();
    
    Super::EndPlay(EndPlayReason);
}

void UDABTManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Only update if we're running
    if (ExecutionState == EBTExecutionState::Running)
    {
        TimeSinceLastUpdate += DeltaTime;
        
        // Update at the specified frequency
        if (TimeSinceLastUpdate >= UpdateFrequency)
        {
            UpdateBehaviorTree(TimeSinceLastUpdate);
            TimeSinceLastUpdate = 0.0f;
        }
    }
}

TArray<EGlobalEventType> UDABTManager::GetListenedEventTypes() const
{
    return {
        EGlobalEventType::NPCRoutineChanged,
        EGlobalEventType::FactionReputationChanged,
        EGlobalEventType::PlayerEnteredRegion,
        EGlobalEventType::CombatStarted,
        EGlobalEventType::CombatEnded
    };
}

bool UDABTManager::StartBehaviorTree(UDABTNode* RootNode)
{
    if (!RootNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTManager: Cannot start behavior tree - RootNode is null"));
        return false;
    }
    
    // Stop any currently running tree
    if (ExecutionState == EBTExecutionState::Running)
    {
        StopBehaviorTree();
    }
    
    CurrentRootNode = RootNode;
    CurrentStatus = EBTNodeStatus::Invalid;
    
    // Initialize the root node
    CurrentRootNode->Initialize(Blackboard);
    SetExecutionState(EBTExecutionState::Running);
    TotalExecutions++;
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("BTManager: Started behavior tree for %s"),
               *GetOwner()->GetName());
    }
    
    return true;
}

void UDABTManager::StopBehaviorTree()
{
    if (ExecutionState == EBTExecutionState::Inactive)
    {
        return;
    }
    
    // Abort the current node if it's running
    if (CurrentRootNode && CurrentStatus == EBTNodeStatus::Running)
    {
        CurrentRootNode->Abort(Blackboard);
    }
    
    SetExecutionState(EBTExecutionState::Inactive);
    CurrentRootNode = nullptr;
    CurrentStatus = EBTNodeStatus::Invalid;
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("BTManager: Stopped behavior tree for %s"), 
               *GetOwner()->GetName());
    }
}

void UDABTManager::PauseBehaviorTree()
{
    if (ExecutionState == EBTExecutionState::Running)
    {
        SetExecutionState(EBTExecutionState::Paused);
        
        if (bDebugMode)
        {
            UE_LOG(LogTemp, Log, TEXT("BTManager: Paused behavior tree for %s"), 
                   *GetOwner()->GetName());
        }
    }
}

void UDABTManager::ResumeBehaviorTree()
{
    if (ExecutionState == EBTExecutionState::Paused)
    {
        SetExecutionState(EBTExecutionState::Running);
        
        if (bDebugMode)
        {
            UE_LOG(LogTemp, Log, TEXT("BTManager: Resumed behavior tree for %s"), 
                   *GetOwner()->GetName());
        }
    }
}

void UDABTManager::RestartBehaviorTree()
{
    if (CurrentRootNode)
    {
        UDABTNode* TreeToRestart = CurrentRootNode;
        StopBehaviorTree();
        StartBehaviorTree(TreeToRestart);
    }
}

void UDABTManager::InitializeBlackboard()
{
    if (!Blackboard)
    {
        return;
    }
    
    // Initialize common blackboard values
    Blackboard->SetObject(TargetActorKey, nullptr);
    Blackboard->SetVector(HomeLocationKey, GetOwner()->GetActorLocation());
    Blackboard->SetInt(CurrentPatrolIndexKey, 0);
    Blackboard->SetFloat(AlertLevelKey, 0.0f);
    Blackboard->SetVector(LastKnownPlayerLocationKey, FVector::ZeroVector);
    
    // Initialize empty patrol points array - store as string for now
    // TODO: Implement proper array support in blackboard
    Blackboard->SetString(PatrolPointsKey, TEXT(""));
}

void UDABTManager::SetTarget(AActor* Target)
{
    if (Blackboard)
    {
        Blackboard->SetObject(TargetActorKey, Target);
        
        // Broadcast event if we have a global event bus
        if (GlobalEventBus)
        {
            FGlobalEvent Event;
            Event.EventType = EGlobalEventType::NPCRoutineChanged;
            Event.Source = GetOwner()->GetName();
            Event.Target = Target ? Target->GetName() : TEXT("");
            Event.EventData.Add(TEXT("Action"), TEXT("TargetChanged"));
            GlobalEventBus->BroadcastEvent(Event);
        }
    }
}

AActor* UDABTManager::GetTarget() const
{
    if (Blackboard)
    {
        return Cast<AActor>(Blackboard->GetObject(TargetActorKey));
    }
    return nullptr;
}

void UDABTManager::SetPatrolPoints(const TArray<FVector>& Points)
{
    if (Blackboard)
    {
        // Convert array to string representation for storage
        FString PointsString;
        for (int32 i = 0; i < Points.Num(); i++)
        {
            if (i > 0) PointsString += TEXT("|");
            PointsString += Points[i].ToString();
        }
        Blackboard->SetString(PatrolPointsKey, PointsString);
        Blackboard->SetInt(CurrentPatrolIndexKey, 0); // Reset to first point
    }
}

TArray<FVector> UDABTManager::GetPatrolPoints() const
{
    if (Blackboard)
    {
        // Parse string representation back to array
        FString PointsString = Blackboard->GetString(PatrolPointsKey);
        TArray<FVector> Points;
        if (!PointsString.IsEmpty())
        {
            TArray<FString> PointStrings;
            PointsString.ParseIntoArray(PointStrings, TEXT("|"), true);
            for (const FString& PointStr : PointStrings)
            {
                FVector Point;
                if (Point.InitFromString(PointStr))
                {
                    Points.Add(Point);
                }
            }
        }
        return Points;
    }
    return TArray<FVector>();
}

void UDABTManager::SetHomeLocation(const FVector& Location)
{
    if (Blackboard)
    {
        Blackboard->SetVector(HomeLocationKey, Location);
    }
}

FVector UDABTManager::GetHomeLocation() const
{
    if (Blackboard)
    {
        return Blackboard->GetVector(HomeLocationKey);
    }
    return FVector::ZeroVector;
}

void UDABTManager::SetDebugMode(bool bEnabled)
{
    bDebugMode = bEnabled;
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("BTManager: Debug mode enabled for %s"), 
               *GetOwner()->GetName());
    }
}

FString UDABTManager::GetDebugInfo() const
{
    FString DebugInfo = FString::Printf(
        TEXT("BTManager Debug Info for %s:\n")
        TEXT("  State: %s\n")
        TEXT("  Root Node: %s\n")
        TEXT("  Current Status: %s\n")
        TEXT("  Total Executions: %d\n")
        TEXT("  Total Execution Time: %.2f\n")
        TEXT("  Last Execution Time: %.2f\n"),
        *GetOwner()->GetName(),
        *UEnum::GetValueAsString(ExecutionState),
        CurrentRootNode ? *CurrentRootNode->GetName() : TEXT("None"),
        *UEnum::GetValueAsString(CurrentStatus),
        TotalExecutions,
        TotalExecutionTime,
        LastExecutionTime
    );
    
    return DebugInfo;
}

void UDABTManager::LogCurrentState() const
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *GetDebugInfo());
}


void UDABTManager::RegisterWithEventBus()
{
    if (GlobalEventBus)
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        TArray<EGlobalEventType> ListenedEvents = {
            EGlobalEventType::NPCDied,
            EGlobalEventType::CombatStarted,
            EGlobalEventType::CombatEnded,
            EGlobalEventType::PlayerEnteredRegion,
            EGlobalEventType::PlayerLeftRegion
        };
        GlobalEventBus->RegisterListener(ListenerInterface, ListenedEvents);
        
        // Bind to the event delegate
        // Note: OnEventBroadcast delegate binding would be handled differently in actual implementation
    }
}

void UDABTManager::UnregisterFromEventBus()
{
    if (GlobalEventBus)
    {
        GlobalEventBus->UnregisterListener(this);
        
        // Unbind from the event delegate
        // Note: OnEventBroadcast delegate unbinding would be handled differently in actual implementation
    }
}

void UDABTManager::SetExecutionState(EBTExecutionState NewState)
{
    if (ExecutionState != NewState)
    {
        EBTExecutionState OldState = ExecutionState;
        ExecutionState = NewState;
        
        // Broadcast state change event
        OnStateChanged.Broadcast(NewState);
        
        if (bDebugMode)
        {
            UE_LOG(LogTemp, Log, TEXT("BTManager: State changed from %s to %s for %s"),
                   *UEnum::GetValueAsString(OldState),
                   *UEnum::GetValueAsString(NewState),
                   *GetOwner()->GetName());
        }
    }
}

void UDABTManager::UpdateBehaviorTree(float DeltaTime)
{
    if (!CurrentRootNode)
    {
        return;
    }
    
    float StartTime = FPlatformTime::Seconds();
    
    // Execute the behavior tree
    CurrentStatus = CurrentRootNode->Execute(Blackboard);
    
    // Handle completion
    if (CurrentStatus == EBTNodeStatus::Success || CurrentStatus == EBTNodeStatus::Failure)
    {
        HandleTreeCompletion(CurrentStatus);
    }
    
    // Update execution time statistics
    LastExecutionTime = FPlatformTime::Seconds() - StartTime;
    TotalExecutionTime += LastExecutionTime;
}

void UDABTManager::HandleTreeCompletion(EBTNodeStatus FinalStatus)
{
    // Broadcast completion event
    OnTreeCompleted.Broadcast(CurrentRootNode, FinalStatus);
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("BTManager: Behavior tree completed with status %s for %s"),
               *UEnum::GetValueAsString(FinalStatus),
               *GetOwner()->GetName());
    }
    
    // Handle looping
    if (bLoopBehaviorTree)
    {
        RestartBehaviorTree();
    }
    else
    {
        SetExecutionState(EBTExecutionState::Completed);
    }
}

// --- UBTUtilities Implementation ---

UDABTNode* UBTUtilities::CreatePatrolBehaviorTree(const TArray<FVector>& PatrolPoints)
{
    // This would create a simple patrol behavior tree
    // For now, return nullptr as we need to implement specific node types first
    UE_LOG(LogTemp, Warning, TEXT("UBTUtilities::CreatePatrolBehaviorTree - Not yet implemented"));
    return nullptr;
}

UDABTNode* UBTUtilities::CreateGuardBehaviorTree(const FVector& GuardLocation, float GuardRadius)
{
    // This would create a simple guard behavior tree
    // For now, return nullptr as we need to implement specific node types first
    UE_LOG(LogTemp, Warning, TEXT("UBTUtilities::CreateGuardBehaviorTree - Not yet implemented"));
    return nullptr;
}

UDABTNode* UBTUtilities::CreateFollowBehaviorTree(AActor* TargetToFollow, float FollowDistance)
{
    // This would create a simple follow behavior tree
    // For now, return nullptr as we need to implement specific node types first
    UE_LOG(LogTemp, Warning, TEXT("UBTUtilities::CreateFollowBehaviorTree - Not yet implemented"));
    return nullptr;
}

UDABTNode* UBTUtilities::CreateBehaviorTreeFromConfig(const FString& ConfigString)
{
    // This would parse a configuration string and create a behavior tree
    // For now, return nullptr as we need to implement specific node types first
    UE_LOG(LogTemp, Warning, TEXT("UBTUtilities::CreateBehaviorTreeFromConfig - Not yet implemented"));
    return nullptr;
}