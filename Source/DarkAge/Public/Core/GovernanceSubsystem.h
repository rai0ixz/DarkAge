#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/SocialSimulationSubsystem.h"
#include "Data/GovernanceData.h"
#include "UObject/NameTypes.h"
#include "Misc/Guid.h"
#include "Serialization/Archive.h"
#include "GovernanceSubsystem.generated.h"

// Forward declarations
class USocialSimulationSubsystem;

/**
 * Player-Driven Governance Subsystem
 *
 * Implements comprehensive player governance systems including faction management,
 * proposal systems, voting mechanisms, and evolving legal frameworks.
 * Builds upon the SocialSimulationSubsystem to create dynamic political systems.
 *
 * Key Features:
 * - Multiple governance types (democracy, monarchy, council, etc.)
 * - Player-submitted proposals and legislation
 * - Flexible voting systems (majority, consensus, ranked choice, etc.)
 * - Dynamic law creation and evolution
 * - Faction-based political structures
 * - Representative and direct democracy systems
 * - Constitutional frameworks and amendments
 *
 * Integration Points:
 * - SocialSimulationSubsystem: Faction data and social dynamics
 * - PlayerLegacySubsystem: Political legacy tracking
 * - WorldManagementSubsystem: Regional governance effects
 * - QuestManagementSubsystem: Political quest generation
 */
UCLASS()
class DARKAGE_API UGovernanceSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UGovernanceSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// FTickableGameObject overrides
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

    /**
     * Faction Governance Management
     */
    UFUNCTION(BlueprintCallable, Category = "Governance|Factions")
    void CreateFactionGovernance(FName FactionID, EGovernanceType GovernanceType);

    UFUNCTION(BlueprintCallable, Category = "Governance|Factions")
    void UpdateFactionGovernance(FName FactionID, const FFactionGovernance& NewGovernance);

    UFUNCTION(BlueprintPure, Category = "Governance|Factions")
    FFactionGovernance GetFactionGovernance(FName FactionID) const;

    UFUNCTION(BlueprintCallable, Category = "Governance|Factions")
    bool CanCharacterParticipateInFactionGovernance(const FString& CharacterID, FName FactionID) const;

    /**
     * Proposal System
     */
    UFUNCTION(BlueprintCallable, Category = "Governance|Proposals")
    FGuid SubmitProposal(const FString& ProposerID, FName FactionID, const FString& Title, const FString& Description, ELawCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Governance|Proposals")
    bool StartVotingOnProposal(FGuid ProposalID);

    UFUNCTION(BlueprintPure, Category = "Governance|Proposals")
    FGovernanceProposal GetProposal(FGuid ProposalID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Proposals")
    TArray<FGovernanceProposal> GetActiveProposals(FName FactionID) const; // FIX: Removed default parameter

    UFUNCTION(BlueprintPure, Category = "Governance|Proposals")
    TArray<FGovernanceProposal> GetProposalsByStatus(EProposalStatus Status, FName FactionID) const;

    /**
     * Voting System
     */
    UFUNCTION(BlueprintCallable, Category = "Governance|Voting")
    bool SubmitVote(FGuid ProposalID, const FString& VoterID, bool bVoteFor);

    UFUNCTION(BlueprintCallable, Category = "Governance|Voting")
    bool SubmitAbstention(FGuid ProposalID, const FString& VoterID);

    UFUNCTION(BlueprintPure, Category = "Governance|Voting")
    bool HasCharacterVoted(FGuid ProposalID, const FString& CharacterID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Voting")
    bool IsVotingOpen(FGuid ProposalID) const;

    UFUNCTION(BlueprintCallable, Category = "Governance|Voting")
    void ProcessVoteResults(FGuid ProposalID);

    /**
     * Law System
     */
    UFUNCTION(BlueprintCallable, Category = "Governance|Laws")
    FGuid EnactLaw(FGuid ProposalID, const FString& LawTitle, const FString& LawContent);

    UFUNCTION(BlueprintCallable, Category = "Governance|Laws")
    bool RepealLaw(FGuid LawID, FName FactionID);

    UFUNCTION(BlueprintPure, Category = "Governance|Laws")
    FLawData GetLaw(FGuid LawID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Laws")
    TArray<FLawData> GetActiveLaws(FName FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Laws")
    TArray<FLawData> GetLawsByCategory(ELawCategory Category, FName FactionID) const;

    UFUNCTION(BlueprintCallable, Category = "Governance|Laws")
    void UpdateLawEffectiveness(FGuid LawID, float NewEffectiveness);

    /**
     * Analytics and Queries
     */
    UFUNCTION(BlueprintPure, Category = "Governance|Analytics")
    float GetFactionGovernanceStability(FName FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Analytics")
    TArray<FString> GetMostActiveProposers(FName FactionID, int32 Count = 5) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Analytics")
    float GetVoterParticipationRate(FName FactionID) const;

    UFUNCTION(BlueprintPure, Category = "Governance|Analytics")
    TArray<FGuid> GetRecentlyPassedLaws(FName FactionID, float TimeRange = 604800.0f) const; // 1 week

    // Event delegates
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProposalSubmitted, FGuid, ProposalID, FGovernanceProposal, Proposal);
    UPROPERTY(BlueprintAssignable, Category = "Governance Events")
    FOnProposalSubmitted OnProposalSubmitted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVotingStarted, FGuid, ProposalID, FGovernanceProposal, Proposal);
    UPROPERTY(BlueprintAssignable, Category = "Governance Events")
    FOnVotingStarted OnVotingStarted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVoteCast, FGuid, ProposalID, FString, VoterID, bool, bVoteFor);
    UPROPERTY(BlueprintAssignable, Category = "Governance Events")
    FOnVoteCast OnVoteCast;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProposalPassed, FGuid, ProposalID, FGovernanceProposal, Proposal);
    UPROPERTY(BlueprintAssignable, Category = "Governance Events")
    FOnProposalPassed OnProposalPassed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLawEnacted, FGuid, LawID, FLawData, Law);
    UPROPERTY(BlueprintAssignable, Category = "Governance Events")
    FOnLawEnacted OnLawEnacted;

protected:
    // Storage systems
    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Governance Data")
    TMap<FName, FFactionGovernance> FactionGovernanceStructures;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Governance Data")
    TMap<FGuid, FGovernanceProposal> ActiveProposals;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Governance Data")
    TMap<FGuid, FLawData> EnactedLaws;

    // Cached subsystem references
    UPROPERTY()
    USocialSimulationSubsystem* SocialSimulationSubsystem;

    // Configuration
    UPROPERTY(EditAnywhere, Category = "Governance Config")
    float DefaultVotingDuration = 259200.0f; // 3 days

    UPROPERTY(EditAnywhere, Category = "Governance Config")
    float DefaultProposalCooldown = 86400.0f; // 24 hours

    UPROPERTY(EditAnywhere, Category = "Governance Config")
    float DefaultQuorumRequirement = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Governance Config")
    int32 MaxActiveProposalsPerFaction = 10;

private:
    // Helper functions
    bool ValidateProposal(const FGovernanceProposal& Proposal) const;
    bool CalculateVoteResult(const FGovernanceProposal& Proposal, EVoteType VoteType) const;
    void ProcessExpiredProposals();
    void UpdateGovernanceMetrics(float DeltaTime);
    bool MeetsQuorumRequirement(const FGovernanceProposal& Proposal, FName FactionID) const;
    TArray<FString> GetEligibleVoters(FName FactionID) const;

    // Debug commands
    FString DebugListProposals(const TArray<FString>& Args);
    FString DebugListLaws(const TArray<FString>& Args);

    // Persistence
    void SaveGovernance() const;
    void LoadGovernance();
    FString GetSaveFileName() const;

    // Timing
    float LastProposalProcessingTime;
    float LastMetricsUpdateTime;

    // Proposal tracking by character
    TMap<FString, float> LastProposalTimeByCharacter;
};

inline FArchive& operator<<(FArchive& Ar, TMap<FName, FFactionGovernance>& Map)
{
    Ar << Map;
    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, TMap<FGuid, FGovernanceProposal>& Map)
{
    Ar << Map;
    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, TMap<FGuid, FLawData>& Map)
{
    Ar << Map;
    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, TMap<FString, float>& Map)
{
    Ar << Map;
    return Ar;
}