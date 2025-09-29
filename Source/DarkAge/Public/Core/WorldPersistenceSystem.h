#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/RegionState.h"
#include "Core/TimeSystem.h"
#include "Core/GlobalEventBus.h"
#include "Core/WorldStateLock.h"
#include "WorldPersistenceSystem.generated.h"

UENUM(BlueprintType)
enum class EWorldActionType : uint8
{
    Combat       UMETA(DisplayName = "Combat"),
    Trade        UMETA(DisplayName = "Trade"),
    Crafting     UMETA(DisplayName = "Crafting"),
    Conversation UMETA(DisplayName = "Conversation"),
    Exploration  UMETA(DisplayName = "Exploration"),
    Crime        UMETA(DisplayName = "Crime"),
    Faction      UMETA(DisplayName = "Faction"),
    Environment  UMETA(DisplayName = "Environment"),
    Resource     UMETA(DisplayName = "Resource")
};

USTRUCT(BlueprintType)
struct FWorldAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    EWorldActionType ActionType = EWorldActionType::Exploration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FName RegionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FGameDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    float Magnitude = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FName TargetID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FString AdditionalData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    bool bProcessed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FGuid ActionID;

    FWorldAction()
        : Timestamp()
        , ActionID(FGuid::NewGuid())
    {
    }
};

USTRUCT(BlueprintType)
struct FWorldEventLogEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FName EventType = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FName RegionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
    FGameDateTime Timestamp;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWorldStateChanged, FName, RegionID, const FRegionState&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorldEvent, FName, EventType, FName, RegionID, const FString&, Description);

UCLASS()
class DARKAGE_API UWorldPersistenceSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UWorldPersistenceSystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Ticking/update for persistence simulation
    void UpdateWorldPersistence(float DeltaTime);

    // Record a player action in the world
    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void RecordPlayerAction(EWorldActionType ActionType, FVector Location, FName RegionID,
                            float Magnitude, FName TargetID, const FString& AdditionalData);

    // Region queries
    UFUNCTION(BlueprintPure, Category = "Persistence")
    FRegionState GetRegionState(FName RegionID) const;

    // Run multi-cycle simulation
    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void SimulateWorldEvolution(int32 GameMinutes);

    // Region insights
    UFUNCTION(BlueprintPure, Category = "Persistence")
    TArray<FString> GetRecentRegionalEvents(FName RegionID, int32 Count = 5) const;

    UFUNCTION(BlueprintPure, Category = "Persistence")
    float GetPlayerRegionalImpact(FName RegionID) const;

    // Save/Load world state (single file)
    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool SaveWorldState();

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool LoadWorldState();

    // Event query helpers
    UFUNCTION(BlueprintPure, Category = "Persistence")
    TArray<FWorldEventLogEntry> QueryWorldEvents(FName EventType, FName RegionID, int32 MaxResults = 50) const;

    UFUNCTION(BlueprintPure, Category = "Persistence")
    int32 GetEventCount(FName EventType, FName RegionID) const;

    // Optional: types listened from GlobalEventBus
    TArray<EGlobalEventType> GetListenedEventTypes() const;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Persistence")
    FOnWorldStateChanged OnWorldStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Persistence")
    FOnWorldEvent OnWorldEvent;

private:
    // Internal helpers
    FString GetWorldStateSavePath() const;
    void ProcessPlayerActions();
    void UpdateRegion(FName RegionID, float DeltaTime);
    void GenerateRandomEvent(FName RegionID);
    void ApplyActionEffects(const FWorldAction& Action);
    void SimulateNPCActions(float DeltaTime);
    void SimulateFactionDynamics(float DeltaTime);
    void UpdateResourceLevels(float DeltaTime);
    FString GenerateEventDescription(FName EventType, FName RegionID, const TMap<FString, FString>& Parameters);

private:
    // Region database
    UPROPERTY()
    TMap<FName, FRegionState> RegionStates;

    // Recorded player actions
    UPROPERTY()
    TArray<FWorldAction> PlayerActions;

    // Event log
    UPROPERTY()
    TArray<FWorldEventLogEntry> GlobalEventLog;

    // Cached subsystems
    UPROPERTY()
    UTimeSystem* TimeSystem = nullptr;

    UPROPERTY()
    UWorldStateLock* WorldStateLock = nullptr;

    UPROPERTY()
    UGlobalEventBus* GlobalEventBus = nullptr;

    // Timers
    float TimeSinceLastUpdate = 0.0f;
    float TimeSinceLastSave = 0.0f;

    // Current save/load lock
    FGuid CurrentSaveLockID;
};