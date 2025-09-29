#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "DA_NPCManagerSubsystem.generated.h"

class UAINeedsPlanningComponent;

/**
 * Manages all NPCs with a UAINeedsPlanningComponent,
 * replacing their individual Tick() calls with a centralized,
 * time-sliced update loop to optimize performance.
 */
UCLASS()
class DARKAGE_API UDA_NPCManagerSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return !bIsPaused; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDA_NPCManagerSubsystem, STATGROUP_Tickables); }

    /** Registers an NPC's planning component with the manager. */
    void RegisterComponent(UAINeedsPlanningComponent* Component);
 
    /** Unregisters an NPC's planning component from the manager. */
    void UnregisterComponent(UAINeedsPlanningComponent* Component);
 
    /** Read-only access to registered components for debugging and tools. */
    const TArray<TObjectPtr<UAINeedsPlanningComponent>>& GetRegisteredComponents() const { return RegisteredComponents; }
 
    /** Force an immediate refresh of cached world data (debug/tool use). */
    UFUNCTION(BlueprintCallable, Category = "NPC Manager")
    void ForceUpdateWorldCache();
 
    private:
   
    /** All registered NPC components that need updating. */
    UPROPERTY()
    TArray<TObjectPtr<UAINeedsPlanningComponent>> RegisteredComponents;

    /** NPCs close to the player, updated frequently. */
    TArray<TObjectPtr<UAINeedsPlanningComponent>> HighFrequencyComponents;

    /** NPCs at a medium distance, updated moderately. */
    TArray<TObjectPtr<UAINeedsPlanningComponent>> MediumFrequencyComponents;
    
    /** NPCs far from the player, updated infrequently. */
    TArray<TObjectPtr<UAINeedsPlanningComponent>> LowFrequencyComponents;
    
    /** Timer to control the bucketing process. */
    float BucketingTimer = 0.0f;
    
    /** Current index for time-sliced updates in each bucket. */
    int32 HighFrequencyUpdateIndex = 0;
    int32 MediumFrequencyUpdateIndex = 0;
    int32 LowFrequencyUpdateIndex = 0;

    /** Re-categorizes all registered components into the frequency buckets. */
    void UpdateBuckets();

    /** Updates cached world data for efficient AI queries. */
    void UpdateWorldCache();

public:
    /** Gets cached hostile actors near a location. */
    TArray<AActor*> GetNearbyHostiles(const FVector& Location, float Radius) const;

    /** Gets cached friendly NPCs near a location. */
    TArray<AActor*> GetNearbyFriendlyNPCs(const FVector& Location, float Radius) const;

    /** Gets cached water sources near a location. */
    TArray<AActor*> GetNearbyWaterSources(const FVector& Location, float Radius) const;

    /** Gets cached shelter locations near a location. */
    TArray<AActor*> GetNearbyShelters(const FVector& Location, float Radius) const;

private:
    /** Flag to pause the entire AI simulation. */
    bool bIsPaused = false;

    // Advanced AI Group Behaviors
    /** NPC groups for coordinated behavior. */
    TMap<FString, TArray<TObjectPtr<UAINeedsPlanningComponent>>> NPCGroups;

    /** Group leaders and their strategies. */
    TMap<FString, FString> GroupLeaders;

    /** Group objectives and coordination. */
    TMap<FString, FString> GroupObjectives;

    /** Social relationships between NPCs. */
    TMap<FString, TMap<FString, float>> SocialRelationships;

    /** Group formation timer. */
    float GroupFormationTimer = 0.0f;

private:
    /** Cached hostile actors for efficient danger evaluation. */
    TArray<TObjectPtr<AActor>> CachedHostiles;

    /** Cached friendly NPCs for social evaluation. */
    TArray<TObjectPtr<AActor>> CachedFriendlyNPCs;

    /** Cached water sources for thirst needs. */
    TArray<TObjectPtr<AActor>> CachedWaterSources;

    /** Cached shelter locations for rest needs. */
    TArray<TObjectPtr<AActor>> CachedShelters;

    /** Timer for updating world cache. */
    float CacheUpdateTimer = 0.0f;

    // Advanced AI coordination functions
    void UpdateGroupBehaviors(float DeltaTime);
    void FormGroupsBasedOnContext();
    void AssignGroupObjectives();
    void CoordinateGroupActions();
    bool ShouldNPCJoinGroup(UAINeedsPlanningComponent* NPC, const FString& GroupID);
    void UpdateSocialRelationships(float DeltaTime);
    float CalculateSocialBond(UAINeedsPlanningComponent* NPC1, UAINeedsPlanningComponent* NPC2);
    void ExecuteGroupStrategy(const FString& GroupID, const FString& Strategy);
    TArray<UAINeedsPlanningComponent*> GetNearbyAllies(UAINeedsPlanningComponent* NPC, float Radius);
    void HandleGroupCommunication(UAINeedsPlanningComponent* Sender, const FString& Message, float Radius);
};