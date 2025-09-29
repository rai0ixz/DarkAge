#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "PoliticalData.generated.h"

UENUM(BlueprintType)
enum class EPoliticalEventType : uint8
{
	None,
	Assassination,
	Scandal,
	TerritorialDispute,
	TradeOffer,
	Rebellion,
	War,
	PeaceTreaty,
	WarDeclaration,
	AllianceFormed,
	TradeDispute,
	DiplomaticMarriage,
	ResourceDiscovery,
	LeadershipChange
};
