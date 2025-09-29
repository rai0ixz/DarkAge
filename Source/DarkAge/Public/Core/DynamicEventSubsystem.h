#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"

class UWorldManagementSubsystem;
struct FDynamicEventData;
class UPlayerSkillsComponent;
class UFactionManagerSubsystem;

#include "DynamicEventSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDynamicEventType : uint8
{
    BanditRaid     UMETA(DisplayName = "Bandit Raid"),
    MerchantCaravan UMETA(DisplayName = "Merchant Caravan"),
    NaturalDisaster UMETA(DisplayName = "Natural Disaster"),
    PoliticalCrisis UMETA(DisplayName = "Political Crisis"),
    ResourceDiscovery UMETA(DisplayName = "Resource Discovery"),
    Festival       UMETA(DisplayName = "Festival"),
    Plague         UMETA(DisplayName = "Plague Outbreak")
};

UENUM(BlueprintType)
enum class EDynamicEventState : uint8
{
    Dormant    UMETA(DisplayName = "Dormant"),
    Building   UMETA(DisplayName = "Building"),
    Active     UMETA(DisplayName = "Active"),
    Resolving  UMETA(DisplayName = "Resolving"),
    Resolved   UMETA(DisplayName = "Resolved")
};

UENUM(BlueprintType)
enum class EDynamicEventConditionType : uint8
{
    TimeOfDay          UMETA(DisplayName = "Time Of Day"),
    PlayerLevel        UMETA(DisplayName = "Player Level"),
    FactionReputation  UMETA(DisplayName = "Faction Reputation"),
    WorldState         UMETA(DisplayName = "World State")
};

USTRUCT(BlueprintType)
struct FDynamicEventCondition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDynamicEventConditionType ConditionType = EDynamicEventConditionType::TimeOfDay;

    // Generic numeric bounds used by different condition types
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxValue = 0.0f;

    // Target identifier (faction id, state id, etc.) for conditions that need it
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetID = NAME_None;
};

USTRUCT(BlueprintType)
struct FDynamicEventData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EventID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDynamicEventType EventType = EDynamicEventType::BanditRaid;

    // 0..1 base chance to occur when conditions are met
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseChanceToOccur = 0.1f;

    // Conditions required for this template to be eligible
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDynamicEventCondition> Conditions;
};

USTRUCT(BlueprintType)
struct FDynamicEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EventID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDynamicEventType EventType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDynamicEventState State;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RegionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeRemaining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, float> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> TriggeredQuests;

    FDynamicEvent()
        : EventType(EDynamicEventType::BanditRaid)
        , State(EDynamicEventState::Dormant)
        , Intensity(1.0f)
        , Duration(0.0f)
        , TimeRemaining(0.0f)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDynamicEventTriggered, FName, EventID);

UCLASS()
class DARKAGE_API UDynamicEventSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UDynamicEventSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Manual tick used by owning systems
    void Tick(float DeltaTime);

    // Event generation and management
    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    FDynamicEvent GenerateRandomEvent(const FString& RegionID = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    void TriggerEvent(const FDynamicEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    void UpdateEvent(const FString& EventID, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    void ResolveEvent(const FString& EventID);

    // Event queries
    UFUNCTION(BlueprintPure, Category = "Dynamic Events")
    TArray<FDynamicEvent> GetActiveEvents() const;

    UFUNCTION(BlueprintPure, Category = "Dynamic Events")
    TArray<FDynamicEvent> GetEventsInRegion(const FString& RegionID) const;

    // Event effects
    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    void ApplyEventEffects(const FDynamicEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "Dynamic Events")
    void RemoveEventEffects(const FDynamicEvent& Event);

    // Broadcast when a dynamic event template triggers
    UPROPERTY(BlueprintAssignable, Category = "Dynamic Events")
    FOnDynamicEventTriggered OnDynamicEventTriggered;

    // Config/Data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Events")
    UDataTable* DynamicEventDataTable = nullptr;

    // Timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Events")
    float EventCheckInterval = 30.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dynamic Events")
    float TimeSinceLastCheck = 0.0f;

private:
    // External systems
    UPROPERTY()
    UWorldManagementSubsystem* WorldManagementSubsystem = nullptr;

    // Runtime state
    UPROPERTY()
    TMap<FString, FDynamicEvent> ActiveEvents;

    UPROPERTY()
    TArray<FDynamicEvent> EventTemplates;

    // Internal helpers
    void InitializeEventTemplates();
    void CheckForEvents();
    bool AreEventConditionsMet(const FDynamicEvent& EventData) const; // Overload for runtime events
    bool AreEventConditionsMet(const FDynamicEventData& EventData) const; // For template/data table rows

    FDynamicEvent GenerateBanditRaidEvent(const FString& RegionID);
    FDynamicEvent GenerateMerchantCaravanEvent(const FString& RegionID);
    FDynamicEvent GenerateNaturalDisasterEvent(const FString& RegionID);
    FDynamicEvent GeneratePoliticalCrisisEvent(const FString& RegionID);
    FDynamicEvent GenerateResourceDiscoveryEvent(const FString& RegionID);
    FDynamicEvent GenerateFestivalEvent(const FString& RegionID);
    FDynamicEvent GeneratePlagueEvent(const FString& RegionID);

    void UpdateEventEffects(FDynamicEvent& Event, float DeltaTime);
    void CheckEventTriggers(const FDynamicEvent& Event);
};