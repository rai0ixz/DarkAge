#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SurvivalSkillData.generated.h"

UENUM(BlueprintType)
enum class ESurvivalSkill : uint8
{
    Firemaking,
    ShelterBuilding,
    WaterPurification,
    Foraging,
    Hunting,
    Trapping,
    FirstAid
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSurvivalSkillDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    ESurvivalSkill Skill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (MultiLine = true))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    int32 MaxLevel;
    FORCEINLINE FSurvivalSkillDefinition()
        : Skill(ESurvivalSkill::Firemaking)
        , DisplayName(FText::GetEmpty())
        , Description(FText::GetEmpty())
        , MaxLevel(0)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSurvivalSkillInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
    int32 CurrentLevel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
    float CurrentXP;

    FSurvivalSkillInstanceData()
        : CurrentLevel(1)
        , CurrentXP(0.0f)
    {
    }
};