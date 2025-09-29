#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProceduralContentGenerator.generated.h"

USTRUCT(BlueprintType)
struct FProceduralLocation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FString LocationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FString LocationType; // "Village", "Castle", "Forest", "Cave", etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<FString> Features; // "River", "Mine", "Farm", etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    int32 DangerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    int32 EconomicValue;

    FProceduralLocation()
    {
        DangerLevel = 1;
        EconomicValue = 1;
    }
};

USTRUCT(BlueprintType)
struct FProceduralQuest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestType; // "Delivery", "Combat", "Exploration", "Social"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FVector TargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> Objectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TMap<FString, FString> QuestData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 Difficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardGold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> RewardItems;

    FProceduralQuest()
    {
        Difficulty = 1;
        RewardGold = 0;
    }
};

USTRUCT(BlueprintType)
struct FProceduralNPC
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    FString NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    FString NPCType; // "Merchant", "Guard", "Farmer", "Noble"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    FString PersonalityType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TArray<FString> Background;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TMap<FString, int32> Skills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TMap<FString, FString> Relationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    int32 Wealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    int32 Influence;

    FProceduralNPC()
    {
        Wealth = 0;
        Influence = 0;
    }
};

UCLASS()
class DARKAGE_API UProceduralContentGenerator : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UProceduralContentGenerator();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Location generation
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    FProceduralLocation GenerateLocation(const FVector& BasePosition, const FString& PreferredType = "");

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    TArray<FProceduralLocation> GenerateRegion(int32 LocationCount, const FVector& Center, float Radius);

    // Quest generation
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    FProceduralQuest GenerateQuest(const FString& QuestType, int32 Difficulty, const FVector& PlayerLocation);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    TArray<FProceduralQuest> GenerateQuestChain(int32 ChainLength, const FVector& StartLocation);

    // NPC generation
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    FProceduralNPC GenerateNPC(const FString& NPCType, int32 PowerLevel);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    TArray<FProceduralNPC> GenerateSettlementNPCs(int32 NPCCount, const FString& SettlementType);

    // Event generation
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    FString GenerateRandomEvent(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    TArray<FString> GenerateEventChain(int32 EventCount);

    // Content seeding
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void SeedRandomGenerator(int32 Seed);

    UFUNCTION(BlueprintPure, Category = "Procedural")
    int32 GetCurrentSeed() const { return RandomSeed; }

private:
    // Random generation data
    int32 RandomSeed;
    FRandomStream RandomStream;

    // Content templates
    TArray<FString> LocationTypes;
    TArray<FString> LocationFeatures;
    TArray<FString> QuestTypes;
    TArray<FString> NPCNames;
    TArray<FString> NPCBackgrounds;
    TArray<FString> EventTypes;

    // Generation functions
    FString GenerateLocationName(const FString& Type);
    FVector GenerateLocationPosition(const FVector& BasePosition, float MinDistance, float MaxDistance);
    TArray<FString> GenerateLocationFeatures(const FString& Type, int32 FeatureCount);
    int32 CalculateLocationDanger(const FString& Type, const TArray<FString>& Features);
    int32 CalculateLocationValue(const FString& Type, const TArray<FString>& Features);

    FString GenerateQuestTitle(const FString& Type, int32 Difficulty);
    FString GenerateQuestDescription(const FProceduralQuest& Quest);
    TArray<FString> GenerateQuestObjectives(const FString& Type, int32 Difficulty);
    int32 CalculateQuestReward(const FProceduralQuest& Quest);

    FString GenerateNPCName(const FString& Type);
    TArray<FString> GenerateNPCBackground(const FString& Type, int32 Count);
    TMap<FString, int32> GenerateNPCSkills(const FString& Type, int32 PowerLevel);
    TMap<FString, FString> GenerateNPCRelationships(const FProceduralNPC& NPC);

    // Utility functions
    FString GetRandomElement(const TArray<FString>& Array);
    int32 GetRandomInt(int32 Min, int32 Max);
    float GetRandomFloat(float Min, float Max);
    bool GetRandomBool(float TrueChance = 0.5f);

    void InitializeContentTemplates();
};