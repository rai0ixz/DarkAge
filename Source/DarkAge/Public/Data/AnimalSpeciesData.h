#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/WorldEcosystemData.h"
#include "AnimalSpeciesData.generated.h"

USTRUCT(BlueprintType)
struct FAnimalSpeciesData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animal Species")
    FAnimalPopulationData PopulationData;
};