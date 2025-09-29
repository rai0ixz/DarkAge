#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/CrimeTypes.h"
#include "CrimeManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECrimeStatus : uint8
{
    Reported,
    UnderInvestigation,
    Witnessed,
    Unsolved,
    Resolved
};

USTRUCT(BlueprintType)
struct FEvidence
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FGuid EvidenceId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    TWeakObjectPtr<AActor> AssociatedActor;
};

USTRUCT(BlueprintType)
struct FBounty
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    int32 Amount = 0;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    TWeakObjectPtr<AActor> BountyPlacer;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    bool IsActive = false;
};

USTRUCT(BlueprintType)
struct FCrimeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FGuid CrimeId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    TWeakObjectPtr<AActor> Perpetrator;

    TArray<TWeakObjectPtr<AActor>> Witnesses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    ECrimeType CrimeType = ECrimeType::None;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FVector CrimeLocation = FVector::ZeroVector;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FDateTime Timestamp;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    ECrimeStatus Status = ECrimeStatus::Reported;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    TMap<FGuid, FEvidence> Evidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    FBounty Bounty;
};

UCLASS()
class DARKAGE_API UCrimeManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UCrimeManagerSubsystem();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void ReportCrime(AActor* Perpetrator, AActor* Witness, ECrimeType CrimeCommitted, float BaseNotorietyValue, const FVector& CrimeLocation, const FString& RegionID);
    bool GetCrimeData(const FGuid& CrimeId, FCrimeData& OutCrimeData) const;
    void AddWitness(const FGuid& CrimeId, AActor* Witness);
    void ReportEvidence(const FGuid& CrimeId, const FString& EvidenceDescription, AActor* AssociatedActor);
    void PlaceBounty(const FGuid& CrimeId, int32 Amount, AActor* BountyPlacer);
    void RegisterDebugCommands();
    void UpdateCrimeStatus(const FGuid& CrimeId, ECrimeStatus NewStatus);
    TArray<FCrimeData> GetCrimesByPerpetrator(AActor* Perpetrator) const;
    TArray<FCrimeData> GetCrimesByStatus(ECrimeStatus Status) const;
    TArray<FCrimeData> GetCrimesByRegion(const FString& RegionID) const;
    TArray<FBounty> GetBounties() const;
    TArray<AActor*> GetWitnesses(const FGuid& CrimeId) const;
    TArray<FEvidence> GetEvidence(const FGuid& CrimeId) const;

private:
    UPROPERTY()
    TMap<FGuid, FCrimeData> ReportedCrimes;

    void SaveCrimes();
    void LoadCrimes();
};