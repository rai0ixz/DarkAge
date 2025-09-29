#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "PoliticalIntrigueSubsystem.generated.h"

UENUM(BlueprintType)
enum class EIntrigueType : uint8
{
    Espionage       UMETA(DisplayName = "Espionage"),
    Assassination   UMETA(DisplayName = "Assassination"),
    Sabotage        UMETA(DisplayName = "Sabotage"),
    Blackmail       UMETA(DisplayName = "Blackmail"),
    Bribery         UMETA(DisplayName = "Bribery"),
    Propaganda      UMETA(DisplayName = "Propaganda"),
    Infiltration    UMETA(DisplayName = "Infiltration"),
    Conspiracy      UMETA(DisplayName = "Conspiracy")
};

UENUM(BlueprintType)
enum class ESpyRank : uint8
{
    Informant       UMETA(DisplayName = "Informant"),
    Agent           UMETA(DisplayName = "Agent"),
    Operative       UMETA(DisplayName = "Operative"),
    Spymaster       UMETA(DisplayName = "Spymaster"),
    ShadowLord      UMETA(DisplayName = "Shadow Lord")
};

UENUM(BlueprintType)
enum class EIntrigueOutcome : uint8
{
    CriticalSuccess UMETA(DisplayName = "Critical Success"),
    Success         UMETA(DisplayName = "Success"),
    PartialSuccess  UMETA(DisplayName = "Partial Success"),
    Failure         UMETA(DisplayName = "Failure"),
    CriticalFailure UMETA(DisplayName = "Critical Failure"),
    Exposed         UMETA(DisplayName = "Exposed"),
    Compromised     UMETA(DisplayName = "Compromised")
};

UENUM(BlueprintType)
enum class ESecretType : uint8
{
    Personal        UMETA(DisplayName = "Personal Secret"),
    Political       UMETA(DisplayName = "Political Secret"),
    Military        UMETA(DisplayName = "Military Secret"),
    Economic        UMETA(DisplayName = "Economic Secret"),
    Magical         UMETA(DisplayName = "Magical Secret"),
    Criminal        UMETA(DisplayName = "Criminal Secret"),
    Religious       UMETA(DisplayName = "Religious Secret"),
    Technological   UMETA(DisplayName = "Technological Secret")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSpyAgent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FString AgentID;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FString AgentName;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FString CoverIdentity;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    ESpyRank Rank = ESpyRank::Informant;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FString EmployerFaction;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FString TargetFaction;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Stealth = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Deception = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Investigation = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Combat = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Loyalty = 75.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    float Suspicion = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    bool bIsActive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    bool bIsCompromised = false;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    TArray<FString> KnownSecrets;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    TArray<FString> ActiveMissions;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Agent")
    FVector LastKnownLocation;

    FSpyAgent()
    {
        AgentID = TEXT("");
        AgentName = TEXT("");
        CoverIdentity = TEXT("");
        Rank = ESpyRank::Informant;
        EmployerFaction = TEXT("");
        TargetFaction = TEXT("");
        Stealth = 50.0f;
        Deception = 50.0f;
        Investigation = 50.0f;
        Combat = 50.0f;
        Loyalty = 75.0f;
        Suspicion = 0.0f;
        bIsActive = true;
        bIsCompromised = false;
        LastKnownLocation = FVector::ZeroVector;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSecret
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    FString SecretID;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    FString SecretName;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    ESecretType SecretType = ESecretType::Personal;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    FString OwnerID;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    float Value = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    float Sensitivity = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    TArray<FString> KnownBy;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    float DiscoveryDifficulty = 0.7f;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    bool bIsActive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    float ExpirationTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Secret")
    TMap<FString, float> PotentialConsequences;

    FSecret()
    {
        SecretID = TEXT("");
        SecretName = TEXT("");
        Description = TEXT("");
        SecretType = ESecretType::Personal;
        OwnerID = TEXT("");
        Value = 100.0f;
        Sensitivity = 0.5f;
        DiscoveryDifficulty = 0.7f;
        bIsActive = true;
        ExpirationTime = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FIntrigueMission
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString MissionID;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString MissionName;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    EIntrigueType MissionType = EIntrigueType::Espionage;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString EmployerFaction;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString TargetFaction;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString TargetIndividual;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    FString AssignedAgent;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    float Difficulty = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    float TimeLimit = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    float Progress = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    bool bIsCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    TMap<FString, float> RequiredSkills;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    TArray<FString> RequiredResources;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    float RewardValue = 1000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Intrigue Mission")
    TMap<FString, float> RiskFactors;

    FIntrigueMission()
    {
        MissionID = TEXT("");
        MissionName = TEXT("");
        MissionType = EIntrigueType::Espionage;
        EmployerFaction = TEXT("");
        TargetFaction = TEXT("");
        TargetIndividual = TEXT("");
        AssignedAgent = TEXT("");
        Difficulty = 0.5f;
        TimeLimit = 0.0f;
        StartTime = 0.0f;
        Progress = 0.0f;
        bIsActive = false;
        bIsCompleted = false;
        RewardValue = 1000.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FConspiracy
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    FString ConspiracyID;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    FString ConspiracyName;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    TArray<FString> Participants;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    FString LeaderID;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    TArray<FString> TargetFactions;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    TArray<FString> PlannedActions;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    float Secrecy = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    float Progress = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    bool bIsActive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    bool bIsExposed = false;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Conspiracy")
    float PlannedExecutionTime = 0.0f;

    FConspiracy()
    {
        ConspiracyID = TEXT("");
        ConspiracyName = TEXT("");
        Description = TEXT("");
        LeaderID = TEXT("");
        Secrecy = 1.0f;
        Progress = 0.0f;
        bIsActive = true;
        bIsExposed = false;
        StartTime = 0.0f;
        PlannedExecutionTime = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSpyNetwork
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    FString NetworkID;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    FString NetworkName;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    FString OwnerFaction;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    TArray<FString> Agents;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    TArray<FString> SafeHouses;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    float Funding = 10000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    float Efficiency = 0.7f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    float Security = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    TMap<FString, int32> IntelligenceAssets;

    UPROPERTY(BlueprintReadOnly, Category = "Spy Network")
    TArray<FString> ActiveOperations;

    FSpyNetwork()
    {
        NetworkID = TEXT("");
        NetworkName = TEXT("");
        OwnerFaction = TEXT("");
        Funding = 10000.0f;
        Efficiency = 0.7f;
        Security = 0.8f;
    }
};

// Political Intrigue Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIntrigueMissionCompleted, const FString&, MissionID, EIntrigueOutcome, Outcome, const FString&, AgentID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAgentCompromised, const FString&, AgentID, const FString&, CompromisedBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSecretDiscovered, const FString&, SecretID, const FString&, DiscoveredBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConspiracyExposed, const FString&, ConspiracyID, float, ExposureLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAssassinationAttempt, const FString&, TargetID, const FString&, AssassinID, bool, bSuccessful);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UPoliticalIntrigueSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UPoliticalIntrigueSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UPoliticalIntrigueSubsystem, STATGROUP_Tickables); }

    // Political Intrigue Delegates
    UPROPERTY(BlueprintAssignable, Category = "Political Intrigue Events")
    FOnIntrigueMissionCompleted OnIntrigueMissionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Political Intrigue Events")
    FOnAgentCompromised OnAgentCompromised;

    UPROPERTY(BlueprintAssignable, Category = "Political Intrigue Events")
    FOnSecretDiscovered OnSecretDiscovered;

    UPROPERTY(BlueprintAssignable, Category = "Political Intrigue Events")
    FOnConspiracyExposed OnConspiracyExposed;

    UPROPERTY(BlueprintAssignable, Category = "Political Intrigue Events")
    FOnAssassinationAttempt OnAssassinationAttempt;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    FString RecruitAgent(const FString& FactionID, const FString& TargetNPCID, float RecruitmentCost);

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    FString StartIntrigueMission(const FString& AgentID, EIntrigueType MissionType, const FString& TargetID);

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    bool AttemptAssassination(const FString& AgentID, const FString& TargetID, const FString& Method);

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    FString CreateConspiracy(const FString& LeaderID, const TArray<FString>& Participants, const FString& Goal);

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    bool BlackmailTarget(const FString& AgentID, const FString& TargetID, const FString& SecretID);

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    void EstablishSpyNetwork(const FString& FactionID, const FString& TargetRegion, float InitialFunding);

    UFUNCTION(BlueprintPure, Category = "Political Intrigue")
    TArray<FSpyAgent> GetFactionAgents(const FString& FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Political Intrigue")
    TArray<FSecret> GetKnownSecrets(const FString& AgentID) const;

    UFUNCTION(BlueprintPure, Category = "Political Intrigue")
    TArray<FIntrigueMission> GetActiveMissions(const FString& FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Political Intrigue")
    float GetFactionIntrigueRating(const FString& FactionID) const;

    UFUNCTION(BlueprintCallable, Category = "Political Intrigue")
    void CounterIntelligenceOperation(const FString& FactionID, const FString& TargetNetworkID);

protected:
    // Intrigue system data
    UPROPERTY()
    TMap<FString, FSpyAgent> SpyAgents;

    UPROPERTY()
    TMap<FString, FSecret> Secrets;

    UPROPERTY()
    TArray<FIntrigueMission> ActiveMissions;

    UPROPERTY()
    TMap<FString, FConspiracy> ActiveConspiracies;

    UPROPERTY()
    TMap<FString, FSpyNetwork> SpyNetworks;

    // Faction intrigue capabilities
    UPROPERTY()
    TMap<FString, float> FactionIntrigueRatings;

    // Note: TMap with TArray values cannot be exposed to Blueprint
    TMap<FString, TArray<FString>> FactionSecrets;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intrigue Configuration")
    float MissionUpdateInterval = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intrigue Configuration")
    float CounterIntelligenceCheckInterval = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intrigue Configuration")
    float BaseAssassinationSuccessRate = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intrigue Configuration")
    bool bEnablePlayerTargeting = true;

    // Internal state
    float MissionUpdateTimer = 0.0f;
    float CounterIntelligenceTimer = 0.0f;
    int32 NextAgentID = 1;
    int32 NextMissionID = 1;
    int32 NextSecretID = 1;

    // Initialization functions
    void InitializeSpyNetworks();
    void InitializeSecrets();
    void InitializeFactionCapabilities();

    // Mission management functions
    void UpdateActiveMissions(float DeltaTime);
    void ProcessMissionOutcomes();
    EIntrigueOutcome CalculateMissionOutcome(const FIntrigueMission& Mission, const FSpyAgent& Agent);
    void ApplyMissionConsequences(const FIntrigueMission& Mission, EIntrigueOutcome Outcome);

    // Agent management functions
    void UpdateAgentStatus(float DeltaTime);
    void ProcessAgentLoyalty();
    void HandleCompromisedAgents();
    float CalculateAgentSuspicion(const FSpyAgent& Agent);

    // Secret management functions
    void GenerateRandomSecrets();
    void ProcessSecretDiscovery();
    void UpdateSecretValues();
    bool AttemptSecretDiscovery(const FSpyAgent& Agent, const FString& TargetID);

    // Conspiracy management functions
    void UpdateActiveConspiracies(float DeltaTime);
    void ProcessConspiracyProgress();
    void CheckConspiracyExposure();
    void ExecuteConspiracy(FConspiracy& Conspiracy);

    // Counter-intelligence functions
    void ProcessCounterIntelligence();
    void DetectEnemyAgents(const FString& FactionID);
    void NeutralizeThreats(const FString& FactionID);
    float CalculateDetectionRisk(const FSpyAgent& Agent, const FString& TargetFaction);

    // Assassination system functions
    bool ProcessAssassinationAttempt(const FSpyAgent& Agent, const FString& TargetID, const FString& Method);
    float CalculateAssassinationChance(const FSpyAgent& Agent, const FString& TargetID, const FString& Method);
    void ApplyAssassinationConsequences(const FString& TargetID, bool bSuccessful, const FString& AgentID);

    // Blackmail and coercion functions
    bool ProcessBlackmailAttempt(const FSpyAgent& Agent, const FString& TargetID, const FSecret& Secret);
    void ApplyBlackmailEffects(const FString& TargetID, const FSecret& Secret);
    float CalculateBlackmailLeverage(const FSecret& Secret, const FString& TargetID);

    // Network management functions
    void UpdateSpyNetworks(float DeltaTime);
    void ProcessNetworkOperations();
    void HandleNetworkCompromise();
    float CalculateNetworkEfficiency(const FSpyNetwork& Network);

    // Utility functions
    FString GenerateUniqueAgentID();
    FString GenerateUniqueMissionID();
    FString GenerateUniqueSecretID();
    FString GenerateUniqueConspiracyID();
    float CalculateIntrigueRating(const FString& FactionID);
    void UpdateFactionRelations(const FString& FactionA, const FString& FactionB, float RelationChange);

    // AI integration functions
    void ProcessAIIntrigueDecisions();
    void GenerateAIMissions();
    void HandleAIRecruitment();
    void ProcessAIConspiracies();
};