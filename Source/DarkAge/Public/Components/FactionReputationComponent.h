#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FactionData.h"
#include "FactionReputationComponent.generated.h"

/**
 * Struct to track a player's reputation with a faction
 */
USTRUCT(BlueprintType)
struct FFactionReputation
{
    GENERATED_BODY()

    // The faction's ID
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Faction Reputation")
    FName FactionID;

    // The player's reputation with this faction (-100 to 100)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Faction Reputation")
    float ReputationValue;

    // The player's rank within this faction (0 = Outsider, 1-10 = Ranks)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Faction Reputation")
    int32 FactionRank;

    // Constructor
    FFactionReputation()
        : FactionID(NAME_None)
        , ReputationValue(0.0f)
        , FactionRank(0)
    {
    }

    FFactionReputation(FName InFactionID, float InReputationValue = 0.0f, int32 InFactionRank = 0)
        : FactionID(InFactionID)
        , ReputationValue(InReputationValue)
        , FactionRank(InFactionRank)
    {
    }
};

/**
 * Delegate for when faction reputation changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFactionReputationChanged, FName, FactionID, float, OldReputation, float, NewReputation);

/**
 * Delegate for when faction rank changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFactionRankChanged, FName, FactionID, int32, OldRank, int32, NewRank);

/**
 * Component to handle a player's reputation with different factions
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UFactionReputationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFactionReputationComponent();

protected:
    virtual void BeginPlay() override;

    // Map of faction reputations, keyed by faction ID
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Faction Reputation")
    TMap<FName, FFactionReputation> FactionReputations;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /**
     * Get the player's reputation with a faction
     * @param FactionID - The ID of the faction
     * @return The player's reputation value (-100 to 100)
     */
    UFUNCTION(BlueprintPure, Category = "Faction Reputation")
    float GetFactionReputation(FName FactionID) const;

    /**
     * Get the player's rank with a faction
     * @param FactionID - The ID of the faction
     * @return The player's faction rank (0 = Outsider, 1-10 = Ranks)
     */
    UFUNCTION(BlueprintPure, Category = "Faction Reputation")
    int32 GetFactionRank(FName FactionID) const;

    /**
     * Modify the player's reputation with a faction
     * @param FactionID - The ID of the faction
     * @param Delta - The amount to change the reputation by
     * @return True if the reputation was successfully modified
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    bool ModifyFactionReputation(FName FactionID, float Delta);

    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    bool ModifyReputation(FName FactionID, float Delta);

    /**
     * Set the player's reputation with a faction
     * @param FactionID - The ID of the faction
     * @param NewReputation - The new reputation value
     * @return True if the reputation was successfully set
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    bool SetFactionReputation(FName FactionID, float NewReputation);

    /**
     * Set the player's rank with a faction
     * @param FactionID - The ID of the faction
     * @param NewRank - The new faction rank
     * @return True if the rank was successfully set
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    bool SetFactionRank(FName FactionID, int32 NewRank);

    /**
     * Get all factions the player has reputation with
     * @return Array of faction IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    TArray<FName> GetAllFactions() const;

    /**
     * Get the player's reputation data with a faction
     * @param FactionID - The ID of the faction
     * @param OutReputation - The reputation data
     * @return True if the faction was found
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Reputation")
    bool GetFactionReputationData(FName FactionID, FFactionReputation& OutReputation) const;

    /**
     * Check if the player is allied with a faction
     * @param FactionID - The ID of the faction
     * @return True if the player is allied with the faction
     */
    UFUNCTION(BlueprintPure, Category = "Faction Reputation")
    bool IsAlliedWithFaction(FName FactionID) const;

    /**
     * Check if the player is hostile to a faction
     * @param FactionID - The ID of the faction
     * @return True if the player is hostile to the faction
     */
    UFUNCTION(BlueprintPure, Category = "Faction Reputation")
    bool IsHostileToFaction(FName FactionID) const;

    /**
     * Delegate for when faction reputation changes
     */
    UPROPERTY(BlueprintAssignable, Category = "Faction Reputation")
    FOnFactionReputationChanged OnFactionReputationChanged;

    /**
     * Delegate for when faction rank changes
     */
    UPROPERTY(BlueprintAssignable, Category = "Faction Reputation")
    FOnFactionRankChanged OnFactionRankChanged;

private:
    // Helper function to update faction rank based on reputation
    void UpdateFactionRank(FName FactionID);

    // Helper function to get reputation thresholds for ranks
    float GetReputationThresholdForRank(int32 Rank) const;
};