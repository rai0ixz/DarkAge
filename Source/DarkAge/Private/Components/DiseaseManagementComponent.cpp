#include "Components/DiseaseManagementComponent.h"
#include "Components/StatusEffectComponent.h"
#include "Components/InventoryComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"

UDiseaseManagementComponent::UDiseaseManagementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDiseaseManagementComponent::BeginPlay()
{
	Super::BeginPlay();
	StatusEffectComponent = GetOwner()->FindComponentByClass<UStatusEffectComponent>();

	if (DiseaseDataTable)
	{
		UE_LOG(LogTemp, Log, TEXT("Disease Data Table loaded."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Disease Data Table not loaded."));
	}
}

void UDiseaseManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 i = ActiveDiseases.Num() - 1; i >= 0; --i)
	{
		FActiveDiseaseInstance& Disease = ActiveDiseases[i];
		if (Disease.bIsActive)
		{
			UpdateDiseaseProgression(Disease, DeltaTime);
		}
		else
		{
			ActiveDiseases.RemoveAt(i);
		}
	}
}

void UDiseaseManagementComponent::Infect(FName DiseaseID)
{
	if (HasImmunity(DiseaseID))
	{
		return;
	}

	if (DiseaseDataTable)
	{
		static const FString ContextString(TEXT("Disease Data Context"));
		FDiseaseData* DiseaseData = DiseaseDataTable->FindRow<FDiseaseData>(DiseaseID, ContextString);

		if (DiseaseData)
		{
			FActiveDiseaseInstance NewDisease;
			NewDisease.DiseaseID = DiseaseID;
			NewDisease.DiseaseDataRowHandle.DataTable = DiseaseDataTable;
			NewDisease.DiseaseDataRowHandle.RowName = DiseaseID;
			NewDisease.bIsActive = true;
			if (UWorld* World = GetWorld())
			{
				NewDisease.ContractionTime = World->GetTimeSeconds();
			}
			NewDisease.bIsContagious = DiseaseData->BaseTransmissionRate > 0;
			ActiveDiseases.Add(NewDisease);
			OnDiseasesChanged.Broadcast();
		}
	}
}

void UDiseaseManagementComponent::TreatDisease(FName DiseaseID, FName TreatmentID)
{
	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	for (int32 i = 0; i < ActiveDiseases.Num(); ++i)
	{
		if (ActiveDiseases[i].DiseaseID == DiseaseID)
		{
			static const FString ContextString(TEXT("Disease Data Context"));
			FDiseaseData* DiseaseData = ActiveDiseases[i].DiseaseDataRowHandle.GetRow<FDiseaseData>(ContextString);
			if (DiseaseData)
			{
				const FDiseaseTreatment* Treatment = DiseaseData->AvailableTreatments.FindByPredicate([TreatmentID](const FDiseaseTreatment& InTreatment)
					{
						return InTreatment.TreatmentID == TreatmentID;
					});

				if (Treatment)
				{
					bool bHasAllItems = true;
					for (const FName& ItemID : Treatment->RequiredItems)
					{
						FItemData ItemData;
						ItemData.ItemID = ItemID;
						if (!Inventory->HasItem(ItemData, 1))
						{
							bHasAllItems = false;
							break;
						}
					}

					if (bHasAllItems && Inventory->CanAfford(Treatment->Cost))
					{
						for (const FName& ItemID : Treatment->RequiredItems)
						{
							FItemData ItemData;
							ItemData.ItemID = ItemID;
							Inventory->RemoveItem(ItemData, 1);
						}
						Inventory->RemoveCurrency(Treatment->Cost);

						if (FMath::FRand() < Treatment->EffectivenessRate)
						{
							if (Treatment->bCanCureCompletely)
							{
								ActiveDiseases[i].bIsActive = false;
								RemoveDiseaseEffects(DiseaseData, ActiveDiseases[i]);
								OnDiseasesChanged.Broadcast();
							}
							else
							{
								// Apply treatment effects, e.g., reduce severity or pause progression
							}
						}
					}
				}
			}
			return;
		}
	}
}

bool UDiseaseManagementComponent::IsInfectedWith(FName DiseaseID) const
{
	return ActiveDiseases.ContainsByPredicate([DiseaseID](const FActiveDiseaseInstance& Disease)
	{
		return Disease.DiseaseID == DiseaseID && Disease.bIsActive;
	});
}

bool UDiseaseManagementComponent::HasImmunity(FName DiseaseID) const
{
	for (const FDiseaseImmunityRecord& Record : ImmunityRecords)
	{
		if (Record.DiseaseID == DiseaseID)
		{
			if (Record.bIsPermanent)
			{
				return true;
			}
			if (UWorld* World = GetWorld())
			{
				if (World->GetTimeSeconds() < Record.ImmunityStartTime + Record.ImmunityDuration)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UDiseaseManagementComponent::UpdateDiseaseProgression(FActiveDiseaseInstance& Disease, float DeltaTime)
{
    static const FString ContextString(TEXT("Disease Data Context"));
    FDiseaseData* DiseaseData = Disease.DiseaseDataRowHandle.GetRow<FDiseaseData>(ContextString);

    if (!DiseaseData)
    {
        return;
    }

    Disease.TotalDiseaseTime += DeltaTime;

    if (Disease.bIsIncubating)
    {
        if (Disease.TotalDiseaseTime >= DiseaseData->IncubationPeriod)
        {
            Disease.bIsIncubating = false;
            Disease.TimeInCurrentStage = 0.0f;
            ApplyDiseaseEffects(DiseaseData, Disease);
        }
    }
    else
    {
        Disease.TimeInCurrentStage += DeltaTime;
        if (DiseaseData->Stages.IsValidIndex(Disease.CurrentStage))
        {
            const FDiseaseStage& CurrentStage = DiseaseData->Stages[Disease.CurrentStage];

            if (Disease.TimeInCurrentStage >= CurrentStage.Duration && CurrentStage.bCanProgress)
            {
                if (Disease.CurrentStage < DiseaseData->Stages.Num() - 1)
                {
                    Disease.CurrentStage++;
                    Disease.TimeInCurrentStage = 0.0f;
                    ApplyDiseaseEffects(DiseaseData, Disease);
                }
                else if (DiseaseData->bIsChronicCondition)
                {
                    // Chronic condition remains in the final stage
                }
                else
                {
                    // Disease has run its course
                    Disease.bIsActive = false;
                    RemoveDiseaseEffects(DiseaseData, Disease);
                    if (DiseaseData->bCanDevelopImmunity)
                    {
                        AddImmunity(Disease.DiseaseID, DiseaseData->ImmunityDuration, false);
                    }
                }
            }
        }
    }

    if (!Disease.bIsDetected)
    {
        AttemptToDetectDisease(Disease);
    }

    if (Disease.bIsContagious)
    {
        TransmitDisease(Disease);
    }
}

void UDiseaseManagementComponent::ApplyDiseaseEffects(const FDiseaseData* DiseaseData, FActiveDiseaseInstance& ActiveDisease)
{
	if (!StatusEffectComponent.IsValid() || !DiseaseData)
	{
		return;
	}

	for (const FName& EffectID : DiseaseData->AssociatedStatusEffects)
	{
		// This is a simplified implementation. A real implementation would use a data table to get status effect data.
		// FDataTableRowHandle EffectHandle;
		// EffectHandle.DataTable = ...;
		// EffectHandle.RowName = EffectID;
		// StatusEffectComponent->ApplyEffect(EffectHandle);
	}
}

void UDiseaseManagementComponent::RemoveDiseaseEffects(const FDiseaseData* DiseaseData, FActiveDiseaseInstance& ActiveDisease)
{
	if (!StatusEffectComponent.IsValid() || !DiseaseData)
	{
		return;
	}

	for (const FName& EffectID : DiseaseData->AssociatedStatusEffects)
	{
		StatusEffectComponent->RemoveEffect(EffectID);
	}
}

void UDiseaseManagementComponent::AttemptToDetectDisease(FActiveDiseaseInstance& Disease)
{
    static const FString ContextString(TEXT("Disease Data Context"));
    FDiseaseData* DiseaseData = Disease.DiseaseDataRowHandle.GetRow<FDiseaseData>(ContextString);
    if (DiseaseData && DiseaseData->Stages.IsValidIndex(Disease.CurrentStage))
    {
        const FDiseaseStage& CurrentStage = DiseaseData->Stages[Disease.CurrentStage];
        if (CurrentStage.bCanBeDetected)
        {
            // A more complex implementation would involve player stats (e.g., perception, medical skill)
            if (FMath::FRand() < 0.5f) 
            {
                Disease.bIsDetected = true;
            }
        }
    }
}

void UDiseaseManagementComponent::TransmitDisease(const FActiveDiseaseInstance& Disease)
{
    static const FString ContextString(TEXT("Disease Data Context"));
    FDiseaseData* DiseaseData = Disease.DiseaseDataRowHandle.GetRow<FDiseaseData>(ContextString);
    if (!DiseaseData || !Disease.bIsContagious || !GetOwner())
    {
        return;
    }

    // A more complex implementation would use different transmission vectors (airborne, contact, etc.)
    // For now, we'll do a simple proximity check for airborne transmission.
    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(DiseaseData->TransmissionRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());

    if(UWorld* World = GetWorld())
    {
        bool bHit = World->OverlapMultiByChannel(
            Overlaps,
            GetOwner()->GetActorLocation(),
            FQuat::Identity,
            ECC_Pawn,
            Sphere,
            QueryParams
        );

        if (bHit)
        {
            for (const FOverlapResult& Hit : Overlaps)
            {
                AActor* OtherActor = Hit.GetActor();
                if (OtherActor && OtherActor != GetOwner())
                {
                    UDiseaseManagementComponent* OtherDiseaseComp = OtherActor->FindComponentByClass<UDiseaseManagementComponent>();
                    if (OtherDiseaseComp)
                    {
                        // Add randomness to transmission
                        if (FMath::FRand() < DiseaseData->BaseTransmissionRate)
                        {
                            OtherDiseaseComp->Infect(Disease.DiseaseID);
                        }
                    }
                }
            }
        }
    }
}

void UDiseaseManagementComponent::AddImmunity(FName DiseaseID, float Duration, bool bIsPermanent)
{
	FDiseaseImmunityRecord NewRecord;
	NewRecord.DiseaseID = DiseaseID;
	if (UWorld* World = GetWorld())
	{
		NewRecord.ImmunityStartTime = World->GetTimeSeconds();
	}
	NewRecord.ImmunityDuration = Duration;
	NewRecord.bIsPermanent = bIsPermanent;
	ImmunityRecords.Add(NewRecord);
}

int32 UDiseaseManagementComponent::GetCurrentStage(FName DiseaseID) const
{
	const FActiveDiseaseInstance* Disease = ActiveDiseases.FindByPredicate([DiseaseID](const FActiveDiseaseInstance& InDisease)
	{
		return InDisease.DiseaseID == DiseaseID;
	});

	if (Disease)
	{
		return Disease->CurrentStage;
	}

	return -1;
}

void UDiseaseManagementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	   DOREPLIFETIME(UDiseaseManagementComponent, ActiveDiseases);
}

void UDiseaseManagementComponent::ServerInfect_Implementation(FName DiseaseID)
{
	   Infect(DiseaseID);
}

void UDiseaseManagementComponent::ServerTreatDisease_Implementation(FName DiseaseID, FName TreatmentID)
{
	   TreatDisease(DiseaseID, TreatmentID);
}

void UDiseaseManagementComponent::OnRep_ActiveDiseases()
{
	   // Broadcast a generic event that the UI can listen to.
	      // A more detailed implementation would compare against a cached list to see what specifically changed.
	      OnDiseasesChanged.Broadcast();
}