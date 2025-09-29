#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "AdvancedAIDecisionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EAIDecisionType : uint8
{
    Combat          UMETA(DisplayName = "Combat"),
    Social          UMETA(DisplayName = "Social"),
    Economic        UMETA(DisplayName = "Economic"),
    Survival        UMETA(DisplayName = "Survival"),
    Exploration     UMETA(DisplayName = "Exploration"),
    Political       UMETA(DisplayName = "Political"),
    Crafting        UMETA(DisplayName = "Crafting"),
    Emergency       UMETA(DisplayName = "Emergency")
};

UENUM(BlueprintType)
enum class EAIPersonalityType : uint8
{
    Aggressive      UMETA(DisplayName = "Aggressive"),
    Defensive       UMETA(DisplayName = "Defensive"),
    Cautious        UMETA(DisplayName = "Cautious"),
    Opportunistic   UMETA(DisplayName = "Opportunistic"),
    Cooperative     UMETA(DisplayName = "Cooperative"),
    Independent     UMETA(DisplayName = "Independent"),
    Adaptive        UMETA(DisplayName = "Adaptive"),
    Loyal           UMETA(DisplayName = "Loyal"),
    MAX             UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EDecisionOutcome : uint8
{
    Success         UMETA(DisplayName = "Success"),
    Failure         UMETA(DisplayName = "Failure"),
    Partial         UMETA(DisplayName = "Partial Success"),
    Catastrophic    UMETA(DisplayName = "Catastrophic Failure"),
    Unexpected      UMETA(DisplayName = "Unexpected Result")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAIDecisionNode
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    FString NodeID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    FString NodeName;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    EAIDecisionType DecisionType = EAIDecisionType::Combat;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    TMap<FString, float> Conditions;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    TArray<FString> Actions;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    TMap<FString, float> Weights;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    float SuccessRate = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    float LearningRate = 0.1f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    int32 TimesUsed = 0;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    int32 TimesSuccessful = 0;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    TArray<FString> ChildNodes;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision")
    TArray<FString> ParentNodes;

    FAIDecisionNode()
    {
        NodeID = TEXT("");
        NodeName = TEXT("");
        DecisionType = EAIDecisionType::Combat;
        SuccessRate = 0.5f;
        LearningRate = 0.1f;
        TimesUsed = 0;
        TimesSuccessful = 0;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAIDecisionTree
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    FString TreeID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    FString TreeName;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    EAIPersonalityType PersonalityType = EAIPersonalityType::Adaptive;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    TMap<FString, FAIDecisionNode> Nodes;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    FString RootNodeID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    float AdaptationRate = 0.05f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    float ExplorationRate = 0.2f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    TMap<FString, float> MemoryWeights;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    TArray<FString> RecentDecisions;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Tree")
    int32 MaxMemorySize = 50;

    FAIDecisionTree()
    {
        TreeID = TEXT("");
        TreeName = TEXT("");
        PersonalityType = EAIPersonalityType::Adaptive;
        RootNodeID = TEXT("");
        AdaptationRate = 0.05f;
        ExplorationRate = 0.2f;
        MaxMemorySize = 50;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAIDecisionContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    FString NPCID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    FVector NPCLocation;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TMap<FString, float> EnvironmentalFactors;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TMap<FString, float> SocialFactors;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TMap<FString, float> EconomicFactors;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TArray<FString> NearbyNPCs;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TArray<FString> AvailableResources;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    float ThreatLevel = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    float OpportunityLevel = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    float TimeOfDay = 12.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    FString CurrentWeather;

    UPROPERTY(BlueprintReadOnly, Category = "AI Context")
    TArray<FString> ActiveWorldEvents;

    FAIDecisionContext()
    {
        NPCID = TEXT("");
        NPCLocation = FVector::ZeroVector;
        ThreatLevel = 0.0f;
        OpportunityLevel = 0.0f;
        TimeOfDay = 12.0f;
        CurrentWeather = TEXT("Clear");
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAIDecisionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    FString DecisionID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    FString ChosenAction;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    float Confidence = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    TMap<FString, float> ReasoningFactors;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    EDecisionOutcome Outcome = EDecisionOutcome::Success;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    float ExecutionTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Decision Result")
    TArray<FString> ConsequentActions;

    FAIDecisionResult()
    {
        DecisionID = TEXT("");
        ChosenAction = TEXT("");
        Confidence = 0.5f;
        Outcome = EDecisionOutcome::Success;
        ExecutionTime = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FAILearningData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    FString LearningID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    FString NPCID;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    FAIDecisionContext Context;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    FAIDecisionResult Result;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    float Timestamp = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    float RewardValue = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Learning")
    TMap<FString, float> FeatureWeights;

    FAILearningData()
    {
        LearningID = TEXT("");
        NPCID = TEXT("");
        Timestamp = 0.0f;
        RewardValue = 0.0f;
    }
};

// AI Decision Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAIDecisionMade, const FString&, NPCID, const FAIDecisionResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAILearningUpdate, const FString&, NPCID, const FAILearningData&, LearningData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAIBehaviorAdapted, const FString&, NPCID, const FString&, BehaviorType, float, AdaptationStrength);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedAIDecisionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedAIDecisionSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedAIDecisionSubsystem, STATGROUP_Tickables); }

    // AI Decision Delegates
    UPROPERTY(BlueprintAssignable, Category = "AI Decision Events")
    FOnAIDecisionMade OnAIDecisionMade;

    UPROPERTY(BlueprintAssignable, Category = "AI Decision Events")
    FOnAILearningUpdate OnAILearningUpdate;

    UPROPERTY(BlueprintAssignable, Category = "AI Decision Events")
    FOnAIBehaviorAdapted OnAIBehaviorAdapted;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    FAIDecisionResult MakeDecision(const FString& NPCID, const FAIDecisionContext& Context);

    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    void RegisterDecisionOutcome(const FString& NPCID, const FString& DecisionID, EDecisionOutcome Outcome, float RewardValue);

    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    void CreateDecisionTree(const FString& NPCID, EAIPersonalityType PersonalityType);

    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    void UpdateNPCPersonality(const FString& NPCID, EAIPersonalityType NewPersonality);

    UFUNCTION(BlueprintPure, Category = "AI Decision")
    FAIDecisionTree GetNPCDecisionTree(const FString& NPCID) const;

    UFUNCTION(BlueprintPure, Category = "AI Decision")
    TArray<FAILearningData> GetNPCLearningHistory(const FString& NPCID) const;

    UFUNCTION(BlueprintPure, Category = "AI Decision")
    float GetNPCAdaptationLevel(const FString& NPCID) const;

    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    void ForceAILearningUpdate(const FString& NPCID);

    UFUNCTION(BlueprintCallable, Category = "AI Decision")
    void ResetNPCAI(const FString& NPCID);

protected:
    // AI data storage
    UPROPERTY()
    TMap<FString, FAIDecisionTree> NPCDecisionTrees;

    // Note: TMap with TArray values cannot be exposed to Blueprint
    TMap<FString, TArray<FAILearningData>> NPCLearningHistory;

    UPROPERTY()
    TMap<EAIPersonalityType, FAIDecisionTree> PersonalityTemplates;

    UPROPERTY()
    TArray<FAILearningData> GlobalLearningData;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Configuration")
    float LearningUpdateInterval = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Configuration")
    float GlobalAdaptationRate = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Configuration")
    int32 MaxLearningHistorySize = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Configuration")
    bool bEnableGlobalLearning = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Configuration")
    bool bEnableCrossNPCLearning = true;

    // Internal state
    float LearningUpdateTimer = 0.0f;
    int32 NextDecisionID = 1;

    // Initialization functions
    void InitializePersonalityTemplates();
    void InitializeDecisionNodes();

    // Decision making functions
    FAIDecisionResult ProcessDecisionTree(const FString& NPCID, const FAIDecisionContext& Context);
    FString SelectBestAction(const FAIDecisionNode& Node, const FAIDecisionContext& Context);
    float CalculateActionScore(const FString& Action, const FAIDecisionNode& Node, const FAIDecisionContext& Context);
    float EvaluateConditions(const TMap<FString, float>& Conditions, const FAIDecisionContext& Context);

    // Learning functions
    void UpdateLearningData(float DeltaTime);
    void ProcessLearningFeedback(const FString& NPCID, const FAILearningData& LearningData);
    void AdaptDecisionWeights(FAIDecisionTree& Tree, const FAILearningData& LearningData);
    void UpdateSuccessRates(FAIDecisionTree& Tree, const FString& NodeID, EDecisionOutcome Outcome);

    // Cross-NPC learning functions
    void ProcessGlobalLearning();
    void ShareLearningBetweenNPCs();
    TArray<FAILearningData> FindSimilarExperiences(const FAIDecisionContext& Context, const FString& ExcludeNPCID);

    // Personality system functions
    void ApplyPersonalityModifiers(FAIDecisionTree& Tree, EAIPersonalityType Personality);
    float GetPersonalityWeight(EAIPersonalityType Personality, EAIDecisionType DecisionType);
    void EvolvePersonality(const FString& NPCID, const TArray<FAILearningData>& RecentExperiences);

    // Context analysis functions
    FAIDecisionContext AnalyzeEnvironment(const FString& NPCID, const FVector& Location);
    float CalculateThreatLevel(const FAIDecisionContext& Context);
    float CalculateOpportunityLevel(const FAIDecisionContext& Context);
    TMap<FString, float> ExtractContextFeatures(const FAIDecisionContext& Context);

    // Utility functions
    FString GenerateUniqueDecisionID();
    float CalculateRewardValue(EDecisionOutcome Outcome, const FAIDecisionContext& Context);
    void PruneOldLearningData();
    void OptimizeDecisionTrees();

    // Advanced AI features
    void ProcessEmergentBehaviors();
    void DetectBehaviorPatterns(const FString& NPCID);
    void AdaptToWorldEvents(const TArray<FString>& ActiveEvents);
    void ImplementSocialLearning();
};