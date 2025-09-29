#pragma once

#include "CoreMinimal.h"
#include "NPCEcosystemData.generated.h"

UENUM(BlueprintType)
enum class ENPCVocation : uint8
{
    None,
    Guard,
    Merchant,
    Crafter,
    Hunter,
    Farmer
};

UENUM(BlueprintType)
enum class ENPCProfession : uint8
{
    None,
    Farmer,
    Merchant,
    Guard,
    Innkeeper,
    Miller,
    Herbalist,
    Blacksmith,
    Scholar,
    Priest,
    Noble,
    Miner,
    Fisherman,
    Sailor
};

UENUM(BlueprintType)
enum class ENPCActivity : uint8
{
    Sleeping,
    Working,
    Eating,
    Socializing,
    Relaxing,
    Trading,
    Patrolling,
    Training
};

UENUM(BlueprintType)
enum class ENPCMood : uint8
{
    Content,
    Happy,
    Sad,
    Angry,
    Fearful,
    Excited,
    Depressed
};

UENUM(BlueprintType)
enum class ENPCHealthStatus : uint8
{
    Healthy,
    Sick,
    Injured,
    Dying,
    Recovering
};

UENUM(BlueprintType)
enum class ERelationshipType : uint8
{
    Acquaintance,
    Friend,
    Rival,
    Romantic,
    Family
};

USTRUCT(BlueprintType)
struct FNPCPersonalityTraits
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Sociability = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Ambition = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Loyalty = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Courage = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Intelligence = 0.5f;
};

USTRUCT(BlueprintType)
struct FNPCNeeds
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Hunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Thirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Rest = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Safety = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Social = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Purpose = 100.0f;
};

USTRUCT(BlueprintType)
struct FNPCRelationship
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid TargetNPCID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERelationshipType RelationshipType = ERelationshipType::Acquaintance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RelationshipStrength = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LastInteractionTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FScheduleEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENPCActivity Activity = ENPCActivity::Sleeping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Location;
};

USTRUCT(BlueprintType)
struct FNPCArchetype
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ArchetypeName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WorkEfficiencyModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SocialNeedModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RestNeedModifier = 1.0f;
};

USTRUCT(BlueprintType)
struct FNPCData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FString NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    ENPCVocation Vocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    bool bIsAlive;

    // Extended NPC data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FGuid NPCID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FString HomeSettlement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FString CurrentSettlement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    ENPCProfession Profession = ENPCProfession::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FNPCPersonalityTraits PersonalityTraits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    FNPCNeeds Needs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    TMap<FGuid, FNPCRelationship> Relationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    ENPCActivity CurrentActivity = ENPCActivity::Sleeping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    ENPCMood Mood = ENPCMood::Content;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    ENPCHealthStatus HealthStatus = ENPCHealthStatus::Healthy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
    TArray<FScheduleEntry> DailySchedule;

    FNPCData()
        : Vocation(ENPCVocation::None)
        , bIsAlive(true)
        , Profession(ENPCProfession::None)
        , CurrentActivity(ENPCActivity::Sleeping)
        , Mood(ENPCMood::Content)
        , HealthStatus(ENPCHealthStatus::Healthy)
    {}
};