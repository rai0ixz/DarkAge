#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FactionData.generated.h"

UENUM(BlueprintType)
enum class EFactionID : uint8
{
	TheIronBrotherhood UMETA(DisplayName = "The Iron Brotherhood"),
	TheMerchantCoalition UMETA(DisplayName = "The Merchant Coalition"),
	TheCircleOfDruids UMETA(DisplayName = "The Circle of Druids"),
	TheScholarsOfSundering UMETA(DisplayName = "The Scholars of the Sundering"),
	None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EFactionRelationshipType : uint8
{
	Hostile UMETA(DisplayName = "Hostile"),
	Neutral UMETA(DisplayName = "Neutral"),
	Allied UMETA(DisplayName = "Allied"),
	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FFactionInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FName FactionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	float Reputation = 0.0f;
};

USTRUCT(BlueprintType)
struct FFaction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    TMap<FName, EFactionRelationshipType> Relationships;

    friend FArchive& operator<<(FArchive& Ar, FFaction& Faction)
    {
        Ar << Faction.DisplayName;
        Ar << Faction.Relationships;
        return Ar;
    }
};