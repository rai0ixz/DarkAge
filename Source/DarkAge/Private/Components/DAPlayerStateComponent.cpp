#include "Components/DAPlayerStateComponent.h"
#include "Components/StatlineComponent.h"
#include "Components/NotorietyComponent.h"
#include "Data/PropertyData.h"
#include "Core/WorldManagementSubsystem.h"
#include "Core/QuestManagementSubsystem.h"
#include "Core/NetworkManagerSubsystem.h"
#include "Data/AchievementData.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"

UDAPlayerStateComponent::UDAPlayerStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    CurrentSocialStanding = EPlayerSocialStanding::Peasant;
    GlobalReputation = 0.0f;
    PlayerWealth = 100;
    SpouseID = TEXT("");
    SpouseName = TEXT("");
    LastIncomeProcessTime = 0.0f;
}

void UDAPlayerStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UDAPlayerStateComponent, CurrentSocialStanding);
    DOREPLIFETIME(UDAPlayerStateComponent, GlobalReputation);
    DOREPLIFETIME(UDAPlayerStateComponent, PlayerWealth);
    DOREPLIFETIME(UDAPlayerStateComponent, OwnedPropertyDetails);
    DOREPLIFETIME(UDAPlayerStateComponent, OwnedBusinessDetails);
    DOREPLIFETIME(UDAPlayerStateComponent, ReplicatedRegionalReputations);
    DOREPLIFETIME(UDAPlayerStateComponent, ReplicatedBountiesByRegion);
    DOREPLIFETIME(UDAPlayerStateComponent, ReplicatedFactionStandings);
    DOREPLIFETIME(UDAPlayerStateComponent, PlayerQuests);
    DOREPLIFETIME(UDAPlayerStateComponent, UnlockedStoryMilestones);
    DOREPLIFETIME_CONDITION(UDAPlayerStateComponent, PlayerAchievements, COND_OwnerOnly);
    DOREPLIFETIME(UDAPlayerStateComponent, SpouseID);
    DOREPLIFETIME(UDAPlayerStateComponent, SpouseName);
    DOREPLIFETIME(UDAPlayerStateComponent, FamilyMembers);
}

void UDAPlayerStateComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        StatlineComponent = Owner->FindComponentByClass<UStatlineComponent>();
        NotorietyComponent = Owner->FindComponentByClass<UNotorietyComponent>();
    }
    
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        QuestManagementSubsystem = GameInstance->GetSubsystem<UQuestManagementSubsystem>();
    }
    
    UWorldManagementSubsystem* WorldSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWorldManagementSubsystem>();
    LastIncomeProcessTime = WorldSubsystem ? WorldSubsystem->GetCurrentWorldTime() : 0.0f;

    UE_LOG(LogTemp, Log, TEXT("DAPlayerStateComponent initialized for %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
}

void UDAPlayerStateComponent::UpdateSocialStanding()
{
    if (GetOwner() && GetOwner()->GetNetMode() != NM_Standalone && GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        ServerUpdateSocialStanding();
    }
    else
    {
        EPlayerSocialStanding NewStanding = CalculateSocialStanding();
        
        if (NewStanding != CurrentSocialStanding)
        {
            EPlayerSocialStanding OldStanding = CurrentSocialStanding;
            CurrentSocialStanding = NewStanding;
            OnPlayerStateChanged.Broadcast(FName("SocialStanding"), (float)NewStanding);
        }
    }
}

float UDAPlayerStateComponent::GetRegionalReputation(const FString& RegionID) const
{
    for (const auto& Pair : ReplicatedRegionalReputations)
    {
        if (Pair.RegionID == RegionID)
        {
            return Pair.Reputation;
        }
    }
    return 0.0f;
}

void UDAPlayerStateComponent::AddProperty(const FString& PropertyID)
{
    if (GetOwner()->HasAuthority())
    {
        if (!PropertyID.IsEmpty() && !OwnedProperties.Contains(PropertyID))
        {
            OwnedProperties.Add(PropertyID);
            UpdateSocialStanding();
        }
    }
    else
    {
        ServerAddLegacyProperty(PropertyID);
    }
}

void UDAPlayerStateComponent::ProcessDailyIncomeAndExpenses()
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    int32 TotalIncome = GetTotalDailyIncome();
    int32 TotalExpenses = GetTotalDailyExpenses();

    PlayerWealth += TotalIncome;
    PlayerWealth -= TotalExpenses;
}

void UDAPlayerStateComponent::AddBounty(const FString& RegionID, float Amount, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        if (!RegionID.IsEmpty() && Amount > 0.0f)
        {
            FRegionBountyPair* BountyPair = ReplicatedBountiesByRegion.FindByPredicate([&RegionID](const FRegionBountyPair& Pair) {
                return Pair.RegionID == RegionID;
            });

            if (BountyPair)
            {
                BountyPair->Bounty += Amount;
            }
            else
            {
                ReplicatedBountiesByRegion.Add(FRegionBountyPair(RegionID, Amount));
            }
        }
    }
    else
    {
        ServerAddBounty(RegionID, Amount, Reason);
    }
}

void UDAPlayerStateComponent::ClearBounty(const FString& RegionID, float AmountPaid)
{
    if (GetOwner()->HasAuthority())
    {
        if (!RegionID.IsEmpty())
        {
            FRegionBountyPair* BountyPair = ReplicatedBountiesByRegion.FindByPredicate([&RegionID](const FRegionBountyPair& Pair) {
                return Pair.RegionID == RegionID;
            });

            if (BountyPair)
            {
                BountyPair->Bounty -= AmountPaid;
                if (BountyPair->Bounty <= 0)
                {
                    ReplicatedBountiesByRegion.RemoveAll([&RegionID](const FRegionBountyPair& Pair) {
                        return Pair.RegionID == RegionID;
                    });
                }
            }
        }
    }
    else
    {
        ServerClearBounty(RegionID, AmountPaid);
    }
}

void UDAPlayerStateComponent::RecordMajorEvent(const FString& EventDescription, const FString& RegionID)
{
    if (GetOwner()->HasAuthority())
    {
        if (!EventDescription.IsEmpty())
        {
            if (UWorld* World = GetWorld())
            {
                FString TimestampedEvent = FString::Printf(TEXT("Day %d: %s [%s]"), static_cast<int32>(World->GetTimeSeconds() / 86400), *EventDescription, *RegionID);
                PersonalHistory.Add(TimestampedEvent);
            }
        }
    }
    else
    {
        ServerRecordMajorEvent(EventDescription, RegionID);
    }
}

void UDAPlayerStateComponent::UnlockAchievement(const FName& AchievementID)
{
    if (GetOwner()->HasAuthority())
    {
        if (!AchievementID.IsNone())
        {
            FPlayerAchievementProgress* Progress = PlayerAchievements.FindByPredicate([&AchievementID](const FPlayerAchievementProgress& Prog) {
                return Prog.AchievementID == AchievementID;
            });

            if (Progress && !Progress->bIsUnlocked)
            {
                Progress->bIsUnlocked = true;
                if (UWorld* World = GetWorld())
                {
                    Progress->UnlockTimestamp = World->GetTimeSeconds();
                }
                OnAchievementUnlocked.Broadcast(AchievementID, FAchievementData());
            }
            else if (!Progress)
            {
                FPlayerAchievementProgress NewProgress;
                NewProgress.AchievementID = AchievementID;
                NewProgress.bIsUnlocked = true;
                if (UWorld* World = GetWorld())
                {
                    NewProgress.UnlockTimestamp = World->GetTimeSeconds();
                }
                PlayerAchievements.Add(NewProgress);
                OnAchievementUnlocked.Broadcast(AchievementID, FAchievementData());
            }
        }
    }
    else
    {
        ServerUnlockAchievement(AchievementID);
    }
}

void UDAPlayerStateComponent::UpdateAchievementProgress(const FName& AchievementID, int32 ProgressIncrement)
{
    if (GetOwner()->HasAuthority())
    {
        if (!AchievementID.IsNone() && ProgressIncrement > 0)
        {
            FPlayerAchievementProgress* Progress = PlayerAchievements.FindByPredicate([&AchievementID](const FPlayerAchievementProgress& Prog) {
                return Prog.AchievementID == AchievementID;
            });

            if (Progress && !Progress->bIsUnlocked)
            {
                Progress->CurrentProgress += ProgressIncrement;
                OnAchievementProgressUpdated.Broadcast(AchievementID, Progress->CurrentProgress, 100);
            }
        }
    }
    else
    {
        ServerUpdateAchievementProgress(AchievementID, ProgressIncrement);
    }
}

void UDAPlayerStateComponent::SetAchievementProgress(const FName& AchievementID, int32 NewProgress)
{
    if (GetOwner()->HasAuthority())
    {
        if (!AchievementID.IsNone() && NewProgress >= 0)
        {
            FPlayerAchievementProgress* Progress = PlayerAchievements.FindByPredicate([&AchievementID](const FPlayerAchievementProgress& Prog) {
                return Prog.AchievementID == AchievementID;
            });

            if (Progress && !Progress->bIsUnlocked)
            {
                Progress->CurrentProgress = NewProgress;
                OnAchievementProgressUpdated.Broadcast(AchievementID, Progress->CurrentProgress, 100);
            }
        }
    }
    else
    {
        ServerSetAchievementProgress(AchievementID, NewProgress);
    }
}

void UDAPlayerStateComponent::AddFamilyMember(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age)
{
    if (GetOwner()->HasAuthority())
    {
        if (!CharacterID.IsEmpty() && !CharacterName.IsEmpty() && !HasFamilyMember(CharacterID))
        {
            FFamilyMember NewMember;
            NewMember.CharacterID = CharacterID;
            NewMember.CharacterName = CharacterName;
            NewMember.RelationshipType = RelationshipType;
            NewMember.Age = Age;
            NewMember.IsAlive = true;
            FamilyMembers.Add(NewMember);
            OnFamilyMemberAdded.Broadcast(CharacterID, CharacterName, RelationshipType);
        }
    }
    else
    {
        ServerAddFamilyMember(CharacterID, CharacterName, RelationshipType, Age);
    }
}

void UDAPlayerStateComponent::RemoveFamilyMember(const FString& CharacterID, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        if (!CharacterID.IsEmpty())
        {
            int32 MemberIndex = FamilyMembers.IndexOfByPredicate([&CharacterID](const FFamilyMember& Member) {
                return Member.CharacterID == CharacterID;
            });

            if (MemberIndex != INDEX_NONE)
            {
                FamilyMembers.RemoveAt(MemberIndex);
                OnFamilyMemberRemoved.Broadcast(CharacterID, Reason);
            }
        }
    }
    else
    {
        ServerRemoveFamilyMember(CharacterID, Reason);
    }
}

void UDAPlayerStateComponent::UpdateFamilyMemberStatus(const FString& CharacterID, bool bIsAlive, int32 NewAge)
{
    if (GetOwner()->HasAuthority())
    {
        FFamilyMember* Member = FamilyMembers.FindByPredicate([&CharacterID](const FFamilyMember& FamMember) {
            return FamMember.CharacterID == CharacterID;
        });

        if (Member)
        {
            Member->IsAlive = bIsAlive;
            Member->Age = NewAge;
            OnFamilyMemberStatusChanged.Broadcast(CharacterID, bIsAlive);
        }
    }
    else
    {
        ServerUpdateFamilyMemberStatus(CharacterID, bIsAlive, NewAge);
    }
}

void UDAPlayerStateComponent::MarryCharacter(const FString& CharacterID, const FString& CharacterName)
{
    if (GetOwner()->HasAuthority())
    {
        if (!IsMarried())
        {
            SpouseID = CharacterID;
            SpouseName = CharacterName;
            OnPlayerMarried.Broadcast(CharacterID, CharacterName);
        }
    }
    else
    {
        ServerMarryCharacter(CharacterID, CharacterName);
    }
}

void UDAPlayerStateComponent::DivorceSpouse(const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        if (IsMarried())
        {
            SpouseID.Empty();
            SpouseName.Empty();
            OnPlayerDivorced.Broadcast(Reason);
        }
    }
    else
    {
        ServerDivorceSpouse(Reason);
    }
}

void UDAPlayerStateComponent::AddChild(const FString& ChildID, const FString& ChildName, int32 Age)
{
    AddFamilyMember(ChildID, ChildName, TEXT("Child"), Age);
}

void UDAPlayerStateComponent::ProcessIncomeIfNeeded()
{
    if (UWorld* World = GetWorld())
    {
        UWorldManagementSubsystem* WorldSubsystem = World->GetGameInstance()->GetSubsystem<UWorldManagementSubsystem>();
        if (!WorldSubsystem)
        {
            return;
        }

        float CurrentTime = WorldSubsystem->GetCurrentWorldTime();
    
        if ((CurrentTime - LastIncomeProcessTime) >= 24.0f)
        {
            ProcessDailyIncomeAndExpenses();
            LastIncomeProcessTime = CurrentTime;
        }
    }
}


int32 UDAPlayerStateComponent::GetTotalDailyIncome() const
{
    int32 TotalIncome = 0;
    for (const FOwnedPropertyDetails& Property : OwnedPropertyDetails)
    {
        //TotalIncome += CalculatePropertyIncome(Property);
    }
    for (const FBusinessDetails& Business : OwnedBusinessDetails)
    {
        //TotalIncome += CalculateBusinessIncome(Business);
    }
    return TotalIncome;
}

int32 UDAPlayerStateComponent::GetTotalDailyExpenses() const
{
    int32 TotalExpenses = 0;
    for (const FOwnedPropertyDetails& Property : OwnedPropertyDetails)
    {
        //TotalExpenses += CalculatePropertyMaintenance(Property);
    }
    for (const FBusinessDetails& Business : OwnedBusinessDetails)
    {
        //TotalExpenses += CalculateBusinessOperatingCost(Business);
    }
    return TotalExpenses;
}

TArray<FTransactionRecord> UDAPlayerStateComponent::GetRecentTransactions(int32 DaysBack) const
{
    TArray<FTransactionRecord> Recent;
    return Recent;
}

FOwnedPropertyDetails UDAPlayerStateComponent::GetPropertyInfo(const FGuid& PropertyID) const
{
    for (const FOwnedPropertyDetails& Property : OwnedPropertyDetails)
    {
        if (Property.PropertyID == PropertyID)
        {
            return Property;
        }
    }
    return FOwnedPropertyDetails();
}

FBusinessDetails UDAPlayerStateComponent::GetBusinessInfo(const FGuid& BusinessID) const
{
    for (const FBusinessDetails& Business : OwnedBusinessDetails)
    {
        if (Business.BusinessID == BusinessID)
        {
            return Business;
        }
    }
    return FBusinessDetails();
}

float UDAPlayerStateComponent::GetTotalBountyValue() const
{
    float TotalBounty = 0.0f;
    for (const auto& BountyPair : ReplicatedBountiesByRegion)
    {
        TotalBounty += BountyPair.Bounty;
    }
    return TotalBounty;
}

float UDAPlayerStateComponent::GetBountyInRegion(const FString& RegionID) const
{
    for (const auto& Pair : ReplicatedBountiesByRegion)
    {
        if (Pair.RegionID == RegionID)
        {
            return Pair.Bounty;
        }
    }
    return 0.0f;
}

bool UDAPlayerStateComponent::IsWantedInRegion(const FString& RegionID) const
{
    return GetBountyInRegion(RegionID) > 0.0f;
}

bool UDAPlayerStateComponent::HasAchievement(const FName& AchievementID) const
{
    for (const auto& Ach : PlayerAchievements)
    {
        if (Ach.AchievementID == AchievementID)
        {
            return Ach.bIsUnlocked;
        }
    }
    return false;
}

int32 UDAPlayerStateComponent::GetAchievementProgress(const FName& AchievementID) const
{
    for (const auto& Ach : PlayerAchievements)
    {
        if (Ach.AchievementID == AchievementID)
        {
            return Ach.CurrentProgress;
        }
    }
    return 0;
}

TArray<FPlayerAchievementProgress> UDAPlayerStateComponent::GetUnlockedAchievements() const
{
    TArray<FPlayerAchievementProgress> UnlockedList;
    for (const auto& Progress : PlayerAchievements)
    {
        if (Progress.bIsUnlocked)
        {
            UnlockedList.Add(Progress);
        }
    }
    return UnlockedList;
}

TArray<FPlayerAchievementProgress> UDAPlayerStateComponent::GetAchievementsInProgress() const
{
    TArray<FPlayerAchievementProgress> InProgressList;
    for (const auto& Progress : PlayerAchievements)
    {
        if (!Progress.bIsUnlocked && Progress.CurrentProgress > 0)
        {
            InProgressList.Add(Progress);
        }
    }
    return InProgressList;
}

float UDAPlayerStateComponent::GetAchievementCompletionPercentage() const
{
    return 0.0f;
}

void UDAPlayerStateComponent::AwardAchievementToPlayer(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID)
{
    if (!PlayerActor || AchievementID.IsNone())
    {
        return;
    }
    
    UDAPlayerStateComponent* PlayerState = PlayerActor->FindComponentByClass<UDAPlayerStateComponent>();
    if (PlayerState)
    {
        PlayerState->UnlockAchievement(AchievementID);
    }
}

void UDAPlayerStateComponent::UpdatePlayerAchievementProgress(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID, int32 ProgressIncrement)
{
    if (!PlayerActor || AchievementID.IsNone() || ProgressIncrement <= 0)
    {
        return;
    }
    
    UDAPlayerStateComponent* PlayerState = PlayerActor->FindComponentByClass<UDAPlayerStateComponent>();
    if (PlayerState)
    {
        PlayerState->UpdateAchievementProgress(AchievementID, ProgressIncrement);
    }
}

bool UDAPlayerStateComponent::DoesPlayerHaveAchievement(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID)
{
    if (!PlayerActor || AchievementID.IsNone())
    {
        return false;
    }
    
    UDAPlayerStateComponent* PlayerState = PlayerActor->FindComponentByClass<UDAPlayerStateComponent>();
    if (PlayerState)
    {
        return PlayerState->HasAchievement(AchievementID);
    }
    
    return false;
}

float UDAPlayerStateComponent::GetFactionStanding(const FName& FactionID) const
{
    for (const auto& Pair : ReplicatedFactionStandings)
    {
        if (Pair.FactionID == FactionID)
        {
            return Pair.Standing;
        }
    }
    return 0.0f;
}

TArray<FName> UDAPlayerStateComponent::GetFriendlyFactions(float MinStanding) const
{
    TArray<FName> FriendlyFactions;
    for (const auto& FactionPair : ReplicatedFactionStandings)
    {
        if (FactionPair.Standing >= MinStanding)
        {
            FriendlyFactions.Add(FactionPair.FactionID);
        }
    }
    return FriendlyFactions;
}

TArray<FName> UDAPlayerStateComponent::GetHostileFactions(float MaxStanding) const
{
    TArray<FName> HostileFactions;
    for (const auto& FactionPair : ReplicatedFactionStandings)
    {
        if (FactionPair.Standing <= MaxStanding)
        {
            HostileFactions.Add(FactionPair.FactionID);
        }
    }
    return HostileFactions;
}

bool UDAPlayerStateComponent::IsFactionFriendly(const FName& FactionID, float MinStanding) const
{
    return GetFactionStanding(FactionID) >= MinStanding;
}

bool UDAPlayerStateComponent::IsFactionHostile(const FName& FactionID, float MaxStanding) const
{
    return GetFactionStanding(FactionID) <= MaxStanding;
}

void UDAPlayerStateComponent::UpdateSurvivalState()
{
    ActiveConditions.Empty();
    
    if (StatlineComponent)
    {
        // Implement checks
    }
}

bool UDAPlayerStateComponent::IsInCriticalCondition() const
{
    return ActiveConditions.Contains(FName("CriticalHealth")) || 
           ActiveConditions.Contains(FName("Starving")) ||
           ActiveConditions.Contains(FName("Dehydrated"));
}

void UDAPlayerStateComponent::SavePlayerState()
{
    // Implementation
}

void UDAPlayerStateComponent::LoadPlayerState()
{
    // Implementation
}

TArray<FFamilyMember> UDAPlayerStateComponent::GetChildren() const
{
    TArray<FFamilyMember> Children;
    for (const FFamilyMember& Member : FamilyMembers)
    {
        if (Member.RelationshipType == TEXT("Child") || Member.RelationshipType == TEXT("Son") || Member.RelationshipType == TEXT("Daughter"))
        {
            Children.Add(Member);
        }
    }
    return Children;
}

bool UDAPlayerStateComponent::HasFamilyMember(const FString& CharacterID) const
{
    for (const FFamilyMember& Member : FamilyMembers)
    {
        if (Member.CharacterID == CharacterID)
        {
            return true;
        }
    }
    return false;
}

FFamilyMember UDAPlayerStateComponent::GetFamilyMember(const FString& CharacterID) const
{
    for (const FFamilyMember& Member : FamilyMembers)
    {
        if (Member.CharacterID == CharacterID)
        {
            return Member;
        }
    }
    return FFamilyMember();
}

EPlayerSocialStanding UDAPlayerStateComponent::CalculateSocialStanding() const
{
    float TotalBounty = GetTotalBountyValue();

    if (TotalBounty > 1000.0f)
    {
        return EPlayerSocialStanding::Outlaw;
    }

    if (GlobalReputation < -50.0f || TotalBounty > 100.0f)
    {
        return EPlayerSocialStanding::Vagrant;
    }

    int32 TotalPropertyValue = 0;
    int32 TotalBusinessValue = 0;

    for (const FOwnedPropertyDetails& Property : OwnedPropertyDetails)
    {
        TotalPropertyValue += Property.CurrentValue + (Property.UpgradeLevel * 1000);
    }

    for (const FBusinessDetails& Business : OwnedBusinessDetails)
    {
        //TotalBusinessValue += Business.InitialInvestment + (Business.BusinessLevel * 2000);
    }

    int32 TotalAssetValue = TotalPropertyValue + TotalBusinessValue;

    float FamilyBonus = 0.0f;
    if (IsMarried())
    {
        FamilyBonus += 5.0f;
    }
    if (GetChildren().Num() > 0)
    {
        FamilyBonus += 2.0f;
    }

    float EffectiveReputation = GlobalReputation + FamilyBonus;

    if ((PlayerWealth > 50000 || TotalAssetValue > 100000) && OwnedPropertyDetails.Num() > 5)
    {
        if (EffectiveReputation > 80.0f)
        {
            return EPlayerSocialStanding::Legend;
        }
        else if (EffectiveReputation > 50.0f)
        {
            return EPlayerSocialStanding::Hero;
        }
        return EPlayerSocialStanding::Noble;
    }

    if ((PlayerWealth > 10000 || TotalAssetValue > 20000) && (OwnedPropertyDetails.Num() > 0 || OwnedBusinessDetails.Num() > 0))
    {
        return EPlayerSocialStanding::Merchant;
    }

    if (PlayerWealth > 1000 || TotalAssetValue > 5000)
    {
        return EPlayerSocialStanding::Citizen;
    }

    if (EffectiveReputation > 20.0f)
    {
        return EPlayerSocialStanding::Citizen;
    }

    return EPlayerSocialStanding::Peasant;
}

void UDAPlayerStateComponent::ServerUnlockAchievement_Implementation(const FName& AchievementID)
{
    UnlockAchievement(AchievementID);
}

void UDAPlayerStateComponent::ServerUpdateAchievementProgress_Implementation(const FName& AchievementID, int32 ProgressIncrement)
{
    UpdateAchievementProgress(AchievementID, ProgressIncrement);
}

void UDAPlayerStateComponent::ServerSetAchievementProgress_Implementation(const FName& AchievementID, int32 NewProgress)
{
    SetAchievementProgress(AchievementID, NewProgress);
}

void UDAPlayerStateComponent::ServerModifyFactionStanding_Implementation(const FName& FactionID, float Amount, const FString& Reason)
{
    if (!FactionID.IsNone())
    {
        float* Standing = nullptr;
        for (auto& Pair : ReplicatedFactionStandings)
        {
            if (Pair.FactionID == FactionID)
            {
                Standing = &Pair.Standing;
                break;
            }
        }

        if (!Standing)
        {
            ReplicatedFactionStandings.Add(FFactionStandingPair(FactionID, 0.0f));
            Standing = &ReplicatedFactionStandings.Last().Standing;
        }
        
        *Standing += Amount;
        *Standing = FMath::Clamp(*Standing, -100.0f, 100.0f);
        OnPlayerStateChanged.Broadcast(FName(*FString::Printf(TEXT("Faction_%s"), *FactionID.ToString())), *Standing);
    }
}

void UDAPlayerStateComponent::ServerModifyGlobalReputation_Implementation(float Amount, const FString& Reason)
{
    GlobalReputation += Amount;
    OnPlayerStateChanged.Broadcast(FName("GlobalReputation"), GlobalReputation);
    UpdateSocialStanding();
}

void UDAPlayerStateComponent::ServerModifyRegionalReputation_Implementation(const FString& RegionID, float Amount, const FString& Reason)
{
    if (!RegionID.IsEmpty())
    {
        float* RegionReputation = nullptr;
        for (auto& Pair : ReplicatedRegionalReputations)
        {
            if (Pair.RegionID == RegionID)
            {
                RegionReputation = &Pair.Reputation;
                break;
            }
        }

        if (!RegionReputation)
        {
            ReplicatedRegionalReputations.Add(FRegionReputationPair(RegionID, 0.0f));
            RegionReputation = &ReplicatedRegionalReputations.Last().Reputation;
        }
        
        *RegionReputation += Amount;
    }
}

void UDAPlayerStateComponent::ServerSpendMoney_Implementation(int32 Amount, const FString& Reason)
{
    if (Amount > 0 && PlayerWealth >= Amount)
    {
        PlayerWealth -= Amount;
        OnPlayerStateChanged.Broadcast(FName("PlayerWealth"), (float)PlayerWealth);
    }
}

void UDAPlayerStateComponent::ServerAddMoney_Implementation(int32 Amount, const FString& Reason)
{
    if (Amount > 0)
    {
        PlayerWealth += Amount;
        OnPlayerStateChanged.Broadcast(FName("PlayerWealth"), (float)PlayerWealth);
    }
}

void UDAPlayerStateComponent::ServerUpdateSocialStanding_Implementation()
{
    UpdateSocialStanding();
}

void UDAPlayerStateComponent::ServerAcquireProperty_Implementation(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice)
{
    if (PlayerWealth >= PurchasePrice)
    {
        PlayerWealth -= PurchasePrice;
        OwnedPropertyDetails.Add(PropertyDetails);
        OnPropertyIncomeGenerated.Broadcast(PropertyDetails.PropertyID, 0, PropertyDetails.RegionID);
        UpdateSocialStanding();
    }
}

void UDAPlayerStateComponent::ServerStartBusiness_Implementation(const FBusinessDetails& BusinessDetails, int32 InitialInvestment)
{
    if (PlayerWealth >= InitialInvestment)
    {
        PlayerWealth -= InitialInvestment;
        OwnedBusinessDetails.Add(BusinessDetails);
        OnBusinessIncomeGenerated.Broadcast(BusinessDetails.BusinessID, 0, BusinessDetails.RegionID);
        UpdateSocialStanding();
    }
}

void UDAPlayerStateComponent::ServerLoseProperty_Implementation(const FGuid& PropertyID, const FString& Reason)
{
    const int32 Index = OwnedPropertyDetails.IndexOfByPredicate([&PropertyID](const FOwnedPropertyDetails& Detail) {
        return Detail.PropertyID == PropertyID;
    });
    if (Index != INDEX_NONE)
    {
        OnPropertyLost.Broadcast(PropertyID, Reason, OwnedPropertyDetails[Index].RegionID);
        OwnedPropertyDetails.RemoveAt(Index);
        UpdateSocialStanding();
    }
}

void UDAPlayerStateComponent::ServerLoseBusiness_Implementation(const FGuid& BusinessID, const FString& Reason)
{
    const int32 Index = OwnedBusinessDetails.IndexOfByPredicate([&BusinessID](const FBusinessDetails& Detail) {
        return Detail.BusinessID == BusinessID;
    });
    if (Index != INDEX_NONE)
    {
        OnBusinessLost.Broadcast(BusinessID, Reason, OwnedBusinessDetails[Index].RegionID);
        OwnedBusinessDetails.RemoveAt(Index);
        UpdateSocialStanding();
    }
}

void UDAPlayerStateComponent::ServerUpgradeProperty_Implementation(const FGuid& PropertyID, int32 UpgradeCost)
{
    FOwnedPropertyDetails* Property = OwnedPropertyDetails.FindByPredicate([&PropertyID](const FOwnedPropertyDetails& Detail) {
        return Detail.PropertyID == PropertyID;
    });
    if (Property && PlayerWealth >= UpgradeCost)
    {
        PlayerWealth -= UpgradeCost;
        Property->UpgradeLevel++;
        Property->CurrentValue += UpgradeCost; // Or some other logic
        // Broadcast an upgrade event if one exists
    }
}

void UDAPlayerStateComponent::ServerUpgradeBusiness_Implementation(const FGuid& BusinessID, int32 UpgradeCost)
{
    FBusinessDetails* Business = OwnedBusinessDetails.FindByPredicate([&BusinessID](const FBusinessDetails& Detail) {
        return Detail.BusinessID == BusinessID;
    });
    if (Business && PlayerWealth >= UpgradeCost)
    {
        PlayerWealth -= UpgradeCost;
        Business->BusinessLevel++;
        // Broadcast an upgrade event if one exists
    }
}

void UDAPlayerStateComponent::OnRep_CurrentSocialStanding()
{
    OnPlayerStateChanged.Broadcast(FName("SocialStanding"), (float)CurrentSocialStanding);
}

void UDAPlayerStateComponent::OnRep_GlobalReputation()
{
    OnPlayerStateChanged.Broadcast(FName("GlobalReputation"), GlobalReputation);
}

void UDAPlayerStateComponent::OnRep_PlayerWealth()
{
    OnPlayerStateChanged.Broadcast(FName("PlayerWealth"), (float)PlayerWealth);
}

void UDAPlayerStateComponent::OnRep_PlayerQuests()
{
    OnPlayerStateChanged.Broadcast(FName("PersonalQuests"), (float)PlayerQuests.Num());
}

void UDAPlayerStateComponent::OnRep_UnlockedStoryMilestones()
{
    OnPlayerStateChanged.Broadcast(FName("StoryMilestones"), (float)UnlockedStoryMilestones.Num());
}

TArray<FPlayerQuestData> UDAPlayerStateComponent::GetActiveQuests() const
{
    TArray<FPlayerQuestData> ActiveQuests;
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.CurrentState == EQuestState::QS_InProgress)
        {
            ActiveQuests.Add(Quest);
        }
    }
    return ActiveQuests;
}

TArray<FPlayerQuestData> UDAPlayerStateComponent::GetCompletedQuests() const
{
    TArray<FPlayerQuestData> CompletedQuests;
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.CurrentState == EQuestState::QS_Completed)
        {
            CompletedQuests.Add(Quest);
        }
    }
    return CompletedQuests;
}

TArray<FPlayerQuestData> UDAPlayerStateComponent::GetTrackedQuests() const
{
    TArray<FPlayerQuestData> TrackedQuests;
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.bIsTracked && Quest.CurrentState == EQuestState::QS_InProgress)
        {
            TrackedQuests.Add(Quest);
        }
    }
    return TrackedQuests;
}

FPlayerQuestData UDAPlayerStateComponent::GetQuestData(FName QuestID) const
{
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return Quest;
        }
    }
    return FPlayerQuestData();
}

void UDAPlayerStateComponent::SetQuestTracked(FName QuestID, bool bTracked)
{
    for (auto& QuestData : PlayerQuests)
    {
        if (QuestData.QuestID == QuestID)
        {
            QuestData.bIsTracked = bTracked;
            OnPlayerQuestProgressUpdated.Broadcast(QuestID);
            return;
        }
    }
}
 
bool UDAPlayerStateComponent::IsQuestTracked(FName QuestID) const
{
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return Quest.bIsTracked;
        }
    }
    return false;
}

int32 UDAPlayerStateComponent::GetActiveQuestCount() const
{
    int32 Count = 0;
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.CurrentState == EQuestState::QS_InProgress)
        {
            Count++;
        }
    }
    return Count;
}

int32 UDAPlayerStateComponent::GetCompletedQuestCount() const
{
    int32 Count = 0;
    for (const auto& Quest : PlayerQuests)
    {
        if (Quest.CurrentState == EQuestState::QS_Completed)
        {
            Count++;
        }
    }
    return Count;
}

TArray<FStoryMilestone> UDAPlayerStateComponent::GetUnlockedMilestones() const
{
    TArray<FStoryMilestone> Unlocked;
    for (const auto& Milestone : UnlockedStoryMilestones)
    {
        if (Milestone.bIsUnlocked)
        {
            Unlocked.Add(Milestone);
        }
    }
    return Unlocked;
}

bool UDAPlayerStateComponent::HasUnlockedMilestone(FName MilestoneID) const
{
    for (const auto& Milestone : UnlockedStoryMilestones)
    {
        if (Milestone.MilestoneID == MilestoneID)
        {
            return Milestone.bIsUnlocked;
        }
    }
    return false;
}

void UDAPlayerStateComponent::UnlockStoryMilestone(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID)
{
    if (!MilestoneID.IsNone())
    {
        FStoryMilestone* Milestone = nullptr;
        for (auto& M : UnlockedStoryMilestones)
        {
            if (M.MilestoneID == MilestoneID)
            {
                Milestone = &M;
                break;
            }
        }

        if (!Milestone)
        {
            UnlockedStoryMilestones.Add(FStoryMilestone());
            Milestone = &UnlockedStoryMilestones.Last();
            Milestone->MilestoneID = MilestoneID;
        }

        if (!Milestone->bIsUnlocked)
        {
            Milestone->MilestoneName = MilestoneName;
            Milestone->Description = Description;
            Milestone->RegionID = RegionID;
            Milestone->bIsUnlocked = true;
            Milestone->UnlockTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
            OnPlayerStateChanged.Broadcast(FName("StoryMilestone"), 1.0f);
        }
    }
}

void UDAPlayerStateComponent::OnQuestStarted(FName QuestID, bool bIsPersonalStory)
{
    if (!QuestID.IsNone())
    {
        FPlayerQuestData* QuestData = nullptr;
        for (auto& Quest : PlayerQuests)
        {
            if (Quest.QuestID == QuestID)
            {
                QuestData = &Quest;
                break;
            }
        }

        if (!QuestData)
        {
            PlayerQuests.Add(FPlayerQuestData());
            QuestData = &PlayerQuests.Last();
            QuestData->QuestID = QuestID;
        }

        QuestData->CurrentState = EQuestState::QS_InProgress;
        QuestData->bIsPersonalStory = bIsPersonalStory;
        QuestData->ProgressPercentage = 0.0f;
        QuestData->CompletionTime = 0.0f;
        
        if (bIsPersonalStory && QuestManagementSubsystem)
        {
            // Auto-track
        }
        
        OnPlayerQuestStateChanged.Broadcast(QuestID, EQuestState::QS_InProgress);
        OnPlayerQuestProgressUpdated.Broadcast(QuestID);
    }
}

void UDAPlayerStateComponent::OnQuestCompleted(FName QuestID)
{
    for (auto& QuestData : PlayerQuests)
    {
        if (QuestData.QuestID == QuestID)
        {
            QuestData.CurrentState = EQuestState::QS_Completed;
            QuestData.ProgressPercentage = 1.0f;
            QuestData.CompletionTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
            QuestData.bIsTracked = false;
            
            OnPlayerQuestStateChanged.Broadcast(QuestID, EQuestState::QS_Completed);
            OnPlayerQuestProgressUpdated.Broadcast(QuestID);
            
            if (QuestData.bIsPersonalStory)
            {
                FString MilestoneDescription = FString::Printf(TEXT("Completed personal story quest: %s"), *QuestID.ToString());
                UnlockStoryMilestone(
                    FName(*FString::Printf(TEXT("Quest_%s"), *QuestID.ToString())),
                    FText::FromString(FString::Printf(TEXT("Quest Completed: %s"), *QuestID.ToString())),
                    FText::FromString(MilestoneDescription),
                    TEXT("")
                );
            }
            return;
        }
    }
}

void UDAPlayerStateComponent::OnQuestFailed(FName QuestID)
{
    for (auto& QuestData : PlayerQuests)
    {
        if (QuestData.QuestID == QuestID)
        {
            QuestData.CurrentState = EQuestState::QS_Failed;
            QuestData.bIsTracked = false;
            
            OnPlayerQuestStateChanged.Broadcast(QuestID, EQuestState::QS_Failed);
            OnPlayerQuestProgressUpdated.Broadcast(QuestID);
            return;
        }
    }
}

void UDAPlayerStateComponent::OnQuestAbandoned(FName QuestID)
{
    for (auto& QuestData : PlayerQuests)
    {
        if (QuestData.QuestID == QuestID)
        {
            QuestData.CurrentState = EQuestState::QS_Abandoned;
            QuestData.bIsTracked = false;
            
            OnPlayerQuestStateChanged.Broadcast(QuestID, EQuestState::QS_Abandoned);
            OnPlayerQuestProgressUpdated.Broadcast(QuestID);
            return;
        }
    }
}

void UDAPlayerStateComponent::OnQuestProgressUpdated(FName QuestID, float NewProgressPercentage)
{
    for (auto& QuestData : PlayerQuests)
    {
        if (QuestData.QuestID == QuestID)
        {
            QuestData.ProgressPercentage = FMath::Clamp(NewProgressPercentage, 0.0f, 1.0f);
            OnPlayerQuestProgressUpdated.Broadcast(QuestID);
            return;
        }
    }
}

void UDAPlayerStateComponent::OnRep_PlayerAchievements()
{
    OnPlayerStateChanged.Broadcast(FName("AchievementProgress"), 1.0f);
}

void UDAPlayerStateComponent::OnRep_SpouseInfo()
{
    OnPlayerStateChanged.Broadcast(FName("MaritalStatus"), IsMarried() ? 1.0f : 0.0f);
    
    if (IsMarried())
    {
        OnPlayerMarried.Broadcast(SpouseID, SpouseName);
    }
}
      
void UDAPlayerStateComponent::OnRep_FactionStandings()
{
    for (const auto& FactionPair : ReplicatedFactionStandings)
    {
        OnPlayerStateChanged.Broadcast(FName(*FString::Printf(TEXT("Faction_%s"), *FactionPair.FactionID.ToString())), FactionPair.Standing);
    }
}

void UDAPlayerStateComponent::OnRep_OwnedPropertyDetails()
{
    OnPlayerStateChanged.Broadcast(FName("OwnedProperties"), (float)OwnedPropertyDetails.Num());
}

void UDAPlayerStateComponent::OnRep_OwnedBusinessDetails()
{
    OnPlayerStateChanged.Broadcast(FName("OwnedBusinesses"), (float)OwnedBusinessDetails.Num());
}

void UDAPlayerStateComponent::OnRep_RecentTransactions()
{
    OnPlayerStateChanged.Broadcast(FName("RecentTransactions"), (float)RecentTransactions.Num());
}

void UDAPlayerStateComponent::OnRep_RegionalReputations()
{
    OnPlayerStateChanged.Broadcast(FName("RegionalReputations"), 1.f);
}

void UDAPlayerStateComponent::OnRep_BountiesByRegion()
{
    OnPlayerStateChanged.Broadcast(FName("Bounties"), GetTotalBountyValue());
}

void UDAPlayerStateComponent::OnRep_PersonalHistory()
{
    OnPlayerStateChanged.Broadcast(FName("PersonalHistory"), (float)PersonalHistory.Num());
}

void UDAPlayerStateComponent::OnRep_FamilyMembers()
{
    OnPlayerStateChanged.Broadcast(FName("Family"), (float)FamilyMembers.Num());
}

void UDAPlayerStateComponent::OnRep_ActiveConditions()
{
    OnPlayerStateChanged.Broadcast(FName("SurvivalConditions"), (float)ActiveConditions.Num());
}

bool UDAPlayerStateComponent::ServerUpdateSocialStanding_Validate() { return true; }
bool UDAPlayerStateComponent::ServerModifyGlobalReputation_Validate(float Amount, const FString& Reason) { return FMath::Abs(Amount) <= 100.f && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerModifyRegionalReputation_Validate(const FString& RegionID, float Amount, const FString& Reason) { return !RegionID.IsEmpty() && FMath::Abs(Amount) <= 100.f && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerSpendMoney_Validate(int32 Amount, const FString& Reason) { return Amount > 0 && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerAddMoney_Validate(int32 Amount, const FString& Reason) { return Amount > 0 && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerAcquireProperty_Validate(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice) { return PropertyDetails.PropertyID.IsValid() && PurchasePrice > 0; }
bool UDAPlayerStateComponent::ServerStartBusiness_Validate(const FBusinessDetails& BusinessDetails, int32 InitialInvestment) { return BusinessDetails.BusinessID.IsValid() && InitialInvestment > 0; }
bool UDAPlayerStateComponent::ServerLoseProperty_Validate(const FGuid& PropertyID, const FString& Reason) { return PropertyID.IsValid() && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerLoseBusiness_Validate(const FGuid& BusinessID, const FString& Reason) { return BusinessID.IsValid() && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerUpgradeProperty_Validate(const FGuid& PropertyID, int32 UpgradeCost) { return PropertyID.IsValid() && UpgradeCost > 0; }
bool UDAPlayerStateComponent::ServerUpgradeBusiness_Validate(const FGuid& BusinessID, int32 UpgradeCost) { return BusinessID.IsValid() && UpgradeCost > 0; }
bool UDAPlayerStateComponent::ServerAddLegacyProperty_Validate(const FString& PropertyID) { return !PropertyID.IsEmpty(); }
void UDAPlayerStateComponent::ServerAddLegacyProperty_Implementation(const FString& PropertyID) { AddProperty(PropertyID); }
bool UDAPlayerStateComponent::ServerAddBounty_Validate(const FString& RegionID, float Amount, const FString& Reason) { return !RegionID.IsEmpty() && Amount > 0 && !Reason.IsEmpty(); }
void UDAPlayerStateComponent::ServerAddBounty_Implementation(const FString& RegionID, float Amount, const FString& Reason) { AddBounty(RegionID, Amount, Reason); }
bool UDAPlayerStateComponent::ServerClearBounty_Validate(const FString& RegionID, float AmountPaid) { return !RegionID.IsEmpty() && AmountPaid > 0; }
void UDAPlayerStateComponent::ServerClearBounty_Implementation(const FString& RegionID, float AmountPaid) { ClearBounty(RegionID, AmountPaid); }
bool UDAPlayerStateComponent::ServerRecordMajorEvent_Validate(const FString& EventDescription, const FString& RegionID) { return !EventDescription.IsEmpty(); }
bool UDAPlayerStateComponent::ServerUnlockAchievement_Validate(const FName& AchievementID) { return !AchievementID.IsNone(); }
bool UDAPlayerStateComponent::ServerUpdateAchievementProgress_Validate(const FName& AchievementID, int32 ProgressIncrement) { return !AchievementID.IsNone() && ProgressIncrement != 0; }
bool UDAPlayerStateComponent::ServerSetAchievementProgress_Validate(const FName& AchievementID, int32 NewProgress) { return !AchievementID.IsNone() && NewProgress >= 0; }
bool UDAPlayerStateComponent::ServerSetQuestTracked_Validate(FName QuestID, bool bTracked) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerSetQuestTracked_Implementation(FName QuestID, bool bTracked) { SetQuestTracked(QuestID, bTracked); }
bool UDAPlayerStateComponent::ServerUnlockStoryMilestone_Validate(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID) { return !MilestoneID.IsNone(); }
void UDAPlayerStateComponent::ServerUnlockStoryMilestone_Implementation(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID) { UnlockStoryMilestone(MilestoneID, MilestoneName, Description, RegionID); }
bool UDAPlayerStateComponent::ServerOnQuestStarted_Validate(FName QuestID, bool bIsPersonalStory) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerOnQuestStarted_Implementation(FName QuestID, bool bIsPersonalStory) { OnQuestStarted(QuestID, bIsPersonalStory); }
bool UDAPlayerStateComponent::ServerOnQuestCompleted_Validate(FName QuestID) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerOnQuestCompleted_Implementation(FName QuestID) { OnQuestCompleted(QuestID); }
bool UDAPlayerStateComponent::ServerOnQuestFailed_Validate(FName QuestID) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerOnQuestFailed_Implementation(FName QuestID) { OnQuestFailed(QuestID); }
bool UDAPlayerStateComponent::ServerOnQuestAbandoned_Validate(FName QuestID) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerOnQuestAbandoned_Implementation(FName QuestID) { OnQuestAbandoned(QuestID); }
bool UDAPlayerStateComponent::ServerOnQuestProgressUpdated_Validate(FName QuestID, float NewProgressPercentage) { return !QuestID.IsNone(); }
void UDAPlayerStateComponent::ServerOnQuestProgressUpdated_Implementation(FName QuestID, float NewProgressPercentage) { OnQuestProgressUpdated(QuestID, NewProgressPercentage); }
// Removed duplicate definition of ServerModifyFactionStanding_Validate
bool UDAPlayerStateComponent::ServerMarryCharacter_Validate(const FString& CharacterID, const FString& CharacterName) { return !CharacterID.IsEmpty() && !CharacterName.IsEmpty(); }
bool UDAPlayerStateComponent::ServerDivorceSpouse_Validate(const FString& Reason) { return !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerAddChild_Validate(const FString& ChildID, const FString& ChildName, int32 Age) { return !ChildID.IsEmpty() && !ChildName.IsEmpty() && Age >= 0; }
bool UDAPlayerStateComponent::ServerAddFamilyMember_Validate(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age) { return !CharacterID.IsEmpty() && !CharacterName.IsEmpty() && !RelationshipType.IsEmpty() && Age >= 0; }
bool UDAPlayerStateComponent::ServerRemoveFamilyMember_Validate(const FString& CharacterID, const FString& Reason) { return !CharacterID.IsEmpty() && !Reason.IsEmpty(); }
bool UDAPlayerStateComponent::ServerUpdateFamilyMemberStatus_Validate(const FString& CharacterID, bool bIsAlive, int32 NewAge) { return !CharacterID.IsEmpty() && NewAge >= 0; }

void UDAPlayerStateComponent::ServerUpdateSurvivalState_Implementation()
{
    UpdateSurvivalState();
}

void UDAPlayerStateComponent::AcquireProperty(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice)
{
    if (GetOwner()->HasAuthority())
    {
        ServerAcquireProperty_Implementation(PropertyDetails, PurchasePrice);
    }
    else
    {
        ServerAcquireProperty(PropertyDetails, PurchasePrice);
    }
}

void UDAPlayerStateComponent::StartBusiness(const FBusinessDetails& BusinessDetails, int32 InitialInvestment)
{
    if (GetOwner()->HasAuthority())
    {
        ServerStartBusiness_Implementation(BusinessDetails, InitialInvestment);
    }
    else
    {
        ServerStartBusiness(BusinessDetails, InitialInvestment);
    }
}

void UDAPlayerStateComponent::LoseProperty(const FGuid& PropertyID, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        ServerLoseProperty_Implementation(PropertyID, Reason);
    }
    else
    {
        ServerLoseProperty(PropertyID, Reason);
    }
}

void UDAPlayerStateComponent::LoseBusiness(const FGuid& BusinessID, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        ServerLoseBusiness_Implementation(BusinessID, Reason);
    }
    else
    {
        ServerLoseBusiness(BusinessID, Reason);
    }
}

void UDAPlayerStateComponent::UpgradeProperty(const FGuid& PropertyID, int32 UpgradeCost)
{
    if (GetOwner()->HasAuthority())
    {
        ServerUpgradeProperty_Implementation(PropertyID, UpgradeCost);
    }
    else
    {
        ServerUpgradeProperty(PropertyID, UpgradeCost);
    }
}

void UDAPlayerStateComponent::UpgradeBusiness(const FGuid& BusinessID, int32 UpgradeCost)
{
    if (GetOwner()->HasAuthority())
    {
        ServerUpgradeBusiness_Implementation(BusinessID, UpgradeCost);
    }
    else
    {
        ServerUpgradeBusiness(BusinessID, UpgradeCost);
    }
}

void UDAPlayerStateComponent::AddMoney(int32 Amount, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        ServerAddMoney_Implementation(Amount, Reason);
    }
    else
    {
        ServerAddMoney(Amount, Reason);
    }
}

void UDAPlayerStateComponent::ModifyFactionStanding(const FName& FactionID, float Amount, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        ServerModifyFactionStanding_Implementation(FactionID, Amount, Reason);
    }
    else
    {
        ServerModifyFactionStanding(FactionID, Amount, Reason);
    }
}

void UDAPlayerStateComponent::SpendMoney(int32 Amount, const FString& Reason)
{
    if (GetOwner()->HasAuthority())
    {
        ServerSpendMoney_Implementation(Amount, Reason);
    }
    else
    {
        ServerSpendMoney(Amount, Reason);
    }
}

void UDAPlayerStateComponent::ServerRecordMajorEvent_Implementation(const FString& EventDescription, const FString& RegionID)
{
    if (!EventDescription.IsEmpty())
    {
        if (UWorld* World = GetWorld())
        {
            FString TimestampedEvent = FString::Printf(TEXT("Day %d: %s [%s]"), static_cast<int32>(World->GetTimeSeconds() / 86400), *EventDescription, *RegionID);
            PersonalHistory.Add(TimestampedEvent);
        }
    }
}

void UDAPlayerStateComponent::ServerMarryCharacter_Implementation(const FString& CharacterID, const FString& CharacterName)
{
    if (!IsMarried())
    {
        SpouseID = CharacterID;
        SpouseName = CharacterName;
        OnPlayerMarried.Broadcast(CharacterID, CharacterName);
    }
}

void UDAPlayerStateComponent::ServerDivorceSpouse_Implementation(const FString& Reason)
{
    if (IsMarried())
    {
        SpouseID.Empty();
        SpouseName.Empty();
        OnPlayerDivorced.Broadcast(Reason);
    }
}

void UDAPlayerStateComponent::ServerAddChild_Implementation(const FString& ChildID, const FString& ChildName, int32 Age)
{
    ServerAddFamilyMember_Implementation(ChildID, ChildName, TEXT("Child"), Age);
}

void UDAPlayerStateComponent::ServerAddFamilyMember_Implementation(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age)
{
    if (!CharacterID.IsEmpty() && !CharacterName.IsEmpty() && !HasFamilyMember(CharacterID))
    {
        FFamilyMember NewMember;
        NewMember.CharacterID = CharacterID;
        NewMember.CharacterName = CharacterName;
        NewMember.RelationshipType = RelationshipType;
        NewMember.Age = Age;
        NewMember.IsAlive = true;
        FamilyMembers.Add(NewMember);
        OnFamilyMemberAdded.Broadcast(CharacterID, CharacterName, RelationshipType);
    }
}

void UDAPlayerStateComponent::ServerRemoveFamilyMember_Implementation(const FString& CharacterID, const FString& Reason)
{
    if (!CharacterID.IsEmpty())
    {
        const int32 MemberIndex = FamilyMembers.IndexOfByPredicate([&CharacterID](const FFamilyMember& Member) {
            return Member.CharacterID == CharacterID;
        });

        if (MemberIndex != INDEX_NONE)
        {
            FamilyMembers.RemoveAt(MemberIndex);
            OnFamilyMemberRemoved.Broadcast(CharacterID, Reason);
        }
    }
}

void UDAPlayerStateComponent::ServerUpdateFamilyMemberStatus_Implementation(const FString& CharacterID, bool bIsAlive, int32 NewAge)
{
    FFamilyMember* Member = FamilyMembers.FindByPredicate([&CharacterID](const FFamilyMember& FamMember) {
        return FamMember.CharacterID == CharacterID;
    });

    if (Member)
    {
        Member->IsAlive = bIsAlive;
        Member->Age = NewAge;
        OnFamilyMemberStatusChanged.Broadcast(CharacterID, bIsAlive);
    }
}

bool UDAPlayerStateComponent::ServerModifyFactionStanding_Validate(const FName& FactionID, float Amount, const FString& Reason)
{
    return !FactionID.IsNone() && FMath::Abs(Amount) <= 100.f && !Reason.IsEmpty();
}
