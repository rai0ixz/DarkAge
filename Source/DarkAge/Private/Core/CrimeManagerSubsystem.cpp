#include "Core/CrimeManagerSubsystem.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "GameFramework/Actor.h"
#include "Components/NotorietyComponent.h"
#include "UObject/UObjectGlobals.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Misc/Paths.h"


UCrimeManagerSubsystem::UCrimeManagerSubsystem()
{
	UE_LOG(LogTemp, Log, TEXT("CrimeManagerSubsystem Constructor Called."));
}

void UCrimeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FString ResetFlagPath = FPaths::ProjectSavedDir() + TEXT("ResetCrimes.flag");
	if (IFileManager::Get().FileExists(*ResetFlagPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetCrimes.flag found. Deleting Crimes.sav and re-initializing."));
		IFileManager::Get().Delete(*(FPaths::ProjectSavedDir() + TEXT("Crimes.sav")));
		IFileManager::Get().Delete(*ResetFlagPath);
	}

	LoadCrimes();
	UE_LOG(LogTemp, Log, TEXT("CrimeManagerSubsystem Initialized. Loaded %d crimes from save."), ReportedCrimes.Num());
	
}

void UCrimeManagerSubsystem::Deinitialize()
{
	SaveCrimes();
	UE_LOG(LogTemp, Log, TEXT("CrimeManagerSubsystem Stopped."));
	Super::Deinitialize();
}

void UCrimeManagerSubsystem::ReportCrime(
	AActor* Perpetrator,
	AActor* Witness,
	ECrimeType CrimeCommitted,
	float BaseNotorietyValue,
	const FVector& CrimeLocation,
	const FString& RegionID)
{
	if (!Perpetrator)
	{
		UE_LOG(LogTemp, Warning, TEXT("CrimeManagerSubsystem::ReportCrime - Perpetrator is null."));
		return;
	}

	FCrimeData NewCrime;
	NewCrime.CrimeId = FGuid::NewGuid();
	NewCrime.Perpetrator = Perpetrator;
	if (Witness)
	{
		NewCrime.Witnesses.Add(Witness);
	}
	NewCrime.CrimeType = CrimeCommitted;
	NewCrime.CrimeLocation = CrimeLocation;
	NewCrime.Timestamp = FDateTime::UtcNow();
	NewCrime.Status = ECrimeStatus::Reported;

	ReportedCrimes.Add(NewCrime.CrimeId, NewCrime);

	const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/DarkAge.ECrimeType"), true);
	FString CrimeString = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CrimeCommitted)) : TEXT("UnknownCrime");
	FString WitnessName = Witness ? Witness->GetName() : TEXT("UnknownWitness");

	UE_LOG(LogTemp, Warning, TEXT("CrimeManagerSubsystem: CRIME REPORT! ID: %s, Perpetrator: %s, Witness: %s, Crime: %s, Location: %s, Region: %s, BaseNotoriety: %.2f"),
		*NewCrime.CrimeId.ToString(),
		*Perpetrator->GetName(),
		*WitnessName,
		*CrimeString,
		*CrimeLocation.ToString(),
		*RegionID,
		BaseNotorietyValue);

	UNotorietyComponent* NotorietyComp = Perpetrator->FindComponentByClass<UNotorietyComponent>();
	if (NotorietyComp)
	{
		NotorietyComp->RecordCrime(CrimeCommitted, BaseNotorietyValue, RegionID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CrimeManagerSubsystem::ReportCrime - Perpetrator %s does not have a NotorietyComponent."), *Perpetrator->GetName());
	}
}

bool UCrimeManagerSubsystem::GetCrimeData(const FGuid& CrimeId, FCrimeData& OutCrimeData) const
{
	if (const FCrimeData* FoundCrime = ReportedCrimes.Find(CrimeId))
	{
		OutCrimeData = *FoundCrime;
		return true;
	}
	return false;
}

void UCrimeManagerSubsystem::UpdateCrimeStatus(const FGuid& CrimeId, ECrimeStatus NewStatus)
{
	if (FCrimeData* CrimeToUpdate = ReportedCrimes.Find(CrimeId))
	{
		CrimeToUpdate->Status = NewStatus;
		UE_LOG(LogTemp, Log, TEXT("Crime ID %s status updated to %s"), *CrimeId.ToString(), *UEnum::GetValueAsString(NewStatus));
	}
}

void UCrimeManagerSubsystem::AddWitness(const FGuid& CrimeId, AActor* Witness)
{
	if (FCrimeData* CrimeToUpdate = ReportedCrimes.Find(CrimeId))
	{
		if (Witness)
		{
			CrimeToUpdate->Witnesses.AddUnique(Witness);
			UE_LOG(LogTemp, Log, TEXT("Witness %s added to crime %s"), *Witness->GetName(), *CrimeId.ToString());
		}
	}
}

void UCrimeManagerSubsystem::ReportEvidence(const FGuid& CrimeId, const FString& EvidenceDescription, AActor* AssociatedActor)
{
	if (FCrimeData* CrimeToUpdate = ReportedCrimes.Find(CrimeId))
	{
		FEvidence NewEvidence;
		NewEvidence.EvidenceId = FGuid::NewGuid();
		NewEvidence.Description = EvidenceDescription;
		NewEvidence.AssociatedActor = AssociatedActor;
		CrimeToUpdate->Evidence.Add(NewEvidence.EvidenceId, NewEvidence);
		UE_LOG(LogTemp, Log, TEXT("Evidence reported for crime %s: %s"), *CrimeId.ToString(), *EvidenceDescription);
	}
}

void UCrimeManagerSubsystem::PlaceBounty(const FGuid& CrimeId, int32 Amount, AActor* BountyPlacer)
{
	if (FCrimeData* CrimeToUpdate = ReportedCrimes.Find(CrimeId))
	{
		CrimeToUpdate->Bounty.Amount = Amount;
		CrimeToUpdate->Bounty.BountyPlacer = BountyPlacer;
		CrimeToUpdate->Bounty.IsActive = true;
		UE_LOG(LogTemp, Log, TEXT("Bounty of %d placed on perpetrator of crime %s by %s"), Amount, *CrimeId.ToString(), *BountyPlacer->GetName());
	}
}

            
TArray<FCrimeData> UCrimeManagerSubsystem::GetCrimesByPerpetrator(AActor* Perpetrator) const
{
    TArray<FCrimeData> FoundCrimes;
    for (const auto& CrimePair : ReportedCrimes)
    {
        if (CrimePair.Value.Perpetrator == Perpetrator)
        {
            FoundCrimes.Add(CrimePair.Value);
        }
    }
    return FoundCrimes;
}

TArray<FCrimeData> UCrimeManagerSubsystem::GetCrimesByStatus(ECrimeStatus Status) const
{
    TArray<FCrimeData> FoundCrimes;
    for (const auto& CrimePair : ReportedCrimes)
    {
        if (CrimePair.Value.Status == Status)
        {
            FoundCrimes.Add(CrimePair.Value);
        }
    }
    return FoundCrimes;
}

TArray<FCrimeData> UCrimeManagerSubsystem::GetCrimesByRegion(const FString& RegionID) const
{
    // This function assumes that you have a way to associate a crime with a region.
    // For this example, we'll just return all crimes.
    TArray<FCrimeData> FoundCrimes;
    ReportedCrimes.GenerateValueArray(FoundCrimes);
    return FoundCrimes;
}

TArray<FBounty> UCrimeManagerSubsystem::GetBounties() const
{
    TArray<FBounty> Bounties;
    for (const auto& CrimePair : ReportedCrimes)
    {
        if (CrimePair.Value.Bounty.IsActive)
        {
            Bounties.Add(CrimePair.Value.Bounty);
        }
    }
    return Bounties;
}

TArray<AActor*> UCrimeManagerSubsystem::GetWitnesses(const FGuid& CrimeId) const
{
    TArray<AActor*> Result;
    if (const FCrimeData* FoundCrime = ReportedCrimes.Find(CrimeId))
    {
        for (const TWeakObjectPtr<AActor>& WitnessPtr : FoundCrime->Witnesses)
        {
            if (WitnessPtr.IsValid())
            {
                Result.Add(WitnessPtr.Get());
            }
        }
    }
    return Result;
}

TArray<FEvidence> UCrimeManagerSubsystem::GetEvidence(const FGuid& CrimeId) const
{
    TArray<FEvidence> Evidence;
    if (const FCrimeData* FoundCrime = ReportedCrimes.Find(CrimeId))
    {
        FoundCrime->Evidence.GenerateValueArray(Evidence);
    }
    return Evidence;
}

void UCrimeManagerSubsystem::SaveCrimes()
{
    TArray<uint8> SaveData;
    FMemoryWriter MemoryWriter(SaveData, true);
    
    int32 CrimeCount = ReportedCrimes.Num();
    MemoryWriter << CrimeCount;

    for (const auto& CrimePair : ReportedCrimes)
    {
        FGuid CrimeId = CrimePair.Key;
        FCrimeData CrimeData = CrimePair.Value;
        MemoryWriter << CrimeId;
        //MemoryWriter << CrimeData;
    }

    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Crimes.sav");
    FFileHelper::SaveArrayToFile(SaveData, *SavePath);
}

void UCrimeManagerSubsystem::LoadCrimes()
{
    TArray<uint8> LoadData;
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Crimes.sav");
    if (FFileHelper::LoadFileToArray(LoadData, *SavePath))
    {
        FMemoryReader MemoryReader(LoadData, true);
        
        int32 CrimeCount;
        MemoryReader << CrimeCount;

        for (int32 i = 0; i < CrimeCount; ++i)
        {
            FGuid CrimeId;
            FCrimeData CrimeData;
            MemoryReader << CrimeId;
            //MemoryReader << CrimeData;
            ReportedCrimes.Add(CrimeId, CrimeData);
        }
    }
}