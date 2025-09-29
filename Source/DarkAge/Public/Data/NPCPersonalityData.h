#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPCPersonalityData.generated.h"

UENUM(BlueprintType)
enum class ENPCTrait : uint8
{
    Brave       UMETA(DisplayName = "Brave"),
    Cowardly    UMETA(DisplayName = "Cowardly"),
    Greedy      UMETA(DisplayName = "Greedy"),
    Generous    UMETA(DisplayName = "Generous"),
    Honest      UMETA(DisplayName = "Honest"),
    Deceitful   UMETA(DisplayName = "Deceitful"),
    Hardworking UMETA(DisplayName = "Hardworking"),
    Lazy        UMETA(DisplayName = "Lazy"),
    Social      UMETA(DisplayName = "Social"),
    Reclusive   UMETA(DisplayName = "Reclusive"),
    Ambitious   UMETA(DisplayName = "Ambitious"),
    Content     UMETA(DisplayName = "Content"),
    Aggressive  UMETA(DisplayName = "Aggressive"),
    Peaceful    UMETA(DisplayName = "Peaceful")
};

USTRUCT(BlueprintType)
struct FNPCPersonalityModifiers
{
    GENERATED_BODY()

    // Need decay rate modifiers (multipliers)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
    float HungerDecayModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
    float ThirstDecayModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
    float RestDecayModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
    float SafetyDecayModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
    float SocialDecayModifier = 1.0f;

    // Behavior modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float RiskTolerance = 0.5f; // 0 = very risk averse, 1 = very risk tolerant

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float SocialPreference = 0.5f; // 0 = prefers solitude, 1 = very social

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float WorkEthic = 0.5f; // 0 = lazy, 1 = hardworking

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float Generosity = 0.5f; // 0 = greedy, 1 = generous

    FNPCPersonalityModifiers()
    {
        HungerDecayModifier = 1.0f;
        ThirstDecayModifier = 1.0f;
        RestDecayModifier = 1.0f;
        SafetyDecayModifier = 1.0f;
        SocialDecayModifier = 1.0f;
        RiskTolerance = 0.5f;
        SocialPreference = 0.5f;
        WorkEthic = 0.5f;
        Generosity = 0.5f;
    }
};

UCLASS()
class DARKAGE_API UNPCPersonalityData : public UDataAsset
{
    GENERATED_BODY()

public:
    // Personality name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
    FString PersonalityName;

    // Description of this personality
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
    FString Description;

    // Primary traits that define this personality
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
    TArray<ENPCTrait> PrimaryTraits;

    // Secondary traits
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
    TArray<ENPCTrait> SecondaryTraits;

    // Personality modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
    FNPCPersonalityModifiers Modifiers;

    // Dialogue style hints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> GreetingLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> AngryLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> FriendlyLines;

    // Preferred activities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activities")
    TArray<FString> PreferredActivities;

    // Disliked activities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activities")
    TArray<FString> DislikedActivities;

    // Get a random greeting line
    UFUNCTION(BlueprintPure, Category = "Personality")
    FString GetRandomGreeting() const
    {
        if (GreetingLines.Num() > 0)
        {
            return GreetingLines[FMath::RandRange(0, GreetingLines.Num() - 1)];
        }
        return TEXT("Hello there.");
    }

    // Get a random angry line
    UFUNCTION(BlueprintPure, Category = "Personality")
    FString GetRandomAngryLine() const
    {
        if (AngryLines.Num() > 0)
        {
            return AngryLines[FMath::RandRange(0, AngryLines.Num() - 1)];
        }
        return TEXT("This is unacceptable!");
    }

    // Get a random friendly line
    UFUNCTION(BlueprintPure, Category = "Personality")
    FString GetRandomFriendlyLine() const
    {
        if (FriendlyLines.Num() > 0)
        {
            return FriendlyLines[FMath::RandRange(0, FriendlyLines.Num() - 1)];
        }
        return TEXT("It's good to see you.");
    }

    // Check if personality has a specific trait
    UFUNCTION(BlueprintPure, Category = "Personality")
    bool HasTrait(ENPCTrait Trait) const
    {
        return PrimaryTraits.Contains(Trait) || SecondaryTraits.Contains(Trait);
    }

    // Get trait strength (1.0 for primary, 0.5 for secondary, 0 for none)
    UFUNCTION(BlueprintPure, Category = "Personality")
    float GetTraitStrength(ENPCTrait Trait) const
    {
        if (PrimaryTraits.Contains(Trait))
        {
            return 1.0f;
        }
        else if (SecondaryTraits.Contains(Trait))
        {
            return 0.5f;
        }
        return 0.0f;
    }
};