#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/NPCEcosystemData.h"
#include "Engine/World.h"
#include "NPCEcosystemSubsystem.generated.h"

// Forward declarations
UENUM(BlueprintType)
enum class ESettlementType : uint8
{
    Village,
    Town,
    City,
    Port
};

UENUM(BlueprintType)
enum class ESettlementSpecialization : uint8
{
    Agriculture,
    Mining,
    Crafting,
    Trade,
    Fishing
};

USTRUCT(BlueprintType)
struct FSettlementData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SettlementName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESettlementType SettlementType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Population;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Prosperity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Security;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Happiness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ResourceAvailability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ESettlementSpecialization> Specializations;

    FSettlementData()
    {
        SettlementName = TEXT("");
        Location = FVector::ZeroVector;
        SettlementType = ESettlementType::Village;
        Population = 0;
        Prosperity = 0.5f;
        Security = 0.5f;
        Happiness = 0.5f;
        ResourceAvailability = 0.5f;
    }
};

/**
 * Manages the lifecycle and interactions of NPCs in the world.
 */
UCLASS()
class DARKAGE_API UNPCEcosystemSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UNPCEcosystemSubsystem();

    /** If false, the entire NPC ecosystem simulation update will be skipped. Can be toggled with the 'ToggleNPCEcosystem' console command. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Ecosystem")
    bool bNPCEcosystemEnabled = true;

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    void Tick(float DeltaTime);

private:
    // Initialization methods
    void InitializeSettlements();
    void InitializeNPCArchetypes();
    void InitializeSocialNetworks();

    // Settlement management
    void CreateSettlement(const FString& Name, const FVector& Location, ESettlementType Type, int32 InitialPopulation);
    void PopulateSettlement(const FString& SettlementName, int32 PopulationCount);

    // NPC generation and management
    FNPCData GenerateNPC(const FString& SettlementName, ESettlementType SettlementType);
    FString GenerateNPCName();
    ENPCProfession AssignProfession(ESettlementType SettlementType);
    void GenerateDailySchedule(FNPCData& NPC);

    // Social system
    void CreateSocialConnections(FNPCData& NPC);
    ERelationshipType DetermineRelationshipType(const FNPCData& NPC1, const FNPCData& NPC2);
    float CalculatePersonalityCompatibility(const FNPCData& NPC1, const FNPCData& NPC2);

    // Settlement analysis
    void UpdateSettlementConditions();
    void UpdatePopulationDynamics();
    void ProcessNPCLifeEvents();
    float CalculateEconomicFactor(const FSettlementData& Settlement);
    float CalculateSecurityFactor(const FSettlementData& Settlement);

    // Update methods
    void UpdateEcosystem();
    void UpdateSocialInteractions();
    void UpdateMigrationPatterns();
    void UpdateNPCBehaviors(float DeltaTime);
    
    // Additional helper methods
    void ProcessLifeEvent(FNPCData& NPC);
    void UpdateNPCNeeds(FNPCData& NPC);
    void UpdateNPCRelationships(FNPCData& NPC);
    void ProcessSocialInteraction(FNPCData& NPC, FNPCRelationship& Relationship);
    bool ShouldNPCMigrate(const FNPCData& NPC);
    void ProcessNPCMigration(const FGuid& NPCID);
    void UpdateNPCActivity(FNPCData& NPC, float DeltaTime);

    // Member variables
    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    float PopulationGrowthRate;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    int32 MaxPopulationPerSettlement;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    float MigrationThreshold;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    float SocialInteractionRadius;

    // Timer variables
    float EcosystemUpdateTimer;
    float SocialUpdateTimer;
    float MigrationUpdateTimer;

    // Data containers
    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    TMap<FGuid, FNPCData> NPCPopulation;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    TMap<FString, FSettlementData> Settlements;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    TMap<FString, FString> SocialNetworks;

    UPROPERTY(VisibleAnywhere, Category = "NPC Ecosystem")
    TMap<FString, FNPCArchetype> NPCArchetypes;
};