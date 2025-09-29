#include "AdvancedAIDecisionSubsystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Misc/DateTime.h"


void UAdvancedAIDecisionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize the AI system
    InitializePersonalityTemplates();
    InitializeDecisionNodes();
}

void UAdvancedAIDecisionSubsystem::Tick(float DeltaTime)
{
    // Update learning system
    LearningUpdateTimer += DeltaTime;
    if (LearningUpdateTimer >= LearningUpdateInterval)
    {
        UpdateLearningData(DeltaTime);
        LearningUpdateTimer = 0.0f;
    }
    
    // Process global learning if enabled
    if (bEnableGlobalLearning)
    {
        ProcessGlobalLearning();
    }
    
    // Process emergent behaviors
    ProcessEmergentBehaviors();
}

FAIDecisionResult UAdvancedAIDecisionSubsystem::MakeDecision(const FString& NPCID, const FAIDecisionContext& Context)
{
    FAIDecisionResult Result;
    Result.DecisionID = GenerateUniqueDecisionID();
    Result.ExecutionTime = 0.0f;
    
    // Ensure the NPC has a decision tree
    if (!NPCDecisionTrees.Contains(NPCID))
    {
        CreateDecisionTree(NPCID, EAIPersonalityType::Adaptive);
    }
    
    // Process the decision tree
    Result = ProcessDecisionTree(NPCID, Context);
    
    // Broadcast the decision event
    OnAIDecisionMade.Broadcast(NPCID, Result);
    
    return Result;
}

void UAdvancedAIDecisionSubsystem::RegisterDecisionOutcome(const FString& NPCID, const FString& DecisionID, EDecisionOutcome Outcome, float RewardValue)
{
    // Create learning data from the decision
    FAILearningData LearningData;
    LearningData.LearningID = DecisionID;
    LearningData.NPCID = NPCID;
       if (UWorld* World = GetWorld())
       {
           LearningData.Timestamp = World->GetTimeSeconds();
       }
       else
       {
           LearningData.Timestamp = FDateTime::UtcNow().ToUnixTimestamp();
       }
    LearningData.RewardValue = CalculateRewardValue(Outcome, LearningData.Context);
    LearningData.Result.Outcome = Outcome;
    
    // Store the learning data
    if (!NPCLearningHistory.Contains(NPCID))
    {
        NPCLearningHistory.Add(NPCID, TArray<FAILearningData>());
    }
    
    NPCLearningHistory[NPCID].Add(LearningData);
    
    // Prune old learning data if necessary
    if (NPCLearningHistory[NPCID].Num() > MaxLearningHistorySize)
    {
        NPCLearningHistory[NPCID].RemoveAt(0);
    }
    
    // Process the learning feedback
    ProcessLearningFeedback(NPCID, LearningData);
    
    // Evolve the NPC's personality based on recent experiences
    if (NPCLearningHistory.Contains(NPCID) && NPCLearningHistory[NPCID].Num() >= 5)
    {
        EvolvePersonality(NPCID, NPCLearningHistory[NPCID]);
    }
}

void UAdvancedAIDecisionSubsystem::CreateDecisionTree(const FString& NPCID, EAIPersonalityType PersonalityType)
{
    if (NPCDecisionTrees.Contains(NPCID))
    {
        return; // Tree already exists
    }
    
    // Create a new decision tree
    FAIDecisionTree NewTree;
    NewTree.TreeID = NPCID;
    NewTree.TreeName = FString::Printf(TEXT("DecisionTree_%s"), *NPCID);
    NewTree.PersonalityType = PersonalityType;
    NewTree.AdaptationRate = 0.05f;
    NewTree.ExplorationRate = 0.2f;
    NewTree.MaxMemorySize = 50;
    
    // Initialize with personality template
    if (PersonalityTemplates.Contains(PersonalityType))
    {
        NewTree = PersonalityTemplates[PersonalityType];
        NewTree.TreeID = NPCID;
        NewTree.TreeName = FString::Printf(TEXT("DecisionTree_%s"), *NPCID);
    }
    
    // Set root node (this would be determined by the personality type)
    NewTree.RootNodeID = TEXT("Root");
    
    // Add to the collection
    NPCDecisionTrees.Add(NPCID, NewTree);
}

void UAdvancedAIDecisionSubsystem::UpdateNPCPersonality(const FString& NPCID, EAIPersonalityType NewPersonality)
{
    if (!NPCDecisionTrees.Contains(NPCID))
    {
        return;
    }
    
    // Update the personality type
    NPCDecisionTrees[NPCID].PersonalityType = NewPersonality;
    
    // Apply personality modifiers
    ApplyPersonalityModifiers(NPCDecisionTrees[NPCID], NewPersonality);
    
    // Broadcast behavior adaptation event
    OnAIBehaviorAdapted.Broadcast(NPCID, TEXT("Personality"), 1.0f);
}

FAIDecisionTree UAdvancedAIDecisionSubsystem::GetNPCDecisionTree(const FString& NPCID) const
{
    if (NPCDecisionTrees.Contains(NPCID))
    {
        return NPCDecisionTrees[NPCID];
    }
    
    // Return empty tree if not found
    FAIDecisionTree EmptyTree;
    EmptyTree.TreeID = TEXT("Invalid");
    return EmptyTree;
}

TArray<FAILearningData> UAdvancedAIDecisionSubsystem::GetNPCLearningHistory(const FString& NPCID) const
{
    if (NPCLearningHistory.Contains(NPCID))
    {
        return NPCLearningHistory[NPCID];
    }
    
    // Return empty array if not found
    return TArray<FAILearningData>();
}

float UAdvancedAIDecisionSubsystem::GetNPCAdaptationLevel(const FString& NPCID) const
{
    if (!NPCDecisionTrees.Contains(NPCID))
    {
        return 0.0f;
    }
    
    // Calculate adaptation level based on learning and changes
    const FAIDecisionTree& Tree = NPCDecisionTrees[NPCID];
    float AdaptationLevel = 0.0f;
    
    // This is a simplified calculation - in a real system, this would be more complex
    AdaptationLevel = Tree.AdaptationRate * 100.0f;
    
    return AdaptationLevel;
}

void UAdvancedAIDecisionSubsystem::ForceAILearningUpdate(const FString& NPCID)
{
    if (!NPCDecisionTrees.Contains(NPCID) || !NPCLearningHistory.Contains(NPCID))
    {
        return;
    }
    
    // Process all learning data for this NPC
    for (const FAILearningData& LearningData : NPCLearningHistory[NPCID])
    {
        ProcessLearningFeedback(NPCID, LearningData);
    }
    
    // Optimize the decision tree
    OptimizeDecisionTrees();
}

void UAdvancedAIDecisionSubsystem::ResetNPCAI(const FString& NPCID)
{
    // Remove the decision tree
    if (NPCDecisionTrees.Contains(NPCID))
    {
        NPCDecisionTrees.Remove(NPCID);
    }
    
    // Clear learning history
    if (NPCLearningHistory.Contains(NPCID))
    {
        NPCLearningHistory.Remove(NPCID);
    }
    
    // Create a new default tree
    CreateDecisionTree(NPCID, EAIPersonalityType::Adaptive);
}

void UAdvancedAIDecisionSubsystem::InitializePersonalityTemplates()
{
    // Initialize templates for each personality type
    for (int32 i = 0; i < static_cast<int32>(EAIPersonalityType::MAX); ++i)
    {
        EAIPersonalityType Personality = static_cast<EAIPersonalityType>(i);
        FAIDecisionTree TemplateTree;
        TemplateTree.TreeID = FString::Printf(TEXT("Template_%d"), i);
        TemplateTree.TreeName = TEXT("Personality Template");
        TemplateTree.PersonalityType = Personality;
        TemplateTree.AdaptationRate = 0.05f;
        TemplateTree.ExplorationRate = 0.2f;
        
        // Set default weights based on personality
        ApplyPersonalityModifiers(TemplateTree, Personality);
        
        PersonalityTemplates.Add(Personality, TemplateTree);
    }
}

void UAdvancedAIDecisionSubsystem::InitializeDecisionNodes()
{
    // This would initialize common decision nodes
    // In a real implementation, this would load from data tables or configuration
}

FAIDecisionResult UAdvancedAIDecisionSubsystem::ProcessDecisionTree(const FString& NPCID, const FAIDecisionContext& Context)
{
    FAIDecisionResult Result;
    Result.DecisionID = GenerateUniqueDecisionID();
    Result.Confidence = 0.5f;
    
    if (!NPCDecisionTrees.Contains(NPCID))
    {
        return Result;
    }
    
    const FAIDecisionTree& Tree = NPCDecisionTrees[NPCID];
    
    // For simplicity, we'll just select a random action
    // In a real implementation, this would traverse the decision tree
    Result.ChosenAction = TEXT("DefaultAction");
    Result.Confidence = 0.7f;
    
    return Result;
}

FString UAdvancedAIDecisionSubsystem::SelectBestAction(const FAIDecisionNode& Node, const FAIDecisionContext& Context)
{
    // In a real implementation, this would select the best action based on conditions and weights
    // For now, return a default action
    return TEXT("DefaultAction");
}

float UAdvancedAIDecisionSubsystem::CalculateActionScore(const FString& Action, const FAIDecisionNode& Node, const FAIDecisionContext& Context)
{
    // In a real implementation, this would calculate a score based on various factors
    // For now, return a random score
    return FMath::FRandRange(0.0f, 1.0f);
}

float UAdvancedAIDecisionSubsystem::EvaluateConditions(const TMap<FString, float>& Conditions, const FAIDecisionContext& Context)
{
    // In a real implementation, this would evaluate the conditions against the context
    // For now, return a default value
    return 0.5f;
}

void UAdvancedAIDecisionSubsystem::UpdateLearningData(float DeltaTime)
{
    // Update learning system
    // In a real implementation, this would process learning data and update decision trees
}

void UAdvancedAIDecisionSubsystem::ProcessLearningFeedback(const FString& NPCID, const FAILearningData& LearningData)
{
    if (!NPCDecisionTrees.Contains(NPCID))
    {
        return;
    }
    
    // Update the decision tree based on learning data
    AdaptDecisionWeights(NPCDecisionTrees[NPCID], LearningData);
    UpdateSuccessRates(NPCDecisionTrees[NPCID], LearningData.Result.DecisionID, LearningData.Result.Outcome);
    
    // Add to global learning data if enabled
    if (bEnableGlobalLearning)
    {
        GlobalLearningData.Add(LearningData);
    }
    
    // Broadcast learning update event
    OnAILearningUpdate.Broadcast(NPCID, LearningData);
}

void UAdvancedAIDecisionSubsystem::AdaptDecisionWeights(FAIDecisionTree& Tree, const FAILearningData& LearningData)
{
    // Adapt the decision tree weights based on learning data
    // In a real implementation, this would use machine learning algorithms
    Tree.AdaptationRate += GlobalAdaptationRate * 0.1f;
}

void UAdvancedAIDecisionSubsystem::UpdateSuccessRates(FAIDecisionTree& Tree, const FString& NodeID, EDecisionOutcome Outcome)
{
    // Update success rates based on outcome
    // In a real implementation, this would update the success rate of the specific node
}

void UAdvancedAIDecisionSubsystem::ProcessGlobalLearning()
{
    // Process global learning across all NPCs
    // In a real implementation, this would share learning between NPCs
    if (bEnableCrossNPCLearning)
    {
        ShareLearningBetweenNPCs();
    }
}

void UAdvancedAIDecisionSubsystem::ShareLearningBetweenNPCs()
{
    // Share learning data between NPCs
    // In a real implementation, this would implement social learning
}

TArray<FAILearningData> UAdvancedAIDecisionSubsystem::FindSimilarExperiences(const FAIDecisionContext& Context, const FString& ExcludeNPCID)
{
    // Find similar experiences from other NPCs
    // In a real implementation, this would search the learning history
    return TArray<FAILearningData>();
}

void UAdvancedAIDecisionSubsystem::ApplyPersonalityModifiers(FAIDecisionTree& Tree, EAIPersonalityType Personality)
{
    // Apply personality-specific modifiers to the decision tree
    // In a real implementation, this would adjust weights and parameters
    Tree.ExplorationRate = GetPersonalityWeight(Personality, EAIDecisionType::Exploration);
}

float UAdvancedAIDecisionSubsystem::GetPersonalityWeight(EAIPersonalityType Personality, EAIDecisionType DecisionType)
{
    // Return a weight based on personality and decision type
    // In a real implementation, this would use a more complex calculation
    return 0.5f;
}

void UAdvancedAIDecisionSubsystem::EvolvePersonality(const FString& NPCID, const TArray<FAILearningData>& RecentExperiences)
{
    // Evolve the NPC's personality based on recent experiences
    // In a real implementation, this would analyze the experiences and adjust personality
}

FAIDecisionContext UAdvancedAIDecisionSubsystem::AnalyzeEnvironment(const FString& NPCID, const FVector& Location)
{
    // Analyze the environment for the NPC
    // In a real implementation, this would gather data from the world
    FAIDecisionContext Context;
    Context.NPCID = NPCID;
    Context.NPCLocation = Location;
    return Context;
}

float UAdvancedAIDecisionSubsystem::CalculateThreatLevel(const FAIDecisionContext& Context)
{
    // Calculate threat level based on context
    // In a real implementation, this would analyze enemies, resources, etc.
    return 0.5f;
}

float UAdvancedAIDecisionSubsystem::CalculateOpportunityLevel(const FAIDecisionContext& Context)
{
    // Calculate opportunity level based on context
    // In a real implementation, this would analyze resources, events, etc.
    return 0.5f;
}

TMap<FString, float> UAdvancedAIDecisionSubsystem::ExtractContextFeatures(const FAIDecisionContext& Context)
{
    // Extract features from the context for machine learning
    // In a real implementation, this would extract relevant features
    TMap<FString, float> Features;
    return Features;
}

FString UAdvancedAIDecisionSubsystem::GenerateUniqueDecisionID()
{
    return FString::Printf(TEXT("Decision_%d"), NextDecisionID++);
}

float UAdvancedAIDecisionSubsystem::CalculateRewardValue(EDecisionOutcome Outcome, const FAIDecisionContext& Context)
{
    // Calculate reward value based on outcome and context
    // In a real implementation, this would use a more complex calculation
    float BaseReward = 0.0f;
    switch (Outcome)
    {
    case EDecisionOutcome::Success:
        BaseReward = 1.0f;
        break;
    case EDecisionOutcome::Partial:
        BaseReward = 0.5f;
        break;
    case EDecisionOutcome::Failure:
        BaseReward = -0.5f;
        break;
    case EDecisionOutcome::Catastrophic:
        BaseReward = -1.0f;
        break;
    case EDecisionOutcome::Unexpected:
        BaseReward = 0.2f;
        break;
    }
    return BaseReward;
}

void UAdvancedAIDecisionSubsystem::PruneOldLearningData()
{
    // Remove old learning data to prevent memory issues
    // In a real implementation, this would remove data older than a certain threshold
}

void UAdvancedAIDecisionSubsystem::OptimizeDecisionTrees()
{
    // Optimize decision trees for better performance
    // In a real implementation, this would simplify trees and remove redundant nodes
}

void UAdvancedAIDecisionSubsystem::ProcessEmergentBehaviors()
{
    // Process emergent behaviors from complex interactions
    // In a real implementation, this would detect patterns and create new behaviors
}

void UAdvancedAIDecisionSubsystem::DetectBehaviorPatterns(const FString& NPCID)
{
    // Detect behavior patterns in NPC actions
    // In a real implementation, this would analyze action sequences
}

void UAdvancedAIDecisionSubsystem::AdaptToWorldEvents(const TArray<FString>& ActiveEvents)
{
    // Adapt AI behavior to world events
    // In a real implementation, this would modify decision trees based on events
}

void UAdvancedAIDecisionSubsystem::ImplementSocialLearning()
{
    // Implement social learning between NPCs
    // In a real implementation, this would allow NPCs to learn from each other
}