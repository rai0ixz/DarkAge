#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GovernanceData.generated.h"

UENUM(BlueprintType)
enum class EGovernanceType : uint8
{
	Monarchy,
	Feudal,
	Republic,
	Theocracy,
	DirectDemocracy,
	RepresentativeDemocracy,
	Council
};

UENUM(BlueprintType)
enum class ELawCategory : uint8
{
	Economic,
	Social,
	Military,
	Religious
};

UENUM(BlueprintType)
enum class EProposalStatus : uint8
{
	Pending,
	Approved,
	Rejected
};

UENUM(BlueprintType)
enum class ETreatyType : uint8
{
	None,
	Trade,
	NonAggression,
	Defensive,
	Alliance
};

UENUM(BlueprintType)
enum class EVoteType : uint8
{
	Unanimous,
	Majority,
	SuperMajority
};

USTRUCT(BlueprintType)
struct FLawData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid LawID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText LawName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 FName FactionID;

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 ELawCategory Category = ELawCategory::Economic;
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 FGuid OriginProposalID;
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 float EnactmentTime = 0.0f;
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 bool bIsActive = false;
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 float EffectivenessRating = 0.0f;
};

USTRUCT(BlueprintType)
struct FGovernanceProposal
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid ProposalID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ProposalName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FactionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ProposerCharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELawCategory ProposalType = ELawCategory::Economic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProposalStatus Status = EProposalStatus::Pending;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SubmissionTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VotingStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VotingEndTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VotesFor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VotesAgainst = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Abstentions = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, bool> VoterChoices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoteType RequiredVoteType = EVoteType::Majority;
};

USTRUCT(BlueprintType)
struct FFactionGovernance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FactionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGovernanceType GovernanceType = EGovernanceType::Monarchy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGuid> ActiveLaws;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowMemberProposals = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowPublicProposals = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QuorumRequirement = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinimumMembersForVoting = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProposalCooldownTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VotingDuration = 0.0f;
};

inline FArchive& operator<<(FArchive& Ar, FLawData& Law)
{
    Ar << Law.LawID;
    Ar << Law.LawName;
    Ar << Law.Description;
    Ar << Law.FactionID;
    Ar << Law.Category;
    Ar << Law.OriginProposalID;
    Ar << Law.EnactmentTime;
    Ar << Law.bIsActive;
    Ar << Law.EffectivenessRating;
    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, FGovernanceProposal& Proposal)
{
    Ar << Proposal.ProposalID;
    Ar << Proposal.ProposalName;
    Ar << Proposal.Description;
    Ar << Proposal.FactionID;
    Ar << Proposal.ProposerCharacterID;
    Ar << Proposal.ProposalType;
    Ar << Proposal.Status;
    Ar << Proposal.bIsActive;
    Ar << Proposal.SubmissionTime;
    Ar << Proposal.VotingStartTime;
    Ar << Proposal.VotingEndTime;
    Ar << Proposal.VotesFor;
    Ar << Proposal.VotesAgainst;
    Ar << Proposal.Abstentions;
    Ar << Proposal.VoterChoices;
    Ar << Proposal.RequiredVoteType;
    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, FFactionGovernance& Governance)
{
    Ar << Governance.FactionID;
    Ar << Governance.GovernanceType;
    Ar << Governance.ActiveLaws;
    Ar << Governance.bAllowMemberProposals;
    Ar << Governance.bAllowPublicProposals;
    Ar << Governance.QuorumRequirement;
    Ar << Governance.MinimumMembersForVoting;
    Ar << Governance.ProposalCooldownTime;
    Ar << Governance.VotingDuration;
    return Ar;
}