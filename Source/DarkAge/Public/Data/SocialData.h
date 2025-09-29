
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"



#include "SocialData.generated.h"

// Forward declarations
class UPlayerStateComponent;
class ADAAIBaseCharacter;

/**
 * Social Relationship Types
 */
UENUM(BlueprintType)
enum class ESocialRelationshipType : uint8
{
    Stranger,
    Acquaintance,
    Friend,
    Close_Friend,
    Family,
    Romantic_Interest,
    Spouse,
    Enemy,
    Rival,
    Ally,
    Business_Partner,
    Political_Ally,
    Political_Enemy
};

/**
 * Social Relationship Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FSocialRelationship
{
    GENERATED_BODY()

    FSocialRelationship()
        : RelationshipType(ESocialRelationshipType::Stranger)
        , RelationshipStrength(0.0f)
        , Trust(0.0f)
        , Respect(0.0f)
        , Fear(0.0f)
        , Attraction(0.0f)
        , LastInteractionTime(0.0f)
        , InteractionCount(0)
        , bIsPublicRelationship(true)
    {
    }

    // Type of relationship
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    ESocialRelationshipType RelationshipType;

    // Overall strength of the relationship (-100 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100.0", ClampMax = "100.0"))
    float RelationshipStrength;

    // Trust level (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Trust;

    // Respect level (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Respect;

    // Fear level (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Fear;

    // Attraction level (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Attraction;

    // When this relationship was last updated
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relationship")
    float LastInteractionTime;

    // Number of interactions between these characters
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relationship")
    int32 InteractionCount;

    // Whether this relationship is known to others
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    bool bIsPublicRelationship;

    // History of major events affecting this relationship
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    TArray<FString> RelationshipHistory;
};

/**
 * Faction Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FFactionData
{
    GENERATED_BODY()

    FFactionData()
        : FactionName(TEXT("Unknown"))
        , FactionDescription(TEXT(""))
        , PowerLevel(50.0f)
        , Influence(50.0f)
        , PublicReputation(0.0f)
        , bIsHostileToPlayer(false)
        , bIsPublicFaction(true)
    {
    }

    // Name of the faction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FString FactionName;

    // Description of the faction's goals and beliefs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FString FactionDescription;

    // Overall power level of the faction (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float PowerLevel;

    // Political influence level (0 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Influence;

    // Public reputation (-100 to 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction", meta = (ClampMin = "-100.0", ClampMax = "100.0"))
    float PublicReputation;

    // Whether this faction is hostile to the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    bool bIsHostileToPlayer;

    // Whether this faction is publicly known
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    bool bIsPublicFaction;

    // List of allied faction IDs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    TArray<FName> AlliedFactions;

    // List of enemy faction IDs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    TArray<FName> EnemyFactions;

    // List of member character IDs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    TArray<FString> MemberCharacters;
};

/**
 * Social Event Data
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FSocialEvent
{
    GENERATED_BODY()

    FSocialEvent() = default;

    // Type of social event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    FName EventType = FName("SocialEvent");

    // Description of what happened
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    FString EventDescription = TEXT("");

    // Severity of the event (affects how much it impacts relationships)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float EventSeverity = 1.0f;

    // When the event occurred
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    float EventTime = 0.0f;

    // Location where the event occurred
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    FVector EventLocation = FVector::ZeroVector;

    // Characters directly involved in the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    TArray<FString> InvolvedCharacters;

    // Characters who witnessed the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    TArray<FString> WitnessCharacters;

    // Whether this event should spread to other characters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event")
    bool bShouldPropagate = true;

    // How far the event should spread
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event", meta = (ClampMin = "0.0"))
    float PropagationRadius = 1000.0f;

    // How much the event impact decays over distance/time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Social Event", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PropagationDecay = 0.1f;
};

// ADDED WRAPPER STRUCT HERE
USTRUCT(BlueprintType)
struct FFNameArrayWrapper
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Social Data") // Or EditAnywhere depending on usage
        TArray<FName> FactionIDs;
};