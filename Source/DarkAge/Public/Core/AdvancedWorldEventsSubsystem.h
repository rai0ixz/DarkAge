#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "AdvancedWorldEventsSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWorldEventType : uint8
{
    Natural         UMETA(DisplayName = "Natural Disaster"),
    Political       UMETA(DisplayName = "Political Event"),
    Economic        UMETA(DisplayName = "Economic Event"),
    Military        UMETA(DisplayName = "Military Event"),
    Social          UMETA(DisplayName = "Social Event"),
    Supernatural    UMETA(DisplayName = "Supernatural Event"),
    Technological   UMETA(DisplayName = "Technological Event"),
    Environmental   UMETA(DisplayName = "Environmental Event")
};

UENUM(BlueprintType)
enum class EEventSeverity : uint8
{
    Minor       UMETA(DisplayName = "Minor"),
    Moderate    UMETA(DisplayName = "Moderate"),
    Major       UMETA(DisplayName = "Major"),
    Critical    UMETA(DisplayName = "Critical"),
    Catastrophic UMETA(DisplayName = "Catastrophic")
};

UENUM(BlueprintType)
enum class EEventScope : uint8
{
    Local       UMETA(DisplayName = "Local"),
    Regional    UMETA(DisplayName = "Regional"),
    National    UMETA(DisplayName = "National"),
    Global      UMETA(DisplayName = "Global")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FWorldEventEffect
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Event Effect")
    FString EffectType;

    UPROPERTY(BlueprintReadOnly, Category = "Event Effect")
    float Magnitude = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Event Effect")
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Event Effect")
    TArray<FString> AffectedSystems;

    UPROPERTY(BlueprintReadOnly, Category = "Event Effect")
    TMap<FString, float> SystemModifiers;

    FWorldEventEffect()
    {
        EffectType = TEXT("");
        Magnitude = 1.0f;
        Duration = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FWorldEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    FString EventID;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    FString EventName;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    EWorldEventType EventType = EWorldEventType::Natural;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    EEventSeverity Severity = EEventSeverity::Minor;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    EEventScope Scope = EEventScope::Local;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    FVector EpicenterLocation;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    float AffectedRadius = 1000.0f;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    TArray<FWorldEventEffect> Effects;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    TArray<FString> Prerequisites;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    TArray<FString> TriggerEvents;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    bool bHasCascadingEffects = true;

    UPROPERTY(BlueprintReadOnly, Category = "World Event")
    float CascadeProbability = 0.3f;

    FWorldEvent()
    {
        EventID = TEXT("");
        EventName = TEXT("");
        Description = TEXT("");
        EventType = EWorldEventType::Natural;
        Severity = EEventSeverity::Minor;
        Scope = EEventScope::Local;
        EpicenterLocation = FVector::ZeroVector;
        AffectedRadius = 1000.0f;
        StartTime = 0.0f;
        Duration = 0.0f;
        bIsActive = false;
        bHasCascadingEffects = true;
        CascadeProbability = 0.3f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FEventChain
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Event Chain")
    FString ChainID;

    UPROPERTY(BlueprintReadOnly, Category = "Event Chain")
    TArray<FString> EventSequence;

    UPROPERTY(BlueprintReadOnly, Category = "Event Chain")
    int32 CurrentEventIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Event Chain")
    float ChainStartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Event Chain")
    bool bIsActive = false;

    FEventChain()
    {
        ChainID = TEXT("");
        CurrentEventIndex = 0;
        ChainStartTime = 0.0f;
        bIsActive = false;
    }
};

// World Event Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldEventTriggered, const FWorldEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldEventEnded, const FWorldEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventChainStarted, const FString&, ChainID, const FEventChain&, Chain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCascadeEventTriggered, const FString&, OriginalEventID, const FString&, CascadeEventID);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedWorldEventsSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedWorldEventsSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedWorldEventsSubsystem, STATGROUP_Tickables); }

    // Event Delegates
    UPROPERTY(BlueprintAssignable, Category = "World Events")
    FOnWorldEventTriggered OnWorldEventTriggered;

    UPROPERTY(BlueprintAssignable, Category = "World Events")
    FOnWorldEventEnded OnWorldEventEnded;

    UPROPERTY(BlueprintAssignable, Category = "World Events")
    FOnEventChainStarted OnEventChainStarted;

    UPROPERTY(BlueprintAssignable, Category = "World Events")
    FOnCascadeEventTriggered OnCascadeEventTriggered;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "World Events")
    void TriggerEvent(const FString& EventID);

    UFUNCTION(BlueprintCallable, Category = "World Events")
    void TriggerEventAtLocation(const FString& EventID, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "World Events")
    void StartEventChain(const FString& ChainID);

    UFUNCTION(BlueprintCallable, Category = "World Events")
    void ForceEndEvent(const FString& EventID);

    UFUNCTION(BlueprintPure, Category = "World Events")
    TArray<FWorldEvent> GetActiveEvents() const;

    UFUNCTION(BlueprintPure, Category = "World Events")
    TArray<FEventChain> GetActiveEventChains() const;

    UFUNCTION(BlueprintPure, Category = "World Events")
    bool IsEventActive(const FString& EventID) const;

    UFUNCTION(BlueprintPure, Category = "World Events")
    float GetEventSeverityMultiplier(EEventSeverity Severity) const;

    UFUNCTION(BlueprintCallable, Category = "World Events")
    void SetEventProbabilityModifier(EWorldEventType EventType, float Modifier);

protected:
    // Event data
    UPROPERTY()
    TMap<FString, FWorldEvent> EventTemplates;

    UPROPERTY()
    TArray<FWorldEvent> ActiveEvents;

    UPROPERTY()
    TMap<FString, FEventChain> EventChains;

    UPROPERTY()
    TArray<FEventChain> ActiveEventChains;

    // Event probability modifiers
    UPROPERTY()
    TMap<EWorldEventType, float> EventProbabilityModifiers;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Configuration")
    float BaseEventCheckInterval = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Configuration")
    float GlobalEventProbability = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Configuration")
    int32 MaxConcurrentEvents = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Configuration")
    bool bEnableCascadingEvents = true;

    // Internal state
    float EventCheckTimer = 0.0f;
    int32 NextEventID = 1;

    // Initialization functions
    void InitializeEventTemplates();
    void InitializeEventChains();
    void InitializeProbabilityModifiers();

    // Event management functions
    void ProcessEventGeneration();
    void UpdateActiveEvents(float DeltaTime);
    void UpdateEventChains(float DeltaTime);
    void ProcessCascadingEffects(const FWorldEvent& TriggerEvent);

    // Event creation functions
    FWorldEvent CreateEventFromTemplate(const FString& TemplateID);
    void ApplyEventEffects(const FWorldEvent& Event);
    void RemoveEventEffects(const FWorldEvent& Event);

    // Cascade system functions
    void TriggerCascadeEvent(const FWorldEvent& OriginalEvent);
    TArray<FString> DetermineCascadeEvents(const FWorldEvent& Event);
    float CalculateCascadeProbability(const FWorldEvent& OriginalEvent, const FString& PotentialCascadeID);

    // World state analysis functions
    float AnalyzeWorldStability();
    float CalculateEventProbability(EWorldEventType EventType);
    bool CheckEventPrerequisites(const FWorldEvent& Event);

    // System integration functions
    void NotifyAffectedSystems(const FWorldEvent& Event);
    void ApplySystemModifiers(const FWorldEvent& Event);
    void RemoveSystemModifiers(const FWorldEvent& Event);

    // Helper functions
    FString GenerateUniqueEventID();
    EEventSeverity DetermineEventSeverity(EWorldEventType EventType);
    float GetDistanceFromEpicenter(const FVector& Location, const FWorldEvent& Event);
    bool IsLocationAffectedByEvent(const FVector& Location, const FWorldEvent& Event);
};