
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/WorldStateData.h"
#include "Data/PropertyData.h"  // Include for FOwnedPropertyDetails, FBusinessDetails, EDGPropertyType
#include "Data/QuestData.h"     // Include for EQuestState
#include "Data/AchievementData.h" // Include for FPlayerAchievementProgress, FOnAchievementDelegates
#include "Net/UnrealNetwork.h"
#include "DAPlayerStateComponent.generated.h"

// Forward declarations
// Forward declarations
class UStatlineComponent;
class UNotorietyComponent;
class UQuestManagementSubsystem;

/**
 * Delegate for property income events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPropertyIncomeGenerated, const FGuid&, PropertyID, int32, Amount, const FString&, RegionID);

/**
 * Delegate for business income events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBusinessIncomeGenerated, const FGuid&, BusinessID, int32, Amount, const FString&, RegionID);

/**
 * Delegate for property loss events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPropertyLost, const FGuid&, PropertyID, const FString&, Reason, const FString&, RegionID);

/**
 * Delegate for business loss events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBusinessLost, const FGuid&, BusinessID, const FString&, Reason, const FString&, RegionID);

/**
 * Delegate for player state change events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerStateChanged, FName, StateName, float, NewValue);

/**
 * Delegate for personal quest events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerQuestStateChanged, FName, QuestID, EQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerQuestProgressUpdated, FName, QuestID);

/**
 * Player Quest Data - simplified quest tracking for UI and personal progression
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FPlayerQuestData // This struct is specific to PlayerState, so it's fine here.
{
    GENERATED_BODY()

    FPlayerQuestData()
        : QuestID()
        , CurrentState(EQuestState::QS_NotStarted)
        , bIsTracked(false)
        , bIsPersonalStory(false)
        , ProgressPercentage(0.0f)
        , CompletionTime(0.0f)
    {
    }

    // Quest identifier
    UPROPERTY(BlueprintReadOnly, Category = "Player Quest")
    FName QuestID;

    // Current quest state for this player
    UPROPERTY(BlueprintReadOnly, Category = "Player Quest")
    EQuestState CurrentState;

    // Whether this quest is currently tracked in UI
    UPROPERTY(BlueprintReadWrite, Category = "Player Quest")
    bool bIsTracked;

    // Whether this is part of personal story progression
    UPROPERTY(BlueprintReadOnly, Category = "Player Quest")
    bool bIsPersonalStory;

    // Cached progress percentage for UI
    UPROPERTY(BlueprintReadOnly, Category = "Player Quest")
    float ProgressPercentage;

    // Time when quest was completed (0 if not completed)
    UPROPERTY(BlueprintReadOnly, Category = "Player Quest")
    float CompletionTime;
};

/**
 * Story Milestone tracking for personal progression
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FStoryMilestone // This struct is specific to PlayerState, so it's fine here.
{
    GENERATED_BODY()

    FStoryMilestone()
        : MilestoneID()
        , MilestoneName()
        , Description()
        , UnlockTime(0.0f)
        , RegionID()
        , bIsUnlocked(false)
    {
    }

    // Unique milestone identifier
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    FName MilestoneID;

    // Display name for milestone
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    FText MilestoneName;

    // Description of what was achieved
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    FText Description;

    // Time when milestone was unlocked
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    float UnlockTime;

    // Region where milestone was achieved
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    FString RegionID;

    // Whether this milestone has been unlocked
    UPROPERTY(BlueprintReadOnly, Category = "Story Milestone")
    bool bIsUnlocked;
};

/**
 * Delegate for family-related events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerMarried, FString, SpouseID, FString, SpouseName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDivorced, FString, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFamilyMemberAdded, FString, CharacterID, FString, CharacterName, FString, RelationshipType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFamilyMemberRemoved, FString, CharacterID, FString, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFamilyMemberStatusChanged, FString, CharacterID, bool, IsAlive);

/**
 * Enum for different player social standings
 */
UENUM(BlueprintType)
enum class EPlayerSocialStanding : uint8 // This enum is specific to PlayerState, so it's fine here.
{
    Outlaw        UMETA(DisplayName = "Outlaw"),
    Vagrant       UMETA(DisplayName = "Vagrant"),
    Peasant       UMETA(DisplayName = "Peasant"),
    Citizen       UMETA(DisplayName = "Citizen"),
    Merchant      UMETA(DisplayName = "Merchant"),
    Noble         UMETA(DisplayName = "Noble"),
    Hero          UMETA(DisplayName = "Hero"),
    Legend        UMETA(DisplayName = "Legend")
};

/**
 * Family Relationship Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FFamilyMember // This struct is specific to PlayerState, so it's fine here.
{
    GENERATED_BODY()

    FFamilyMember()
        : CharacterID()
        , CharacterName()
        , RelationshipType(TEXT("Unknown"))
        , IsAlive(true)
        , Age(0)
        , LastSeenTime(0.0f)
    {
    }

    // Unique identifier for the family member
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Family")
    FString CharacterID;

    // Display name of the family member
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Family")
    FString CharacterName;

    // Type of family relationship (spouse, child, parent, sibling, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Family")
    FString RelationshipType;

    // Whether this family member is still alive
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Family")
    bool IsAlive;

    // Age of the family member
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Family")
    int32 Age;

    // Last time player interacted with this family member
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Family")
    float LastSeenTime;
};

/**
 * Structure to hold transaction records for player economy.
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FTransactionRecord // This struct is specific to PlayerState, so it's fine here.
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    int32 Amount;

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    bool bIsIncome; // True for income, false for expense

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    FString RegionID; // Region where transaction occurred

    UPROPERTY(BlueprintReadOnly, Category = "Transaction")
    FString ItemOrServiceID; // ID of item or service related to transaction

    FTransactionRecord()
        : Timestamp(FDateTime::MinValue())
        , Description(TEXT(""))
        , Amount(0)
        , bIsIncome(false)
        , RegionID(TEXT(""))
        , ItemOrServiceID(TEXT(""))
    {
    }
};

/** Helper struct for replicating RegionalReputations TMap */
USTRUCT(BlueprintType)
struct FRegionReputationPair // Specific to PlayerState replication
{
    GENERATED_BODY()

    UPROPERTY()
    FString RegionID;

    UPROPERTY()
    float Reputation;

    FRegionReputationPair() : RegionID(TEXT("")), Reputation(0.0f) {}
    FRegionReputationPair(const FString& InRegionID, float InReputation) : RegionID(InRegionID), Reputation(InReputation) {}
};

/** Helper struct for replicating BountiesByRegion TMap */
USTRUCT(BlueprintType)
struct FRegionBountyPair // Specific to PlayerState replication
{
    GENERATED_BODY()

    UPROPERTY()
    FString RegionID;

    UPROPERTY()
    float Bounty;

    FRegionBountyPair() : RegionID(TEXT("")), Bounty(0.0f) {}
    FRegionBountyPair(const FString& InRegionID, float InBounty) : RegionID(InRegionID), Bounty(InBounty) {}
};

/** Helper struct for replicating FactionStandings TMap */
USTRUCT(BlueprintType)
struct FFactionStandingPair // Specific to PlayerState replication
{
    GENERATED_BODY()

    UPROPERTY()
    FName FactionID;

    UPROPERTY()
    float Standing;

    FFactionStandingPair() : FactionID(NAME_None), Standing(0.0f) {}
    FFactionStandingPair(const FName& InFactionID, float InStanding) : FactionID(InFactionID), Standing(InStanding) {}
};


/**
 * Player State Component
 *
 * Tracks comprehensive player state beyond basic stats including social standing,
 * region relationships, personal history, and long-term progression.
 * Integrates with the World Management system for persistent state tracking.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UDAPlayerStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDAPlayerStateComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Cached component references
    UPROPERTY()
    UStatlineComponent* StatlineComponent;

    UPROPERTY()
    UNotorietyComponent* NotorietyComponent;

    UPROPERTY()
    UQuestManagementSubsystem* QuestManagementSubsystem;

    // Income processing
    float LastIncomeProcessTime;

    // Member variables that were using the incorrect types
    UPROPERTY(ReplicatedUsing = OnRep_OwnedPropertyDetails, BlueprintReadOnly, Category = "Player State|Property")
    TArray<FOwnedPropertyDetails> OwnedPropertyDetails; // FOwnedPropertyDetails is defined in PropertyData.h
    UFUNCTION()
    void OnRep_OwnedPropertyDetails();

    UPROPERTY(ReplicatedUsing = OnRep_OwnedBusinessDetails, BlueprintReadOnly, Category = "Player State|Property")
    TArray<FBusinessDetails> OwnedBusinessDetails; // FBusinessDetails is defined in PropertyData.h
    UFUNCTION()
    void OnRep_OwnedBusinessDetails();

    UPROPERTY(ReplicatedUsing = OnRep_RecentTransactions, BlueprintReadOnly, Category = "Player State|Economy")
    TArray<FTransactionRecord> RecentTransactions;
    UFUNCTION()
    void OnRep_RecentTransactions();

    UPROPERTY(ReplicatedUsing = OnRep_PlayerWealth, BlueprintReadOnly, Category = "Player State|Economy")
    int32 PlayerWealth;
    UFUNCTION()
    void OnRep_PlayerWealth();

    UPROPERTY(ReplicatedUsing = OnRep_CurrentSocialStanding, BlueprintReadOnly, Category = "Player State|Social")
    EPlayerSocialStanding CurrentSocialStanding;
    UFUNCTION()
    void OnRep_CurrentSocialStanding();

    UPROPERTY(ReplicatedUsing = OnRep_GlobalReputation, BlueprintReadOnly, Category = "Player State|Social")
    float GlobalReputation;
    UFUNCTION()
    void OnRep_GlobalReputation();

    UPROPERTY(ReplicatedUsing = OnRep_RegionalReputations, BlueprintReadOnly, Category = "Player State|Social")
    TArray<FRegionReputationPair> ReplicatedRegionalReputations;
    UFUNCTION()
    void OnRep_RegionalReputations();

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State|Economy")
    TArray<FString> OwnedProperties; // Legacy, to be phased out or used for simple ID tracking

    UPROPERTY(ReplicatedUsing = OnRep_BountiesByRegion, BlueprintReadOnly, Category = "Player State|Legal")
    TArray<FRegionBountyPair> ReplicatedBountiesByRegion;
    UFUNCTION()
    void OnRep_BountiesByRegion();

    UPROPERTY(ReplicatedUsing = OnRep_PersonalHistory, BlueprintReadOnly, Category = "Player State|Progression")
    TArray<FString> PersonalHistory;
    UFUNCTION()
    void OnRep_PersonalHistory();

    UPROPERTY(ReplicatedUsing = OnRep_PlayerAchievements, BlueprintReadOnly, Category = "Player State|Achievements")
    TArray<FPlayerAchievementProgress> PlayerAchievements; // FPlayerAchievementProgress is defined in AchievementData.h
    UFUNCTION()
    void OnRep_PlayerAchievements();

    UPROPERTY(ReplicatedUsing = OnRep_PlayerQuests, BlueprintReadOnly, Category = "Player State|Quests")
    TArray<FPlayerQuestData> PlayerQuests;
    UFUNCTION()
    void OnRep_PlayerQuests();

    UPROPERTY(ReplicatedUsing = OnRep_UnlockedStoryMilestones, BlueprintReadOnly, Category = "Player State|Story")
    TArray<FStoryMilestone> UnlockedStoryMilestones;
    UFUNCTION()
    void OnRep_UnlockedStoryMilestones();

    UPROPERTY(ReplicatedUsing = OnRep_FactionStandings, BlueprintReadOnly, Category = "Player State|Factions")
    TArray<FFactionStandingPair> ReplicatedFactionStandings;
    UFUNCTION()
    void OnRep_FactionStandings();

    UPROPERTY(ReplicatedUsing = OnRep_SpouseInfo, BlueprintReadOnly, Category = "Player State|Family")
    FString SpouseID;

    UPROPERTY(ReplicatedUsing = OnRep_SpouseInfo, BlueprintReadOnly, Category = "Player State|Family")
    FString SpouseName;
    UFUNCTION()
    void OnRep_SpouseInfo();

    UPROPERTY(ReplicatedUsing = OnRep_FamilyMembers, BlueprintReadOnly, Category = "Player State|Family")
    TArray<FFamilyMember> FamilyMembers;
    UFUNCTION()
    void OnRep_FamilyMembers();

    UPROPERTY(ReplicatedUsing = OnRep_ActiveConditions, BlueprintReadOnly, Category = "Player State|Survival")
    TArray<FName> ActiveConditions; // e.g., "Hungry", "Injured"
    UFUNCTION()
    void OnRep_ActiveConditions();


public:
    // Event dispatcher for state changes
    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnPlayerStateChanged OnPlayerStateChanged;

    // Event dispatchers for property and business events
    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnPropertyIncomeGenerated OnPropertyIncomeGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnBusinessIncomeGenerated OnBusinessIncomeGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnPropertyLost OnPropertyLost;

    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnBusinessLost OnBusinessLost;

    // Event dispatchers for quest tracking
    UPROPERTY(BlueprintAssignable, Category = "Player Quest Events")
    FOnPlayerQuestStateChanged OnPlayerQuestStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Player Quest Events")
    FOnPlayerQuestProgressUpdated OnPlayerQuestProgressUpdated;

    // Achievement event dispatchers
    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnAchievementUnlocked OnAchievementUnlocked; // Delegate from AchievementData.h

    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnAchievementProgressUpdated OnAchievementProgressUpdated; // Delegate from AchievementData.h

    UPROPERTY(BlueprintAssignable, Category = "Player State Events")
    FOnAchievementMilestoneReached OnAchievementMilestoneReached; // Delegate from AchievementData.h

    // Event dispatchers for family events
    UPROPERTY(BlueprintAssignable, Category = "Player State|Family Events")
    FOnPlayerMarried OnPlayerMarried;

    UPROPERTY(BlueprintAssignable, Category = "Player State|Family Events")
    FOnPlayerDivorced OnPlayerDivorced;

    UPROPERTY(BlueprintAssignable, Category = "Player State|Family Events")
    FOnFamilyMemberAdded OnFamilyMemberAdded;

    UPROPERTY(BlueprintAssignable, Category = "Player State|Family Events")
    FOnFamilyMemberRemoved OnFamilyMemberRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Player State|Family Events")
    FOnFamilyMemberStatusChanged OnFamilyMemberStatusChanged;

/**
     * =====================================================================================
     * Client-Callable Wrappers for Server Functions
     * =====================================================================================
     * These functions provide a clean API for blueprints and C++ code on the client.
     * They will automatically call the corresponding Server_ RPC if on a client,
     * or execute directly if on the server.
     */

    // Property and Business Management
    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void AcquireProperty(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void StartBusiness(const FBusinessDetails& BusinessDetails, int32 InitialInvestment);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void LoseProperty(const FGuid& PropertyID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void LoseBusiness(const FGuid& BusinessID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void UpgradeProperty(const FGuid& PropertyID, int32 UpgradeCost);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void UpgradeBusiness(const FGuid& BusinessID, int32 UpgradeCost);

    // Criminal History and Legal Status
    UFUNCTION(BlueprintCallable, Category = "Player State|Legal")
    void AddBounty(const FString& RegionID, float Amount, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Legal")
    void ClearBounty(const FString& RegionID, float AmountPaid);

    // Personal History and Progression
    UFUNCTION(BlueprintCallable, Category = "Player State|Progression")
    void RecordMajorEvent(const FString& EventDescription, const FString& RegionID);

    // Achievements
    UFUNCTION(BlueprintCallable, Category = "Player State|Achievements")
    void UnlockAchievement(const FName& AchievementID);

    UFUNCTION(BlueprintCallable, Category = "Player State|Achievements")
    void UpdateAchievementProgress(const FName& AchievementID, int32 ProgressIncrement);

    UFUNCTION(BlueprintCallable, Category = "Player State|Achievements")
    void SetAchievementProgress(const FName& AchievementID, int32 NewProgress);

    // Personal Quest and Story Tracking
    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void OnQuestStarted(FName QuestID, bool bIsPersonalStory = false);

    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void OnQuestCompleted(FName QuestID);

    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void OnQuestFailed(FName QuestID);

    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void OnQuestAbandoned(FName QuestID);

    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void OnQuestProgressUpdated(FName QuestID, float NewProgressPercentage);

    UFUNCTION(BlueprintCallable, Category = "Player State|Story")
    void UnlockStoryMilestone(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID);

    // Faction Relationships
    UFUNCTION(BlueprintCallable, Category = "Player State|Factions")
    void ModifyFactionStanding(const FName& FactionID, float Amount, const FString& Reason);

    // Family and Marriage System
    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void MarryCharacter(const FString& CharacterID, const FString& CharacterName);

    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void DivorceSpouse(const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void AddChild(const FString& ChildID, const FString& ChildName, int32 Age);

    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void AddFamilyMember(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age);

    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void RemoveFamilyMember(const FString& CharacterID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Family")
    void UpdateFamilyMemberStatus(const FString& CharacterID, bool IsAlive, int32 NewAge);

    // Economic Status
    UFUNCTION(BlueprintCallable, Category = "Player State|Economy")
    void SpendMoney(int32 Amount, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Economy")
    void AddMoney(int32 Amount, const FString& Reason);
    /**
     * Social Standing and Reputation
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Social")
    EPlayerSocialStanding GetCurrentSocialStanding() const { return CurrentSocialStanding; }

    UFUNCTION(BlueprintCallable, Category = "Player State|Social")
    void UpdateSocialStanding(); // Should likely take parameters or use internal logic

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerUpdateSocialStanding();

    UFUNCTION(BlueprintPure, Category = "Player State|Social")
    float GetGlobalReputation() const { return GlobalReputation; }

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Social")
    void ServerModifyGlobalReputation(float Amount, const FString& Reason);
    void ServerModifyGlobalReputation_Implementation(float Amount, const FString& Reason);
    bool ServerModifyGlobalReputation_Validate(float Amount, const FString& Reason);


    UFUNCTION(BlueprintPure, Category = "Player State|Social")
    float GetRegionalReputation(const FString& RegionID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Social")
    void ServerModifyRegionalReputation(const FString& RegionID, float Amount, const FString& Reason);
    void ServerModifyRegionalReputation_Implementation(const FString& RegionID, float Amount, const FString& Reason);
    bool ServerModifyRegionalReputation_Validate(const FString& RegionID, float Amount, const FString& Reason);

    /**
     * Economic Status
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Economy")
    int32 GetPlayerWealth() const { return PlayerWealth; }

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Economy")
    void ServerSpendMoney(int32 Amount, const FString& Reason);
    void ServerSpendMoney_Implementation(int32 Amount, const FString& Reason);
    bool ServerSpendMoney_Validate(int32 Amount, const FString& Reason);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Economy")
    void ServerAddMoney(int32 Amount, const FString& Reason);
    void ServerAddMoney_Implementation(int32 Amount, const FString& Reason);
    bool ServerAddMoney_Validate(int32 Amount, const FString& Reason);

    /**
     * Property and Business Management
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    TArray<FOwnedPropertyDetails> GetOwnedPropertyDetails() const { return OwnedPropertyDetails; }

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    TArray<FBusinessDetails> GetOwnedBusinessDetails() const { return OwnedBusinessDetails; }

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerAcquireProperty(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice);
    void ServerAcquireProperty_Implementation(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice);
    bool ServerAcquireProperty_Validate(const FOwnedPropertyDetails& PropertyDetails, int32 PurchasePrice);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerStartBusiness(const FBusinessDetails& BusinessDetails, int32 InitialInvestment);
    void ServerStartBusiness_Implementation(const FBusinessDetails& BusinessDetails, int32 InitialInvestment);
    bool ServerStartBusiness_Validate(const FBusinessDetails& BusinessDetails, int32 InitialInvestment);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerLoseProperty(const FGuid& PropertyID, const FString& Reason);
    void ServerLoseProperty_Implementation(const FGuid& PropertyID, const FString& Reason);
    bool ServerLoseProperty_Validate(const FGuid& PropertyID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerLoseBusiness(const FGuid& BusinessID, const FString& Reason);
    void ServerLoseBusiness_Implementation(const FGuid& BusinessID, const FString& Reason);
    bool ServerLoseBusiness_Validate(const FGuid& BusinessID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void ProcessDailyIncomeAndExpenses(); // Likely needs server authority if it modifies state

    UFUNCTION(BlueprintCallable, Category = "Player State|Property")
    void ProcessIncomeIfNeeded(); // Likely needs server authority

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    int32 GetTotalDailyIncome() const;

    EPlayerSocialStanding CalculateSocialStanding() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    int32 GetTotalDailyExpenses() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    TArray<FTransactionRecord> GetRecentTransactions(int32 DaysBack = 7) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    FOwnedPropertyDetails GetPropertyInfo(const FGuid& PropertyID) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Property")
    FBusinessDetails GetBusinessInfo(const FGuid& BusinessID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerUpgradeProperty(const FGuid& PropertyID, int32 UpgradeCost);
    void ServerUpgradeProperty_Implementation(const FGuid& PropertyID, int32 UpgradeCost);
    bool ServerUpgradeProperty_Validate(const FGuid& PropertyID, int32 UpgradeCost);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Property")
    void ServerUpgradeBusiness(const FGuid& BusinessID, int32 UpgradeCost);
    void ServerUpgradeBusiness_Implementation(const FGuid& BusinessID, int32 UpgradeCost);
    bool ServerUpgradeBusiness_Validate(const FGuid& BusinessID, int32 UpgradeCost);

    /**
     * Legacy Property Management (for backward compatibility)
     */
    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Economy")
    void ServerAddLegacyProperty(const FString& PropertyID); // Renamed to avoid conflict, marked for server
    void ServerAddLegacyProperty_Implementation(const FString& PropertyID);
    bool ServerAddLegacyProperty_Validate(const FString& PropertyID);



    UFUNCTION(BlueprintPure, Category = "Player State|Economy")
    TArray<FString> GetOwnedProperties() const { return OwnedProperties; }

    UFUNCTION(BlueprintCallable, Category = "Player State|Economy")
    void AddProperty(const FString& PropertyID);

    /**
     * Criminal History and Legal Status
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Legal")
    float GetTotalBountyValue() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Legal")
    float GetBountyInRegion(const FString& RegionID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Legal")
    void ServerAddBounty(const FString& RegionID, float Amount, const FString& Reason);
    void ServerAddBounty_Implementation(const FString& RegionID, float Amount, const FString& Reason);
    bool ServerAddBounty_Validate(const FString& RegionID, float Amount, const FString& Reason);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Legal")
    void ServerClearBounty(const FString& RegionID, float AmountPaid);
    void ServerClearBounty_Implementation(const FString& RegionID, float AmountPaid);
    bool ServerClearBounty_Validate(const FString& RegionID, float AmountPaid);

    UFUNCTION(BlueprintPure, Category = "Player State|Legal")
    bool IsWantedInRegion(const FString& RegionID) const;

    /**
     * Personal History and Progression
     */
    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Progression")
    void ServerRecordMajorEvent(const FString& EventDescription, const FString& RegionID);
    void ServerRecordMajorEvent_Implementation(const FString& EventDescription, const FString& RegionID);
    bool ServerRecordMajorEvent_Validate(const FString& EventDescription, const FString& RegionID);

    UFUNCTION(BlueprintPure, Category = "Player State|Progression")
    TArray<FString> GetPersonalHistory() const { return PersonalHistory; }

    // Enhanced achievement system
    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Achievements")
    void ServerUnlockAchievement(const FName& AchievementID);
    void ServerUnlockAchievement_Implementation(const FName& AchievementID);
    bool ServerUnlockAchievement_Validate(const FName& AchievementID);

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements")
    bool HasAchievement(const FName& AchievementID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Achievements")
    void ServerUpdateAchievementProgress(const FName& AchievementID, int32 ProgressIncrement);
    void ServerUpdateAchievementProgress_Implementation(const FName& AchievementID, int32 ProgressIncrement);
    bool ServerUpdateAchievementProgress_Validate(const FName& AchievementID, int32 ProgressIncrement);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Achievements")
    void ServerSetAchievementProgress(const FName& AchievementID, int32 NewProgress);
    void ServerSetAchievementProgress_Implementation(const FName& AchievementID, int32 NewProgress);
    bool ServerSetAchievementProgress_Validate(const FName& AchievementID, int32 NewProgress);

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements")
    int32 GetAchievementProgress(const FName& AchievementID) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements")
    TArray<FPlayerAchievementProgress> GetUnlockedAchievements() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements")
    TArray<FPlayerAchievementProgress> GetAchievementsInProgress() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements")
    float GetAchievementCompletionPercentage() const;

    /**
     * Static helper functions for easy achievement integration - these should call server versions if modifying state
     */
    UFUNCTION(BlueprintCallable, Category = "Player State|Achievements", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
    static void AwardAchievementToPlayer(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID);

    UFUNCTION(BlueprintCallable, Category = "Player State|Achievements", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
    static void UpdatePlayerAchievementProgress(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID, int32 ProgressIncrement);

    UFUNCTION(BlueprintPure, Category = "Player State|Achievements", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
    static bool DoesPlayerHaveAchievement(const UObject* WorldContextObject, AActor* PlayerActor, const FName& AchievementID);

    /**
     * Personal Quest and Story Tracking
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    TArray<FPlayerQuestData> GetActiveQuests() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    TArray<FPlayerQuestData> GetCompletedQuests() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    TArray<FPlayerQuestData> GetTrackedQuests() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    FPlayerQuestData GetQuestData(FName QuestID) const;

    UFUNCTION(BlueprintCallable, Category = "Player State|Quests")
    void SetQuestTracked(FName QuestID, bool bTracked);
 
    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerSetQuestTracked(FName QuestID, bool bTracked);
    void ServerSetQuestTracked_Implementation(FName QuestID, bool bTracked);
    bool ServerSetQuestTracked_Validate(FName QuestID, bool bTracked);


    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    bool IsQuestTracked(FName QuestID) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    int32 GetActiveQuestCount() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Quests")
    int32 GetCompletedQuestCount() const;

    // Story milestone tracking
    UFUNCTION(BlueprintPure, Category = "Player State|Story")
    TArray<FStoryMilestone> GetUnlockedMilestones() const;

    UFUNCTION(BlueprintPure, Category = "Player State|Story")
    bool HasUnlockedMilestone(FName MilestoneID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Story")
    void ServerUnlockStoryMilestone(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID);
    void ServerUnlockStoryMilestone_Implementation(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID);
    bool ServerUnlockStoryMilestone_Validate(FName MilestoneID, const FText& MilestoneName, const FText& Description, const FString& RegionID);

    // Quest progression hooks (called by QuestManagementSubsystem, should be server-authoritative)
    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerOnQuestStarted(FName QuestID, bool bIsPersonalStory = false);
    void ServerOnQuestStarted_Implementation(FName QuestID, bool bIsPersonalStory);
    bool ServerOnQuestStarted_Validate(FName QuestID, bool bIsPersonalStory);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerOnQuestCompleted(FName QuestID);
    void ServerOnQuestCompleted_Implementation(FName QuestID);
    bool ServerOnQuestCompleted_Validate(FName QuestID);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerOnQuestFailed(FName QuestID);
    void ServerOnQuestFailed_Implementation(FName QuestID);
    bool ServerOnQuestFailed_Validate(FName QuestID);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerOnQuestAbandoned(FName QuestID);
    void ServerOnQuestAbandoned_Implementation(FName QuestID);
    bool ServerOnQuestAbandoned_Validate(FName QuestID);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Quests")
    void ServerOnQuestProgressUpdated(FName QuestID, float NewProgressPercentage);
    void ServerOnQuestProgressUpdated_Implementation(FName QuestID, float NewProgressPercentage);
    bool ServerOnQuestProgressUpdated_Validate(FName QuestID, float NewProgressPercentage);

    /**
     * Faction Relationships
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    float GetFactionStanding(const FName& FactionID) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Factions")
    void ServerModifyFactionStanding(const FName& FactionID, float Amount, const FString& Reason);
    void ServerModifyFactionStanding_Implementation(const FName& FactionID, float Amount, const FString& Reason);
    bool ServerModifyFactionStanding_Validate(const FName& FactionID, float Amount, const FString& Reason);


    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    TArray<FFactionStandingPair> GetAllFactionStandings() const { return ReplicatedFactionStandings; }

    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    TArray<FName> GetFriendlyFactions(float MinStanding = 25.0f) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    TArray<FName> GetHostileFactions(float MaxStanding = -25.0f) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    bool IsFactionFriendly(const FName& FactionID, float MinStanding = 25.0f) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Factions")
    bool IsFactionHostile(const FName& FactionID, float MaxStanding = -25.0f) const;

    /**
     * Family and Marriage System
     */
    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    bool IsMarried() const { return !SpouseID.IsEmpty(); }

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    FString GetSpouseID() const { return SpouseID; }

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    FString GetSpouseName() const { return SpouseName; }

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerMarryCharacter(const FString& CharacterID, const FString& CharacterName);
    void ServerMarryCharacter_Implementation(const FString& CharacterID, const FString& CharacterName);
    bool ServerMarryCharacter_Validate(const FString& CharacterID, const FString& CharacterName);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerDivorceSpouse(const FString& Reason);
    void ServerDivorceSpouse_Implementation(const FString& Reason);
    bool ServerDivorceSpouse_Validate(const FString& Reason);

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    TArray<FFamilyMember> GetFamilyMembers() const { return FamilyMembers; }

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    TArray<FFamilyMember> GetChildren() const;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerAddChild(const FString& ChildID, const FString& ChildName, int32 Age);
    void ServerAddChild_Implementation(const FString& ChildID, const FString& ChildName, int32 Age);
    bool ServerAddChild_Validate(const FString& ChildID, const FString& ChildName, int32 Age);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerAddFamilyMember(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age);
    void ServerAddFamilyMember_Implementation(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age);
    bool ServerAddFamilyMember_Validate(const FString& CharacterID, const FString& CharacterName, const FString& RelationshipType, int32 Age);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerRemoveFamilyMember(const FString& CharacterID, const FString& Reason);
    void ServerRemoveFamilyMember_Implementation(const FString& CharacterID, const FString& Reason);
    bool ServerRemoveFamilyMember_Validate(const FString& CharacterID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Player State|Family")
    void ServerUpdateFamilyMemberStatus(const FString& CharacterID, bool bIsAlive, int32 NewAge);
    void ServerUpdateFamilyMemberStatus_Implementation(const FString& CharacterID, bool bIsAlive, int32 NewAge);
    bool ServerUpdateFamilyMemberStatus_Validate(const FString& CharacterID, bool bIsAlive, int32 NewAge);


    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    bool HasFamilyMember(const FString& CharacterID) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    FFamilyMember GetFamilyMember(const FString& CharacterID) const;

    UFUNCTION(BlueprintPure, Category = "Player State|Family")
    int32 GetFamilySize() const { return FamilyMembers.Num() + (IsMarried() ? 1 : 0); }

    /**
     * Survival and Health Tracking
     */
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Player State|Survival")
    void ServerUpdateSurvivalState(); // Assuming this modifies state
    void UpdateSurvivalState();


    UFUNCTION(BlueprintPure, Category = "Player State|Survival")
    bool IsInCriticalCondition() const; // Based on other replicated stats

    UFUNCTION(BlueprintPure, Category = "Player State|Survival")
    TArray<FName> GetActiveConditions() const { return ActiveConditions; }

    /**
     * Data Persistence
     */
    UFUNCTION(BlueprintCallable, Category = "Player State|Persistence")
    void SavePlayerState(); // Typically server-side logic

    UFUNCTION(BlueprintCallable, Category = "Player State|Persistence")
    void LoadPlayerState(); // Typically server-side logic
};