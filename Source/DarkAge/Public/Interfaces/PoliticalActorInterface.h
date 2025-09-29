#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/FactionData.h"
#include "Core/PoliticalSystem.h"
#include "PoliticalActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UPoliticalActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can perform political actions
 */
class DARKAGE_API IPoliticalActorInterface
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to negotiate a treaty between factions
	 * @param TreatyType - The type of treaty to negotiate
	 * @param Faction1ID - The first faction involved
	 * @param Faction2ID - The second faction involved
	 * @param DurationDays - How long the treaty should last (0 = indefinite)
	 * @return True if the negotiation was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool NegotiateTreaty(ETreatyType TreatyType, FName Faction1ID, FName Faction2ID, int32 DurationDays);

	/**
	 * Attempt to sabotage relations between factions
	 * @param TargetFaction1ID - The first faction to affect
	 * @param TargetFaction2ID - The second faction to affect
	 * @return True if the sabotage was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool SabotageRelations(FName TargetFaction1ID, FName TargetFaction2ID);

	/**
	 * Attempt to incite a rebellion in a region
	 * @param RegionID - The region to incite rebellion in
	 * @param RebelFactionID - The faction that will lead the rebellion
	 * @return True if the incitement was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool InciteRebellion(FName RegionID, FName RebelFactionID);

	/**
	 * Attempt to spread propaganda for a faction
	 * @param FactionID - The faction to spread propaganda for
	 * @param RegionID - The region to spread propaganda in
	 * @return True if the propaganda campaign was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool SpreadPropaganda(FName FactionID, FName RegionID);

	/**
	 * Attempt to mediate peace between warring factions
	 * @param Faction1ID - The first faction
	 * @param Faction2ID - The second faction
	 * @return True if the mediation was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool MediatePeace(FName Faction1ID, FName Faction2ID);

	/**
	 * Attempt to influence a political event's outcome
	 * @param EventID - The ID of the event to influence
	 * @param DesiredOutcome - True for success, false for failure
	 * @return True if the influence attempt was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	bool InfluencePoliticalEvent(FGuid EventID, bool DesiredOutcome);

	/**
	 * Get the actor's political influence level with a faction
	 * @param FactionID - The faction to check influence with
	 * @return The influence level (0-100)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Political Actions")
	int32 GetPoliticalInfluence(FName FactionID);
};