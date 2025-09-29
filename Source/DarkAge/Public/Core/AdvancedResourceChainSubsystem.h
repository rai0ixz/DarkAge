#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "AdvancedResourceChainSubsystem.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    RawMaterial     UMETA(DisplayName = "Raw Material"),
    ProcessedGood   UMETA(DisplayName = "Processed Good"),
    ManufacturedItem UMETA(DisplayName = "Manufactured Item"),
    LuxuryGood      UMETA(DisplayName = "Luxury Good"),
    Tool            UMETA(DisplayName = "Tool"),
    Weapon          UMETA(DisplayName = "Weapon"),
    Food            UMETA(DisplayName = "Food"),
    Medicine        UMETA(DisplayName = "Medicine"),
    Knowledge       UMETA(DisplayName = "Knowledge"),
    Service         UMETA(DisplayName = "Service")
};

UENUM(BlueprintType)
enum class EProductionMethod : uint8
{
    Manual          UMETA(DisplayName = "Manual Labor"),
    Crafting        UMETA(DisplayName = "Skilled Crafting"),
    Manufacturing   UMETA(DisplayName = "Mass Manufacturing"),
    Alchemy         UMETA(DisplayName = "Alchemical Process"),
    Magic           UMETA(DisplayName = "Magical Creation"),
    Agriculture     UMETA(DisplayName = "Agricultural"),
    Mining          UMETA(DisplayName = "Mining"),
    Hunting         UMETA(DisplayName = "Hunting/Gathering")
};

UENUM(BlueprintType)
enum class EResourceQuality : uint8
{
    Poor            UMETA(DisplayName = "Poor"),
    Common          UMETA(DisplayName = "Common"),
    Good            UMETA(DisplayName = "Good"),
    Excellent       UMETA(DisplayName = "Excellent"),
    Masterwork      UMETA(DisplayName = "Masterwork"),
    Legendary       UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FResourceNode
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    FString ResourceID;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    FString ResourceName;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    EResourceType ResourceType = EResourceType::RawMaterial;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    EResourceQuality BaseQuality = EResourceQuality::Common;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    float BaseValue = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    float Weight = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    float Durability = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    TMap<FString, float> Properties;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    bool bIsRenewable = true;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    float RenewalRate = 1.0f;

    FResourceNode()
    {
        ResourceID = TEXT("");
        ResourceName = TEXT("");
        ResourceType = EResourceType::RawMaterial;
        BaseQuality = EResourceQuality::Common;
        BaseValue = 10.0f;
        Weight = 1.0f;
        Durability = 100.0f;
        bIsRenewable = true;
        RenewalRate = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FResourceRequirement
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Resource Requirement")
    FString ResourceID;

    UPROPERTY(BlueprintReadOnly, Category = "Resource Requirement")
    int32 Quantity = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Resource Requirement")
    EResourceQuality MinQuality = EResourceQuality::Common;

    UPROPERTY(BlueprintReadOnly, Category = "Resource Requirement")
    bool bIsOptional = false;

    UPROPERTY(BlueprintReadOnly, Category = "Resource Requirement")
    float QualityMultiplier = 1.0f;

    FResourceRequirement()
    {
        ResourceID = TEXT("");
        Quantity = 1;
        MinQuality = EResourceQuality::Common;
        bIsOptional = false;
        QualityMultiplier = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FProductionRecipe
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    FString RecipeID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    FString RecipeName;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    FString OutputResourceID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    int32 OutputQuantity = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    TArray<FResourceRequirement> InputRequirements;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    EProductionMethod ProductionMethod = EProductionMethod::Manual;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    float ProductionTime = 60.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    float SkillRequirement = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    TArray<FString> RequiredTools;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    TArray<FString> RequiredFacilities;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    float SuccessRate = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    float QualityVariance = 0.2f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Recipe")
    TMap<FString, float> EnvironmentalModifiers;

    FProductionRecipe()
    {
        RecipeID = TEXT("");
        RecipeName = TEXT("");
        OutputResourceID = TEXT("");
        OutputQuantity = 1;
        ProductionMethod = EProductionMethod::Manual;
        ProductionTime = 60.0f;
        SkillRequirement = 0.0f;
        SuccessRate = 0.8f;
        QualityVariance = 0.2f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FProductionFacility
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    FString FacilityID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    FString FacilityName;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    FVector Location;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    TArray<EProductionMethod> SupportedMethods;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    float EfficiencyMultiplier = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    float QualityBonus = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    int32 MaxConcurrentProductions = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    TArray<FString> ActiveProductions;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    TMap<FString, int32> StoredResources;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    int32 StorageCapacity = 1000;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    float MaintenanceCost = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Facility")
    float Condition = 1.0f;

    FProductionFacility()
    {
        FacilityID = TEXT("");
        FacilityName = TEXT("");
        Location = FVector::ZeroVector;
        EfficiencyMultiplier = 1.0f;
        QualityBonus = 0.0f;
        MaxConcurrentProductions = 1;
        StorageCapacity = 1000;
        MaintenanceCost = 10.0f;
        Condition = 1.0f;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FProductionOrder
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    FString OrderID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    FString RecipeID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    FString FacilityID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    FString WorkerID;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    int32 Quantity = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    float EstimatedCompletionTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    float Progress = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Production Order")
    TMap<FString, int32> AllocatedResources;

    FProductionOrder()
    {
        OrderID = TEXT("");
        RecipeID = TEXT("");
        FacilityID = TEXT("");
        WorkerID = TEXT("");
        Quantity = 1;
        StartTime = 0.0f;
        EstimatedCompletionTime = 0.0f;
        Progress = 0.0f;
        bIsActive = false;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FSupplyChain
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    FString ChainID;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    FString ChainName;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    TArray<FString> ProductionSteps;

    // Note: TMap with TArray values cannot be exposed to Blueprint
    TMap<FString, TArray<FString>> Dependencies;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    float Efficiency = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    float Stability = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    TArray<FString> Bottlenecks;

    UPROPERTY(BlueprintReadOnly, Category = "Supply Chain")
    float TotalLeadTime = 0.0f;

    FSupplyChain()
    {
        ChainID = TEXT("");
        ChainName = TEXT("");
        Efficiency = 1.0f;
        Stability = 1.0f;
        TotalLeadTime = 0.0f;
    }
};

// Resource Chain Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionCompleted, const FString&, OrderID, const FString&, OutputResourceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceShortage, const FString&, ResourceID, const FString&, FacilityID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSupplyChainDisrupted, const FString&, ChainID, float, DisruptionSeverity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNewRecipeDiscovered, const FString&, RecipeID, const FString&, DiscovererID, EResourceType, OutputType);

UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UAdvancedResourceChainSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UAdvancedResourceChainSubsystem() {}

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAdvancedResourceChainSubsystem, STATGROUP_Tickables); }

    // Resource Chain Delegates
    UPROPERTY(BlueprintAssignable, Category = "Resource Chain Events")
    FOnProductionCompleted OnProductionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Resource Chain Events")
    FOnResourceShortage OnResourceShortage;

    UPROPERTY(BlueprintAssignable, Category = "Resource Chain Events")
    FOnSupplyChainDisrupted OnSupplyChainDisrupted;

    UPROPERTY(BlueprintAssignable, Category = "Resource Chain Events")
    FOnNewRecipeDiscovered OnNewRecipeDiscovered;

    // Public Interface Functions
    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    FString StartProduction(const FString& RecipeID, const FString& FacilityID, const FString& WorkerID, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    bool CancelProduction(const FString& OrderID);

    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    void CreateProductionFacility(const FString& FacilityName, const FVector& Location, const TArray<EProductionMethod>& SupportedMethods);

    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    void UpgradeFacility(const FString& FacilityID, float EfficiencyIncrease, float QualityIncrease);

    UFUNCTION(BlueprintPure, Category = "Resource Chain")
    TArray<FProductionRecipe> GetAvailableRecipes(const FString& FacilityID) const;

    UFUNCTION(BlueprintPure, Category = "Resource Chain")
    TArray<FProductionOrder> GetActiveProductions() const;

    UFUNCTION(BlueprintPure, Category = "Resource Chain")
    FSupplyChain AnalyzeSupplyChain(const FString& ResourceID) const;

    UFUNCTION(BlueprintPure, Category = "Resource Chain")
    TArray<FString> FindResourceBottlenecks() const;

    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    void OptimizeSupplyChains();

    UFUNCTION(BlueprintCallable, Category = "Resource Chain")
    bool DiscoverNewRecipe(const FString& DiscovererID, const TArray<FString>& ExperimentalInputs);

protected:
    // Resource system data
    UPROPERTY()
    TMap<FString, FResourceNode> ResourceCatalog;

    UPROPERTY()
    TMap<FString, FProductionRecipe> ProductionRecipes;

    UPROPERTY()
    TMap<FString, FProductionFacility> ProductionFacilities;

    UPROPERTY()
    TArray<FProductionOrder> ActiveProductionOrders;

    UPROPERTY()
    TMap<FString, FSupplyChain> SupplyChains;

    // Global resource tracking
    UPROPERTY()
    TMap<FString, int32> GlobalResourceSupply;

    UPROPERTY()
    TMap<FString, int32> GlobalResourceDemand;

    UPROPERTY()
    TMap<FString, float> ResourcePriceHistory;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Configuration")
    float ProductionUpdateInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Configuration")
    float SupplyChainAnalysisInterval = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Configuration")
    bool bEnableRecipeDiscovery = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Configuration")
    float RecipeDiscoveryChance = 0.05f;

    // Internal state
    float ProductionUpdateTimer = 0.0f;
    float SupplyChainAnalysisTimer = 0.0f;
    int32 NextOrderID = 1;
    int32 NextFacilityID = 1;

    // Initialization functions
    void InitializeResourceCatalog();
    void InitializeProductionRecipes();
    void InitializeProductionFacilities();
    void InitializeSupplyChains();

    // Production management functions
    void UpdateProductionOrders(float DeltaTime);
    void ProcessCompletedProductions();
    bool AllocateResourcesForProduction(FProductionOrder& Order);
    void ReleaseAllocatedResources(const FProductionOrder& Order);

    // Supply chain management functions
    void AnalyzeSupplyChains();
    void UpdateSupplyChainMetrics();
    void DetectSupplyChainDisruptions();
    void OptimizeProductionScheduling();

    // Resource management functions
    void UpdateGlobalResourceMetrics();
    void ProcessResourceRenewal();
    void HandleResourceShortages();
    void BalanceSupplyAndDemand();

    // Recipe discovery system
    void ProcessRecipeDiscovery();
    FProductionRecipe GenerateNewRecipe(const TArray<FString>& Inputs, const FString& DiscovererID);
    bool ValidateRecipeInputs(const TArray<FString>& Inputs);

    // Quality and efficiency systems
    EResourceQuality CalculateOutputQuality(const FProductionRecipe& Recipe, const FProductionFacility& Facility, float WorkerSkill);
    float CalculateProductionEfficiency(const FProductionRecipe& Recipe, const FProductionFacility& Facility);
    void ApplyEnvironmentalModifiers(FProductionOrder& Order);

    // Economic integration functions
    void UpdateResourcePrices();
    float CalculateResourceValue(const FString& ResourceID, EResourceQuality Quality);
    void IntegrateWithMarketSystem();

    // Utility functions
    FString GenerateUniqueOrderID();
    FString GenerateUniqueFacilityID();
    FString GenerateUniqueRecipeID();
    bool HasSufficientResources(const TArray<FResourceRequirement>& Requirements, const FString& FacilityID);
    void ConsumeResources(const TArray<FResourceRequirement>& Requirements, const FString& FacilityID);
    void ProduceResources(const FString& ResourceID, int32 Quantity, EResourceQuality Quality, const FString& FacilityID);
};