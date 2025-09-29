#include "Core/GovernanceSubsystem.h"
#include "DarkAge.h"
#include "Core/SocialSimulationSubsystem.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Misc/Paths.h"

UGovernanceSubsystem::UGovernanceSubsystem() {}

void UGovernanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Check for a reset flag to force-delete corrupted save file
	FString ResetFilePath = FPaths::ProjectSavedDir() + TEXT("ResetGovernance.flag");
	if (IFileManager::Get().FileExists(*ResetFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetGovernance.flag found. Deleting Governance.sav."));
		FString SavePath = GetSaveFileName();
		IFileManager::Get().Delete(*SavePath);
		IFileManager::Get().Delete(*ResetFilePath); // Delete the flag file itself
	}

	LoadGovernance();

	// Cache subsystem references
    SocialSimulationSubsystem = GetGameInstance()->GetSubsystem<USocialSimulationSubsystem>();
    UE_LOG(LogTemp, Log, TEXT("Governance System initialized"));

    // If no governance data was loaded, create default structures
    if (FactionGovernanceStructures.Num() == 0)
    {
        if (SocialSimulationSubsystem)
        {
            // Create basic democratic governance for the player faction
            CreateFactionGovernance(FName("PlayerFaction"), EGovernanceType::DirectDemocracy);
        }
    }


    UE_LOG(LogTemp, Log, TEXT("Governance System started and ready for player interaction"));
}

void UGovernanceSubsystem::Deinitialize()
{
    SaveGovernance();
    Super::Deinitialize();
}

void UGovernanceSubsystem::Tick(float DeltaTime)
{
    // Process expired proposals
    ProcessExpiredProposals();

    // Update governance metrics
    UpdateGovernanceMetrics(DeltaTime);
}

bool UGovernanceSubsystem::IsTickable() const
{
    return true;
}

TStatId UGovernanceSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UGovernanceSubsystem, STATGROUP_Tickables);
}

void UGovernanceSubsystem::CreateFactionGovernance(FName FactionID, EGovernanceType GovernanceType)
{
    if (!SocialSimulationSubsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot create faction governance - SocialSimulationSubsystem not available"));
        return;
    }
    
    // Verify faction exists
    FFactionData FactionData = SocialSimulationSubsystem->GetFactionData(FactionID);
    if (FactionData.FactionName == TEXT("Unknown"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot create governance for unknown faction: %s"), *FactionID.ToString());
        return;
    }
    
    // Create governance structure
    FFactionGovernance NewGovernance;
    NewGovernance.FactionID = FactionID;
    NewGovernance.GovernanceType = GovernanceType;
    
    // Set default values based on governance type
    switch (GovernanceType)
    {
        case EGovernanceType::DirectDemocracy:
            NewGovernance.bAllowMemberProposals = true;
            NewGovernance.bAllowPublicProposals = false;
            NewGovernance.QuorumRequirement = 0.5f;
            break;
            
        case EGovernanceType::RepresentativeDemocracy:
            NewGovernance.bAllowMemberProposals = false;
            NewGovernance.bAllowPublicProposals = true;
            NewGovernance.QuorumRequirement = 0.3f;
            break;
            
        case EGovernanceType::Council:
            NewGovernance.bAllowMemberProposals = true;
            NewGovernance.bAllowPublicProposals = false;
            NewGovernance.QuorumRequirement = 0.6f;
            NewGovernance.MinimumMembersForVoting = 3;
            break;
            
        case EGovernanceType::Monarchy:
            NewGovernance.bAllowMemberProposals = false;
            NewGovernance.bAllowPublicProposals = false;
            NewGovernance.QuorumRequirement = 0.1f; // Only the monarch needs to decide
            break;
            
        default:
            // Use defaults
            break;
    }
    
    FactionGovernanceStructures.Add(FactionID, NewGovernance);
    
    UE_LOG(LogTemp, Log, TEXT("Created %s governance for faction '%s'"), 
           *UEnum::GetValueAsString(GovernanceType), *FactionID.ToString());
}

void UGovernanceSubsystem::UpdateFactionGovernance(FName FactionID, const FFactionGovernance& NewGovernance)
{
    FFactionGovernance* ExistingGovernance = FactionGovernanceStructures.Find(FactionID);
    if (ExistingGovernance)
    {
        *ExistingGovernance = NewGovernance;
        UE_LOG(LogTemp, Log, TEXT("Updated governance structure for faction '%s'"), *FactionID.ToString());
    }
}

FFactionGovernance UGovernanceSubsystem::GetFactionGovernance(FName FactionID) const
{
    const FFactionGovernance* Governance = FactionGovernanceStructures.Find(FactionID);
    return Governance ? *Governance : FFactionGovernance();
}

bool UGovernanceSubsystem::CanCharacterParticipateInFactionGovernance(const FString& CharacterID, FName FactionID) const
{
    if (!SocialSimulationSubsystem)
    {
        return false;
    }
    
    // Check if character is a member of the faction
    TArray<FName> CharacterFactions = SocialSimulationSubsystem->GetCharacterFactions(CharacterID);
    return CharacterFactions.Contains(FactionID);
}

FGuid UGovernanceSubsystem::SubmitProposal(const FString& ProposerID, FName FactionID, const FString& Title, const FString& Description, ELawCategory Category)
{
    // Validate proposer can submit
    if (!CanCharacterParticipateInFactionGovernance(ProposerID, FactionID))
    {
        FFactionGovernance Governance = GetFactionGovernance(FactionID);
        if (!Governance.bAllowPublicProposals)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character %s cannot submit proposals to faction %s"), *ProposerID, *FactionID.ToString());
            return FGuid();
        }
    }
    
    // Check cooldown
    UWorld* World = GetWorld();
    if (!World)
    {
        return FGuid();
    }
    float CurrentTime = World->GetTimeSeconds();
    const float* LastProposalTime = LastProposalTimeByCharacter.Find(ProposerID);
    if (LastProposalTime)
    {
        FFactionGovernance Governance = GetFactionGovernance(FactionID);
        if (CurrentTime - *LastProposalTime < Governance.ProposalCooldownTime)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character %s must wait before submitting another proposal"), *ProposerID);
            return FGuid();
        }
    }
    
    // Check faction proposal limit
    int32 ActiveProposalCount = 0;
    for (const auto& ProposalPair : ActiveProposals)
    {
        if (ProposalPair.Value.FactionID == FactionID && ProposalPair.Value.bIsActive)
        {
            ActiveProposalCount++;
        }
    }
    
    if (ActiveProposalCount >= MaxActiveProposalsPerFaction)
    {
        UE_LOG(LogTemp, Warning, TEXT("Faction %s has reached maximum active proposals limit"), *FactionID.ToString());
        return FGuid();
    }
    
    // Create proposal
    FGovernanceProposal NewProposal;
    NewProposal.ProposerCharacterID = ProposerID;
    NewProposal.FactionID = FactionID;
    NewProposal.ProposalName = FText::FromString(Title);
    NewProposal.Description = FText::FromString(Description);
    NewProposal.ProposalType = Category;
    NewProposal.Status = EProposalStatus::Pending;
    NewProposal.SubmissionTime = CurrentTime;
    
    ActiveProposals.Add(NewProposal.ProposalID, NewProposal);
    LastProposalTimeByCharacter.Add(ProposerID, CurrentTime);
    
    // Broadcast event
    OnProposalSubmitted.Broadcast(NewProposal.ProposalID, NewProposal);
    
    UE_LOG(LogTemp, Log, TEXT("Proposal '%s' submitted by %s for faction %s"), *NewProposal.ProposalName.ToString(), *ProposerID, *FactionID.ToString());
    
    return NewProposal.ProposalID;
}

bool UGovernanceSubsystem::StartVotingOnProposal(FGuid ProposalID)
{
    FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal || !Proposal->bIsActive)
    {
        return false;
    }
    
    if (Proposal->Status != EProposalStatus::Pending)
    {
        return false;
    }
    
    FFactionGovernance Governance = GetFactionGovernance(Proposal->FactionID);
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    Proposal->Status = EProposalStatus::Approved;
    Proposal->VotingStartTime = CurrentTime;
    Proposal->VotingEndTime = CurrentTime + Governance.VotingDuration;
    
    // Broadcast event
    OnVotingStarted.Broadcast(ProposalID, *Proposal);
    
    UE_LOG(LogTemp, Log, TEXT("Voting started on proposal '%s' for faction %s"), *Proposal->ProposalName.ToString(), *Proposal->FactionID.ToString());
    
    return true;
}

FGovernanceProposal UGovernanceSubsystem::GetProposal(FGuid ProposalID) const
{
    const FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    return Proposal ? *Proposal : FGovernanceProposal();
}

TArray<FGovernanceProposal> UGovernanceSubsystem::GetActiveProposals(FName FactionID) const
{
    TArray<FGovernanceProposal> Result;
    
    for (const auto& ProposalPair : ActiveProposals)
    {
        const FGovernanceProposal& Proposal = ProposalPair.Value;
        if (Proposal.bIsActive && (FactionID.IsNone() || Proposal.FactionID == FactionID))
        {
            Result.Add(Proposal);
        }
    }
    
    return Result;
}

TArray<FGovernanceProposal> UGovernanceSubsystem::GetProposalsByStatus(EProposalStatus Status, FName FactionID) const
{
    TArray<FGovernanceProposal> Result;
    
    for (const auto& ProposalPair : ActiveProposals)
    {
        const FGovernanceProposal& Proposal = ProposalPair.Value;
        if (Proposal.Status == Status && (FactionID.IsNone() || Proposal.FactionID == FactionID))
        {
            Result.Add(Proposal);
        }
    }
    
    return Result;
}

bool UGovernanceSubsystem::SubmitVote(FGuid ProposalID, const FString& VoterID, bool bVoteFor)
{
    FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal || !IsVotingOpen(ProposalID))
    {
        return false;
    }
    
    // Check if voter is eligible
    if (!CanCharacterParticipateInFactionGovernance(VoterID, Proposal->FactionID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character %s is not eligible to vote on this proposal"), *VoterID);
        return false;
    }
    
    // Check if already voted
    if (Proposal->VoterChoices.Contains(VoterID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character %s has already voted on this proposal"), *VoterID);
        return false;
    }
    
    // Record vote
    Proposal->VoterChoices.Add(VoterID, bVoteFor);
    if (bVoteFor)
    {
        Proposal->VotesFor++;
    }
    else
    {
        Proposal->VotesAgainst++;
    }
    
    // Broadcast event
    OnVoteCast.Broadcast(ProposalID, VoterID, bVoteFor);
    
    UE_LOG(LogTemp, Log, TEXT("Vote cast by %s on proposal '%s': %s"), *VoterID, *Proposal->ProposalName.ToString(), bVoteFor ? TEXT("For") : TEXT("Against"));
    
    return true;
}

bool UGovernanceSubsystem::SubmitAbstention(FGuid ProposalID, const FString& VoterID)
{
    FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal || !IsVotingOpen(ProposalID))
    {
        return false;
    }
    
    // Check if voter is eligible
    if (!CanCharacterParticipateInFactionGovernance(VoterID, Proposal->FactionID))
    {
        return false;
    }
    
    // Check if already voted
    if (Proposal->VoterChoices.Contains(VoterID))
    {
        return false;
    }
    
    // Record abstention (stored as voter participating but not counted in for/against)
    Proposal->VoterChoices.Add(VoterID, false); // Value doesn't matter for abstentions
    Proposal->Abstentions++;
    
    UE_LOG(LogTemp, Log, TEXT("Abstention submitted by %s on proposal '%s'"), *VoterID, *Proposal->ProposalName.ToString());
    
    return true;
}

bool UGovernanceSubsystem::HasCharacterVoted(FGuid ProposalID, const FString& CharacterID) const
{
    const FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    return Proposal && Proposal->VoterChoices.Contains(CharacterID);
}

bool UGovernanceSubsystem::IsVotingOpen(FGuid ProposalID) const
{
    const FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal || Proposal->Status != EProposalStatus::Approved)
    {
        return false;
    }
    
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    float CurrentTime = World->GetTimeSeconds();
    return CurrentTime >= Proposal->VotingStartTime && CurrentTime <= Proposal->VotingEndTime;
}

void UGovernanceSubsystem::ProcessVoteResults(FGuid ProposalID)
{
    FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal)
    {
        return;
    }
    
    FFactionGovernance Governance = GetFactionGovernance(Proposal->FactionID);
    
    // Check if quorum was met
    if (!MeetsQuorumRequirement(*Proposal, Proposal->FactionID))
    {
        Proposal->Status = EProposalStatus::Rejected;
        UE_LOG(LogTemp, Log, TEXT("Proposal '%s' failed due to insufficient quorum"), *Proposal->ProposalName.ToString());
        return;
    }
    
    // Calculate result based on vote type
    bool bPassed = CalculateVoteResult(*Proposal, Proposal->RequiredVoteType);
    
    if (bPassed)
    {
        Proposal->Status = EProposalStatus::Approved;
        OnProposalPassed.Broadcast(ProposalID, *Proposal);
        UE_LOG(LogTemp, Log, TEXT("Proposal '%s' passed with %d votes for and %d votes against"), *Proposal->ProposalName.ToString(), Proposal->VotesFor, Proposal->VotesAgainst);
    }
    else
    {
        Proposal->Status = EProposalStatus::Rejected;
        UE_LOG(LogTemp, Log, TEXT("Proposal '%s' failed with %d votes for and %d votes against"), *Proposal->ProposalName.ToString(), Proposal->VotesFor, Proposal->VotesAgainst);
    }
}

FGuid UGovernanceSubsystem::EnactLaw(FGuid ProposalID, const FString& LawTitle, const FString& LawContent)
{
    FGovernanceProposal* Proposal = ActiveProposals.Find(ProposalID);
    if (!Proposal || Proposal->Status != EProposalStatus::Approved)
    {
        return FGuid();
    }
    
    // Create law
    FLawData NewLaw;
    NewLaw.LawName = LawTitle.IsEmpty() ? Proposal->ProposalName : FText::FromString(LawTitle);
    NewLaw.Description = LawContent.IsEmpty() ? Proposal->Description : FText::FromString(LawContent);
    NewLaw.Category = Proposal->ProposalType;
    NewLaw.FactionID = Proposal->FactionID;
    NewLaw.OriginProposalID = ProposalID;
    if (UWorld* World = GetWorld())
    {
        NewLaw.EnactmentTime = World->GetTimeSeconds();
    }
    
    EnactedLaws.Add(NewLaw.LawID, NewLaw);
    
    // Update proposal status
    Proposal->Status = EProposalStatus::Rejected;
    
    // Broadcast event
    OnLawEnacted.Broadcast(NewLaw.LawID, NewLaw);
    
    UE_LOG(LogTemp, Log, TEXT("Law '%s' enacted for faction %s"), *NewLaw.LawName.ToString(), *NewLaw.FactionID.ToString());
    
    return NewLaw.LawID;
}

bool UGovernanceSubsystem::RepealLaw(FGuid LawID, FName FactionID)
{
    FLawData* Law = EnactedLaws.Find(LawID);
    if (!Law || Law->FactionID != FactionID || !Law->bIsActive)
    {
        return false;
    }
    
    Law->bIsActive = false;
    
    
    UE_LOG(LogTemp, Log, TEXT("Law '%s' repealed for faction %s"), *Law->LawName.ToString(), *FactionID.ToString());
    
    return true;
}

FLawData UGovernanceSubsystem::GetLaw(FGuid LawID) const
{
    const FLawData* Law = EnactedLaws.Find(LawID);
    return Law ? *Law : FLawData();
}

TArray<FLawData> UGovernanceSubsystem::GetActiveLaws(FName FactionID) const
{
    TArray<FLawData> Result;
    
    for (const auto& LawPair : EnactedLaws)
    {
        const FLawData& Law = LawPair.Value;
        if (Law.bIsActive && (FactionID.IsNone() || Law.FactionID == FactionID))
        {
            Result.Add(Law);
        }
    }
    
    return Result;
}

TArray<FLawData> UGovernanceSubsystem::GetLawsByCategory(ELawCategory Category, FName FactionID) const
{
    TArray<FLawData> Result;
    
    for (const auto& LawPair : EnactedLaws)
    {
        const FLawData& Law = LawPair.Value;
        if (Law.bIsActive && Law.Category == Category && (FactionID.IsNone() || Law.FactionID == FactionID))
        {
            Result.Add(Law);
        }
    }
    
    return Result;
}

void UGovernanceSubsystem::UpdateLawEffectiveness(FGuid LawID, float NewEffectiveness)
{
    FLawData* Law = EnactedLaws.Find(LawID);
    if (Law)
    {
        Law->EffectivenessRating = FMath::Clamp(NewEffectiveness, 0.0f, 100.0f);
    }
}

float UGovernanceSubsystem::GetFactionGovernanceStability(FName FactionID) const
{
    // Calculate stability based on recent voting patterns, law effectiveness, etc.
    const FFactionGovernance* Governance = FactionGovernanceStructures.Find(FactionID);
    if (!Governance)
    {
        return 0.0f;
    }
    
    // Calculate recent proposal pass rate
    TArray<FGovernanceProposal> PassedProposals = GetProposalsByStatus(EProposalStatus::Approved, FactionID);
    TArray<FGovernanceProposal> FailedProposals = GetProposalsByStatus(EProposalStatus::Rejected, FactionID);

    int32 TotalRecentProposals = 0;
    int32 PassedRecentProposals = 0;
    float RecentPassRate = 0.5f; // Default to 50% if no recent proposals

    float RecentTimeframe = 3600.0f; // Consider proposals in the last hour as recent
    UWorld* World = GetWorld();
    if (!World)
    {
        return 0.0f;
    }
    float CurrentTime = World->GetTimeSeconds();

    for (const auto& Proposal : PassedProposals)
    {
        if (CurrentTime - Proposal.SubmissionTime <= RecentTimeframe)
        {
            TotalRecentProposals++;
            PassedRecentProposals++;
        }
    }
    for (const auto& Proposal : FailedProposals)
    {
        if (CurrentTime - Proposal.SubmissionTime <= RecentTimeframe)
        {
            TotalRecentProposals++;
        }
    }

    if (TotalRecentProposals > 0)
    {
        RecentPassRate = (float)PassedRecentProposals / TotalRecentProposals;
    }

    // Calculate average law effectiveness
    TArray<FLawData> ActiveLaws = GetActiveLaws(FactionID);
    float TotalEffectiveness = 0.0f;
    float AverageLawEffectiveness = 50.0f; // Default to 50% if no laws

    if (ActiveLaws.Num() > 0)
    {
        for (const FLawData& Law : ActiveLaws)
        {
            TotalEffectiveness += Law.EffectivenessRating;
        }
        AverageLawEffectiveness = TotalEffectiveness / ActiveLaws.Num();
    }
    
    return (RecentPassRate * 50.0f) + (AverageLawEffectiveness * 0.5f);
}

TArray<FString> UGovernanceSubsystem::GetMostActiveProposers(FName FactionID, int32 Count) const
{
    TMap<FString, int32> ProposerCounts;
    
    for (const auto& ProposalPair : ActiveProposals)
    {
        const FGovernanceProposal& Proposal = ProposalPair.Value;
        if (FactionID.IsNone() || Proposal.FactionID == FactionID)
        {
            int32* CurrentCount = ProposerCounts.Find(Proposal.ProposerCharacterID);
            if (CurrentCount)
            {
                (*CurrentCount)++;
            }
            else
            {
                ProposerCounts.Add(Proposal.ProposerCharacterID, 1);
            }
        }
    }
    
    // Sort by count
    ProposerCounts.ValueSort([](const int32& A, const int32& B) { return A > B; });
    
    TArray<FString> Result;
    for (const auto& ProposerPair : ProposerCounts)
    {
        if (Result.Num() >= Count)
        {
            break;
        }
        Result.Add(ProposerPair.Key);
    }
    
    return Result;
}

float UGovernanceSubsystem::GetVoterParticipationRate(FName FactionID) const
{
    if (!SocialSimulationSubsystem)
    {
        return 0.0f;
    }
    
    // Get recent voting proposals
    TArray<FGovernanceProposal> RecentProposals = GetProposalsByStatus(EProposalStatus::Approved, FactionID);
    RecentProposals.Append(GetProposalsByStatus(EProposalStatus::Rejected, FactionID));
    
    if (RecentProposals.Num() == 0)
    {
        return 0.0f;
    }
    
    int32 TotalVotes = 0;
    int32 TotalEligibleVoters = 0;
    
    for (const FGovernanceProposal& Proposal : RecentProposals)
    {
        TArray<FString> EligibleVoters = GetEligibleVoters(Proposal.FactionID);
        TotalEligibleVoters += EligibleVoters.Num();
        TotalVotes += Proposal.VoterChoices.Num();
    }
    
    return TotalEligibleVoters > 0 ? (float)TotalVotes / (float)TotalEligibleVoters : 0.0f;
}

TArray<FGuid> UGovernanceSubsystem::GetRecentlyPassedLaws(FName FactionID, float TimeRange) const
{
    TArray<FGuid> Result;
    UWorld* World = GetWorld();
    if (!World)
    {
        return Result;
    }
    float CurrentTime = World->GetTimeSeconds();
    float CutoffTime = CurrentTime - TimeRange;
    
    for (const auto& LawPair : EnactedLaws)
    {
        const FLawData& Law = LawPair.Value;
        if (Law.bIsActive && Law.EnactmentTime >= CutoffTime && (FactionID.IsNone() || Law.FactionID == FactionID))
        {
            Result.Add(Law.LawID);
        }
    }
    
    return Result;
}

void UGovernanceSubsystem::SaveGovernance() const
{
    TArray<uint8> SaveData;
    FMemoryWriter MemoryWriter(SaveData, true);

    // Manually serialize TMaps to handle potential issues with non-standard types.
    int32 FactionGovCount = FactionGovernanceStructures.Num();
    MemoryWriter << FactionGovCount;
    for (const auto& Pair : FactionGovernanceStructures)
    {
        FName Key = Pair.Key;
        FFactionGovernance Value = Pair.Value;
        MemoryWriter << Key;
        MemoryWriter << Value;
    }

    int32 ActiveProposalsCount = ActiveProposals.Num();
    MemoryWriter << ActiveProposalsCount;
    for (const auto& Pair : ActiveProposals)
    {
        FGuid Key = Pair.Key;
        FGovernanceProposal Value = Pair.Value;
        MemoryWriter << Key;
        MemoryWriter << Value;
    }

    int32 EnactedLawsCount = EnactedLaws.Num();
    MemoryWriter << EnactedLawsCount;
    for (const auto& Pair : EnactedLaws)
    {
        FGuid Key = Pair.Key;
        FLawData Value = Pair.Value;
        MemoryWriter << Key;
        MemoryWriter << Value;
    }

    int32 LastProposalCount = LastProposalTimeByCharacter.Num();
    MemoryWriter << LastProposalCount;
    for (const auto& Pair : LastProposalTimeByCharacter)
    {
        FString Key = Pair.Key;
        float Value = Pair.Value;
        MemoryWriter << Key;
        MemoryWriter << Value;
    }

    if (FFileHelper::SaveArrayToFile(SaveData, *GetSaveFileName()))
    {
        UE_LOG(LogTemp, Log, TEXT("Governance data saved to %s"), *GetSaveFileName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save governance data to %s"), *GetSaveFileName());
    }
}

void UGovernanceSubsystem::LoadGovernance()
{
    TArray<uint8> SaveData;
    if (!FFileHelper::LoadFileToArray(SaveData, *GetSaveFileName()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Governance save file not found: %s"), *GetSaveFileName());
        return;
    }

    FMemoryReader MemoryReader(SaveData, true);
    // Manually deserialize TMaps.
    int32 FactionGovCount = 0;
    MemoryReader << FactionGovCount;
    FactionGovernanceStructures.Empty(FactionGovCount);
    for (int32 i = 0; i < FactionGovCount; ++i)
    {
        FName Key;
        FFactionGovernance Value;
        MemoryReader << Key;
        MemoryReader << Value;
        FactionGovernanceStructures.Add(Key, Value);
    }

    int32 ActiveProposalsCount = 0;
    MemoryReader << ActiveProposalsCount;
    ActiveProposals.Empty(ActiveProposalsCount);
    for (int32 i = 0; i < ActiveProposalsCount; ++i)
    {
        FGuid Key;
        FGovernanceProposal Value;
        MemoryReader << Key;
        MemoryReader << Value;
        ActiveProposals.Add(Key, Value);
    }

    int32 EnactedLawsCount = 0;
    MemoryReader << EnactedLawsCount;
    EnactedLaws.Empty(EnactedLawsCount);
    for (int32 i = 0; i < EnactedLawsCount; ++i)
    {
        FGuid Key;
        FLawData Value;
        MemoryReader << Key;
        MemoryReader << Value;
        EnactedLaws.Add(Key, Value);
    }

    int32 LastProposalCount = 0;
    MemoryReader << LastProposalCount;
    LastProposalTimeByCharacter.Empty(LastProposalCount);
    for (int32 i = 0; i < LastProposalCount; ++i)
    {
        FString Key;
        float Value;
        MemoryReader << Key;
        MemoryReader << Value;
        LastProposalTimeByCharacter.Add(Key, Value);
    }

    UE_LOG(LogTemp, Log, TEXT("Governance data loaded from %s"), *GetSaveFileName());
}

FString UGovernanceSubsystem::GetSaveFileName() const
{
    return FPaths::ProjectSavedDir() + TEXT("Governance.sav");
}

// Private helper functions

bool UGovernanceSubsystem::ValidateProposal(const FGovernanceProposal& Proposal) const
{
    return !Proposal.ProposalName.IsEmpty() && !Proposal.Description.IsEmpty() && !Proposal.ProposerCharacterID.IsEmpty();
}

bool UGovernanceSubsystem::CalculateVoteResult(const FGovernanceProposal& Proposal, EVoteType VoteType) const
{
    int32 TotalVotes = Proposal.VotesFor + Proposal.VotesAgainst;
    
    if (TotalVotes == 0)
    {
        return false;
    }
    
    float ForPercentage = (float)Proposal.VotesFor / (float)TotalVotes;
    
    switch (VoteType)
    {
        case EVoteType::Majority:
            return ForPercentage > 0.5f;
            
        case EVoteType::SuperMajority:
            return ForPercentage >= 0.6667f;
            
        case EVoteType::Unanimous:
            return Proposal.VotesAgainst == 0 && Proposal.VotesFor > 0;
            
        default:
            return ForPercentage > 0.5f;
    }
}

void UGovernanceSubsystem::ProcessExpiredProposals()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    TArray<FGuid> ProposalsToProcess;
    
    for (auto& ProposalPair : ActiveProposals)
    {
        FGovernanceProposal& Proposal = ProposalPair.Value;
        
        if (Proposal.Status == EProposalStatus::Approved && CurrentTime > Proposal.VotingEndTime)
        {
            ProposalsToProcess.Add(Proposal.ProposalID);
        }
    }
    
    for (FGuid ProposalID : ProposalsToProcess)
    {
        ProcessVoteResults(ProposalID);
    }
}

void UGovernanceSubsystem::UpdateGovernanceMetrics(float DeltaTime)
{
    // Update law effectiveness based on time and events
    for (auto& LawPair : EnactedLaws)
    {
        FLawData& Law = LawPair.Value;
        if (Law.bIsActive)
        {
            // Simple effectiveness decay over time - laws become less effective if not updated
            float DecayRate = 0.1f; // 0.1% per update cycle
            Law.EffectivenessRating = FMath::Max(0.0f, Law.EffectivenessRating - DecayRate);
        }
    }
}

bool UGovernanceSubsystem::MeetsQuorumRequirement(const FGovernanceProposal& Proposal, FName FactionID) const
{
    TArray<FString> EligibleVoters = GetEligibleVoters(FactionID);
    
    if (EligibleVoters.Num() == 0)
    {
        return false;
    }
    
    FFactionGovernance Governance = GetFactionGovernance(FactionID);
    int32 RequiredVotes = FMath::CeilToInt(EligibleVoters.Num() * Governance.QuorumRequirement);
    
    return Proposal.VoterChoices.Num() >= RequiredVotes;
}

TArray<FString> UGovernanceSubsystem::GetEligibleVoters(FName FactionID) const
{
    if (!SocialSimulationSubsystem)
    {
        return TArray<FString>();
    }
    
    FFactionData FactionData = SocialSimulationSubsystem->GetFactionData(FactionID);
    return FactionData.MemberCharacters;
}
