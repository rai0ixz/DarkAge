#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DiseaseData.h"
#include "DiseaseManagementComponent.generated.h"

// Forward Declarations
class UStatusEffectComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiseasesChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UDiseaseManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDiseaseManagementComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Disease")
	void Infect(FName DiseaseID);

	UFUNCTION(BlueprintCallable, Category = "Disease")
	void TreatDisease(FName DiseaseID, FName TreatmentID);

	UFUNCTION(BlueprintPure, Category = "Disease")
	bool IsInfectedWith(FName DiseaseID) const;

	UFUNCTION(BlueprintPure, Category = "Disease")
	const TArray<FActiveDiseaseInstance>& GetActiveDiseases() const { return ActiveDiseases; }

	UFUNCTION(BlueprintPure, Category = "Disease")
	bool HasImmunity(FName DiseaseID) const;

	UFUNCTION(BlueprintPure, Category = "Disease")
	int32 GetCurrentStage(FName DiseaseID) const;


	void UpdateDiseaseProgression(FActiveDiseaseInstance& Disease, float DeltaTime);
	void ApplyDiseaseEffects(const FDiseaseData* DiseaseData, FActiveDiseaseInstance& ActiveDisease);
	void RemoveDiseaseEffects(const FDiseaseData* DiseaseData, FActiveDiseaseInstance& ActiveDisease);
	void AttemptToDetectDisease(FActiveDiseaseInstance& Disease);
	void TransmitDisease(const FActiveDiseaseInstance& Disease);
	void AddImmunity(FName DiseaseID, float Duration, bool bIsPermanent);

	// Replicated diseases
	UPROPERTY(VisibleAnywhere, Category = "Disease", ReplicatedUsing=OnRep_ActiveDiseases)
	TArray<FActiveDiseaseInstance> ActiveDiseases;

	// RepNotify for ActiveDiseases
	UFUNCTION()
	void OnRep_ActiveDiseases();

public:
	// Server RPCs for disease actions
	UFUNCTION(Server, Reliable)
	void ServerInfect(FName DiseaseID);

	UFUNCTION(Server, Reliable)
	void ServerTreatDisease(FName DiseaseID, FName TreatmentID);

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "Disease")
	TArray<FDiseaseImmunityRecord> ImmunityRecords;

	UPROPERTY(BlueprintAssignable, Category = "Disease")
	FOnDiseasesChanged OnDiseasesChanged;

	UPROPERTY()
	TWeakObjectPtr<UStatusEffectComponent> StatusEffectComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Disease")
	UDataTable* DiseaseDataTable;
};