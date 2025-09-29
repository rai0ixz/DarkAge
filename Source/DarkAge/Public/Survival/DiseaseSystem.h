// Copyright (c) 2025 RaioCore. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/DiseaseData.h"
#include "DiseaseSystem.generated.h"

/**
 * @brief Disease System for Dark Age.
 *
 * Manages disease simulation, infection, progression, and treatment.
 * Integrates with character stats, regional factors, and medical systems.
 *
 * @see [API Doc](../../../docs/api/DiseaseSystem.md)
 */
UCLASS()
class DARKAGE_API UDiseaseSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor
	 */
	UDiseaseSystem();

	/**
	 * Initialize the subsystem
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Deinitialize the subsystem
	 */
	virtual void Deinitialize() override;

	/**
	 * Update disease system
	 */
	UFUNCTION(BlueprintCallable, Category = "Disease")
	void UpdateDiseases(float DeltaTime);

	/**
	 * Get disease data by ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Disease")
	FDiseaseData GetDiseaseData(FName DiseaseID) const;

	/**
	 * Check if character is infected with a specific disease
	 */
	UFUNCTION(BlueprintCallable, Category = "Disease")
	bool IsInfectedWith(AActor* Character, FName DiseaseID) const;
	
	// Get all active diseases for a character
	UFUNCTION(BlueprintCallable, Category = "Disease")
	TArray<FActiveDiseaseInstance> GetActiveDiseases(AActor* Character) const;
	
	// Infect character with disease
	UFUNCTION(BlueprintCallable, Category = "Disease")
	bool InfectCharacter(AActor* Character, FName DiseaseID);
	
	// Attempt to cure disease
	UFUNCTION(BlueprintCallable, Category = "Disease")
	bool TreatDisease(AActor* Character, FName DiseaseID, FName MedicineID);
	
	// Calculate infection risk for character in current environment
	UFUNCTION(BlueprintCallable, Category = "Disease")
	float CalculateInfectionRisk(AActor* Character, FName DiseaseID) const;
	
	// Trigger disease outbreak in region
	UFUNCTION(BlueprintCallable, Category = "Disease")
	void TriggerOutbreak(FName DiseaseID, FName RegionID, float Severity);
	
	// Get all diseases endemic to a region
	UFUNCTION(BlueprintCallable, Category = "Disease")
	TArray<FName> GetEndemicDiseases(FName RegionID) const;
	
	// Delegate for disease infection
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiseaseInfection, AActor*, Character, FName, DiseaseID);
	UPROPERTY(BlueprintAssignable, Category = "Disease")
	FOnDiseaseInfection OnDiseaseInfection;
	
	// Delegate for disease progression
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDiseaseProgression, AActor*, Character, FName, DiseaseID, EDiseaseSeverity, NewSeverity);
	UPROPERTY(BlueprintAssignable, Category = "Disease")
	FOnDiseaseProgression OnDiseaseProgression;
	
	// Delegate for disease recovery
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiseaseRecovery, AActor*, Character, FName, DiseaseID);
	UPROPERTY(BlueprintAssignable, Category = "Disease")
	FOnDiseaseRecovery OnDiseaseRecovery;
	
private:
	// Disease database
	UPROPERTY()
	TMap<FName, FDiseaseData> Diseases;
	
	// Active diseases by character - we'll manage this manually without UPROPERTY
	TMap<TWeakObjectPtr<AActor>, TArray<FActiveDiseaseInstance>> CharacterDiseases;
	
	// Disease outbreaks by region - maps region ID to a map of disease IDs and their severity
	TMap<FName, TMap<FName, float>> RegionalOutbreaks;
	
	// Time since last disease update
	float TimeSinceLastUpdate;
	
	// Update disease progression for a character
	void UpdateDiseaseProgression(AActor* Character, float DeltaTime);
	
	// Calculate disease severity progression
	EDiseaseSeverity CalculateDiseaseSeverity(const FDiseaseData& DiseaseData, const FActiveDiseaseInstance& ActiveDisease) const;
	
	// Apply disease effects to character
	void ApplyDiseaseEffects(AActor* Character, const FDiseaseData& DiseaseData, const FActiveDiseaseInstance& ActiveDisease);
	
	// Check for disease transmission between characters
	void CheckDiseaseTransmission(float DeltaTime);
	
	// Update regional disease outbreaks
	void UpdateRegionalOutbreaks(float DeltaTime);
};