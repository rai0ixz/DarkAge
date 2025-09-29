// DiseaseData.h (FINAL CORRECTED VERSION)
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/EnvironmentalFactorsData.h"
#include "DiseaseData.generated.h"

// Forward declarations
class UTexture2D;
class UAnimMontage;
class USoundBase;

UENUM(BlueprintType)
enum class EDiseaseTransmissionMethod : uint8
{
    None            UMETA(DisplayName = "None"),
    Airborne        UMETA(DisplayName = "Airborne"),
    Contact         UMETA(DisplayName = "Contact"),
    Contamination   UMETA(DisplayName = "Contamination"),
    Vector          UMETA(DisplayName = "Vector"),
    Magical         UMETA(DisplayName = "Magical"),
    Environmental   UMETA(DisplayName = "Environmental")
};

UENUM(BlueprintType)
enum class EDiseaseSeverity : uint8
{
    DS_None     UMETA(DisplayName = "None"),
    Mild        UMETA(DisplayName = "Mild"),
    Moderate    UMETA(DisplayName = "Moderate"),
    Severe      UMETA(DisplayName = "Severe"),
    Critical    UMETA(DisplayName = "Critical"),
    Fatal       UMETA(DisplayName = "Fatal")
};

USTRUCT(BlueprintType)
struct DARKAGE_API FDiseaseStage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    FText StageName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage", meta = (ClampMin = "0.0"))
    float Duration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    EDiseaseSeverity Severity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    TArray<FName> SymptomEffects;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage", meta = (ClampMin = "0.0"))
    float TransmissionRateModifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    bool bIsContagious;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    bool bCanProgress;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Stage")
    bool bCanBeDetected;

    FDiseaseStage()
        : Duration(0.0f)
        , Severity(EDiseaseSeverity::Mild)
        , TransmissionRateModifier(1.0f)
        , bIsContagious(false)
        , bCanProgress(true)
        , bCanBeDetected(true)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FDiseaseTreatment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    FName TreatmentID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    FText TreatmentName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    TArray<FName> RequiredItems;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float EffectivenessRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment", meta = (ClampMin = "0.0"))
    float TreatmentDuration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    bool bCanCureCompletely;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    bool bRequiresSpecialist;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment", meta = (ClampMin = "0"))
    int32 Cost;

    FDiseaseTreatment()
        : EffectivenessRate(1.0f)
        , TreatmentDuration(0.0f)
        , bCanCureCompletely(true)
        , bRequiresSpecialist(false)
        , Cost(0)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FDiseaseData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FName DiseaseID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FText DiseaseName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
    EDiseaseTransmissionMethod TransmissionMethod;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BaseTransmissionRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (EditCondition = "TransmissionMethod == EDiseaseTransmissionMethod::Airborne || TransmissionMethod == EDiseaseTransmissionMethod::Contact", ClampMin = "0.0"))
    float TransmissionRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "0.0"))
    float IncubationPeriod;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    TArray<FDiseaseStage> Stages;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treatment")
    TArray<FDiseaseTreatment> AvailableTreatments;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity", meta = (ClampMin = "0.0"))
    float ImmunityDuration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    bool bCanDevelopImmunity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Severity")
    bool bIsFatal;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    bool bIsChronicCondition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
    bool bAffectsNPCs;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
    bool bAffectsAnimals;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prevention")
    TArray<FText> PreventionMethods;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TArray<FName> AssociatedStatusEffects;

    FDiseaseData()
        : TransmissionMethod(EDiseaseTransmissionMethod::None)
        , BaseTransmissionRate(0.0f)
        , TransmissionRadius(100.0f)
        , IncubationPeriod(0.0f)
        , ImmunityDuration(0.0f)
        , bCanDevelopImmunity(false)
        , bIsFatal(false)
        , bIsChronicCondition(false)
        , bAffectsNPCs(true)
        , bAffectsAnimals(false)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FActiveDiseaseInstance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    FDataTableRowHandle DiseaseDataRowHandle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    FName DiseaseID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    int32 CurrentStage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    float TimeInCurrentStage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    float TotalDiseaseTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    float ContractionTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    FString SourceActorID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    bool bIsIncubating;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    bool bIsDetected;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    bool bIsActive;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    TArray<FName> AppliedStatusEffects;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease Instance")
    bool bIsContagious;
   
    FActiveDiseaseInstance()
    	: CurrentStage(0)
    	, TimeInCurrentStage(0.0f)
    	, TotalDiseaseTime(0.0f)
    	, ContractionTime(0.0f)
    	, bIsIncubating(true)
    	, bIsDetected(false)
    	, bIsActive(true)
    	, bIsContagious(false)
    {
    }

    bool operator==(const FActiveDiseaseInstance& Other) const
    {
        return DiseaseID == Other.DiseaseID
            && SourceActorID == Other.SourceActorID
            && CurrentStage == Other.CurrentStage
            && FMath::IsNearlyEqual(TimeInCurrentStage, Other.TimeInCurrentStage)
            && FMath::IsNearlyEqual(TotalDiseaseTime, Other.TotalDiseaseTime)
            && FMath::IsNearlyEqual(ContractionTime, Other.ContractionTime)
            && bIsIncubating == Other.bIsIncubating
            && bIsDetected == Other.bIsDetected
            && bIsActive == Other.bIsActive
            && AppliedStatusEffects == Other.AppliedStatusEffects;
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FDiseaseImmunityRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    FName DiseaseID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    float ImmunityStartTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    float ImmunityDuration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Immunity")
    bool bIsPermanent;

    FDiseaseImmunityRecord()
        : ImmunityStartTime(0.0f)
        , ImmunityDuration(0.0f)
        , bIsPermanent(false)
    {
    }
};