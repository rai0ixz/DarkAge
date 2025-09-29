#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MinimapIconData.generated.h"

UENUM(BlueprintType)
enum class EMinimapIconType : uint8
{
    Player,
    Enemy,
    QuestObjective,
    PointOfInterest
};

USTRUCT(BlueprintType)
struct FMinimapIconData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMinimapIconType IconType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> IconTexture;
    FORCEINLINE FMinimapIconData()
        : IconType(EMinimapIconType::PointOfInterest)
        , IconTexture(nullptr)
    {
    }
};