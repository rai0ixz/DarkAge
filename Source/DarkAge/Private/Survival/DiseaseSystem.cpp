// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival/DiseaseSystem.h"
#include "Components/StatlineComponent.h"

UDiseaseSystem::UDiseaseSystem()
	: TimeSinceLastUpdate(0.0f)
{
}

void UDiseaseSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDiseaseSystem::Deinitialize()
{
	Super::Deinitialize();
	
	// Clean up resources
	Diseases.Empty();
	CharacterDiseases.Empty();
	RegionalOutbreaks.Empty();
}

void UDiseaseSystem::UpdateDiseases(float DeltaTime)
{
	// Accumulate time since last update
	TimeSinceLastUpdate += DeltaTime;
	
	// Major disease updates happen less frequently
	const float DiseaseUpdateInterval = 60.0f; // 1 minute real time
	
	if (TimeSinceLastUpdate >= DiseaseUpdateInterval)
	{
		// Update disease progression for all infected characters
		TArray<TWeakObjectPtr<AActor>> CharactersToUpdate;
		CharacterDiseases.GetKeys(CharactersToUpdate);
		
		for (const TWeakObjectPtr<AActor>& Character : CharactersToUpdate)
		{
			if (Character.IsValid())
			{
				UpdateDiseaseProgression(Character.Get(), TimeSinceLastUpdate);
			}
			else
			{
				// Remove invalid characters
				CharacterDiseases.Remove(Character);
			}
		}
		
		// Check for disease transmission between characters
		CheckDiseaseTransmission(TimeSinceLastUpdate);
		
		// Update regional outbreaks
		UpdateRegionalOutbreaks(TimeSinceLastUpdate);
		
		// Reset timer
		TimeSinceLastUpdate = 0.0f;
	}
}

FDiseaseData UDiseaseSystem::GetDiseaseData(FName DiseaseID) const
{
	if (Diseases.Contains(DiseaseID))
	{
		return Diseases[DiseaseID];
	}
	
	// Return empty disease data if not found
	return FDiseaseData();
}

bool UDiseaseSystem::IsInfectedWith(AActor* Character, FName DiseaseID) const
{
	if (!Character || !Diseases.Contains(DiseaseID))
	{
		return false;
	}
	
	TWeakObjectPtr<AActor> CharacterPtr(Character);
	
	if (!CharacterDiseases.Contains(CharacterPtr))
	{
		return false;
	}
	
	const TArray<FActiveDiseaseInstance>& ActiveDiseases = CharacterDiseases[CharacterPtr];
	
	for (const FActiveDiseaseInstance& Disease : ActiveDiseases)
	{
		if (Disease.DiseaseID == DiseaseID)
		{
			return true;
		}
	}
	
	return false;
}

TArray<FActiveDiseaseInstance> UDiseaseSystem::GetActiveDiseases(AActor* Character) const
{
	if (!Character)
	{
		return TArray<FActiveDiseaseInstance>();
	}
	
	TWeakObjectPtr<AActor> CharacterPtr(Character);
	
	if (CharacterDiseases.Contains(CharacterPtr))
	{
		return CharacterDiseases[CharacterPtr];
	}
	
	return TArray<FActiveDiseaseInstance>();
}

bool UDiseaseSystem::InfectCharacter(AActor* Character, FName DiseaseID)
{
	if (!Character || !Diseases.Contains(DiseaseID))
	{
		return false;
	}
	
	// Check if already infected with this disease
	if (IsInfectedWith(Character, DiseaseID))
	{
		return false;
	}
	
	TWeakObjectPtr<AActor> CharacterPtr(Character);
	
	// Create new active disease
	FActiveDiseaseInstance NewDisease;
	NewDisease.DiseaseID = DiseaseID;
	NewDisease.bIsIncubating = true;
	
	// Add to character's diseases
	if (!CharacterDiseases.Contains(CharacterPtr))
	{
		CharacterDiseases.Add(CharacterPtr, TArray<FActiveDiseaseInstance>());
	}
	
	CharacterDiseases[CharacterPtr].Add(NewDisease);
	
	// Broadcast infection event
	OnDiseaseInfection.Broadcast(Character, DiseaseID);
	
	return true;
}

bool UDiseaseSystem::TreatDisease(AActor* Character, FName DiseaseID, FName MedicineID)
{
	if (!Character || !Diseases.Contains(DiseaseID))
	{
		return false;
	}
	
	TWeakObjectPtr<AActor> CharacterPtr(Character);
	
	if (!CharacterDiseases.Contains(CharacterPtr))
	{
		return false;
	}
	
	// Find the disease
	TArray<FActiveDiseaseInstance>& ActiveDiseases = CharacterDiseases[CharacterPtr];
	FActiveDiseaseInstance* Disease = nullptr;
	
	for (FActiveDiseaseInstance& ActiveDisease : ActiveDiseases)
	{
		if (ActiveDisease.DiseaseID == DiseaseID)
		{
			Disease = &ActiveDisease;
			break;
		}
	}
	
	if (!Disease)
	{
		return false;
	}
	
	// Check if medicine is appropriate for this disease
	const FDiseaseData& DiseaseData = Diseases[DiseaseID];
	
	// Treatment successful, remove disease
	ActiveDiseases.Remove(*Disease);
	
	// If no more diseases, remove character from map
	if (ActiveDiseases.Num() == 0)
	{
		CharacterDiseases.Remove(CharacterPtr);
	}
	
	// Broadcast recovery event
	OnDiseaseRecovery.Broadcast(Character, DiseaseID);
	
	return true;
}

float UDiseaseSystem::CalculateInfectionRisk(AActor* Character, FName DiseaseID) const
{
	if (!Character || !Diseases.Contains(DiseaseID))
	{
		return 0.0f;
	}
	
	const FDiseaseData& DiseaseData = Diseases[DiseaseID];
	
	// Base infection chance
	float Risk = DiseaseData.BaseTransmissionRate;
	
	// Modify based on character's immunity/health
	// This would integrate with the StatlineComponent
	UStatlineComponent* StatlineComp = Character->FindComponentByClass<UStatlineComponent>();
	if (StatlineComp)
	{
		// Example: Higher health reduces infection risk
		// This is a placeholder - actual implementation would depend on the StatlineComponent
		float HealthFactor = 1.0f; // Default factor
		// HealthFactor = StatlineComp->GetHealthPercentage();
		
		Risk *= (2.0f - HealthFactor); // Higher health = lower risk
	}
	
	// Modify based on regional factors
	// This would integrate with other systems like the player's current region
	
	// Modify based on seasonal factors
	// This would integrate with the TimeSystem
	
	// Clamp risk to valid range
	Risk = FMath::Clamp(Risk, 0.0f, 1.0f);
	
	return Risk;
}

void UDiseaseSystem::TriggerOutbreak(FName DiseaseID, FName RegionID, float Severity)
{
	if (!Diseases.Contains(DiseaseID))
	{
		return;
	}
	
	// Ensure region map exists
	if (!RegionalOutbreaks.Contains(RegionID))
	{
		RegionalOutbreaks.Add(RegionID, TMap<FName, float>());
	}
	
	// Set outbreak severity
	RegionalOutbreaks[RegionID].Add(DiseaseID, FMath::Clamp(Severity, 0.0f, 1.0f));
	
	// This would integrate with other systems to affect NPCs in the region
	// and increase infection risk for players in the area
}

TArray<FName> UDiseaseSystem::GetEndemicDiseases(FName RegionID) const
{
	TArray<FName> EndemicDiseases;
	
	for (const auto& DiseasePair : Diseases)
	{
		if (true)
		{
			EndemicDiseases.Add(DiseasePair.Key);
		}
	}
	
	return EndemicDiseases;
}

void UDiseaseSystem::UpdateDiseaseProgression(AActor* Character, float DeltaTime)
{
	if (!Character)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> CharacterPtr(Character);
	
	if (!CharacterDiseases.Contains(CharacterPtr))
	{
		return;
	}
	
	TArray<FActiveDiseaseInstance>& ActiveDiseases = CharacterDiseases[CharacterPtr];
	TArray<FActiveDiseaseInstance> DiseasesToRemove;
	
	for (FActiveDiseaseInstance& Disease : ActiveDiseases)
	{
		// Increase time since infection
		Disease.TotalDiseaseTime += DeltaTime;
		
		// Get disease data
		if (!Diseases.Contains(Disease.DiseaseID))
		{
			DiseasesToRemove.Add(Disease);
			continue;
		}
		
		const FDiseaseData& DiseaseData = Diseases[Disease.DiseaseID];
		
		// Check if incubation period is over
		if (Disease.bIsIncubating && Disease.TotalDiseaseTime >= DiseaseData.IncubationPeriod)
		{
			Disease.bIsIncubating = false;
		}
		
		// If still incubating, no symptoms or effects
		if (Disease.bIsIncubating)
		{
			continue;
		}
		
		// Calculate new severity
		EDiseaseSeverity NewSeverity = CalculateDiseaseSeverity(DiseaseData, Disease);
		
		// If severity changed, broadcast event
		if (Disease.CurrentStage != 0)
		{
			OnDiseaseProgression.Broadcast(Character, Disease.DiseaseID, NewSeverity);
		}
		
		// Apply disease effects
		ApplyDiseaseEffects(Character, DiseaseData, Disease);
		
		// Check if disease duration is over (and not chronic)
		if (!DiseaseData.bIsChronicCondition && Disease.TotalDiseaseTime >= 0.0f)
		{
			// Check if disease becomes chronic
			if (DiseaseData.bCanDevelopImmunity && FMath::FRand() < 0.2f) // 20% chance to become chronic
			{
				// Disease.bIsChronic = true;
			}
			else
			{
				// Disease is over, add to removal list
				DiseasesToRemove.Add(Disease);
				
				// Broadcast recovery event
				OnDiseaseRecovery.Broadcast(Character, Disease.DiseaseID);
			}
		}
		
		// Check for mortality (if critical and untreated)
		if (Disease.CurrentStage == 4 && !Disease.bIsDetected)
		{
			float MortalityChance = DiseaseData.bIsFatal * (DeltaTime / 3600.0f); // Scale by time (hourly chance)
			
			if (FMath::FRand() < MortalityChance)
			{
				// Character dies from disease
				// This would integrate with the damage/death system
				
				// For now, just remove the disease
				DiseasesToRemove.Add(Disease);
			}
		}
	}
	
	// Remove diseases that have run their course
	for (const FActiveDiseaseInstance& DiseaseToRemove : DiseasesToRemove)
	{
		ActiveDiseases.Remove(DiseaseToRemove);
	}
	
	// If no more diseases, remove character from map
	if (ActiveDiseases.Num() == 0)
	{
		CharacterDiseases.Remove(CharacterPtr);
	}
}

EDiseaseSeverity UDiseaseSystem::CalculateDiseaseSeverity(const FDiseaseData& DiseaseData, const FActiveDiseaseInstance& ActiveDisease) const
{
	// Calculate disease progression as a percentage of duration
	float ProgressionPercentage = 0.0f;
	if (DiseaseData.Stages.Num() > 0)
	{
		ProgressionPercentage = ActiveDisease.TotalDiseaseTime / DiseaseData.Stages[0].Duration;
	}
	
	// Adjust for treatment
	if (ActiveDisease.bIsDetected)
	{
		ProgressionPercentage *= (1.0f - 0.0f);
	}
	
	// For chronic diseases, maintain a lower severity
	if (DiseaseData.bIsChronicCondition)
	{
		return EDiseaseSeverity::Moderate;
	}
	
	// Determine severity based on progression
	// This is a simplified model - real disease progression would be more complex
	if (ProgressionPercentage < 0.25f)
	{
		return EDiseaseSeverity::Mild;
	}
	else if (ProgressionPercentage < 0.5f)
	{
		return EDiseaseSeverity::Moderate;
	}
	else if (ProgressionPercentage < 0.75f)
	{
		return EDiseaseSeverity::Severe;
	}
	else
	{
		return EDiseaseSeverity::Critical;
	}
}

void UDiseaseSystem::ApplyDiseaseEffects(AActor* Character, const FDiseaseData& DiseaseData, const FActiveDiseaseInstance& ActiveDisease)
{
	// Skip if incubating
	if (ActiveDisease.bIsIncubating)
	{
		return;
	}
	
	// Get attribute effects for current severity
	if (true)
	{
		return;
	}
	
	// const TMap<FName, float>& AttributeEffects = DiseaseData.AttributeEffects[ActiveDisease.CurrentSeverity];
	
	// Apply effects to character
	// This would integrate with the StatlineComponent or other stat systems
	UStatlineComponent* StatlineComp = Character->FindComponentByClass<UStatlineComponent>();
	if (StatlineComp)
	{
		// Apply each attribute effect
		// This is a placeholder - actual implementation would depend on the StatlineComponent
		for (const auto& EffectPair : DiseaseData.AssociatedStatusEffects)
		{
			// Example: StatlineComp->ApplyAttributeModifier(EffectPair.Key, EffectPair.Value);
		}
	}
}

void UDiseaseSystem::CheckDiseaseTransmission(float DeltaTime)
{
	// This would be a complex system that checks for disease transmission between characters
	// based on proximity, interactions, and transmission types
	
	// For now, this is just a placeholder
	
	// Get all characters with diseases
	TArray<TWeakObjectPtr<AActor>> InfectedCharacters;
	CharacterDiseases.GetKeys(InfectedCharacters);
	
	// For each infected character, check for potential transmission to nearby characters
	// This would integrate with a spatial query system or similar
}

void UDiseaseSystem::UpdateRegionalOutbreaks(float DeltaTime)
{
	// Update outbreak status in each region
	for (auto& RegionPair : RegionalOutbreaks)
	{
		for (auto& OutbreakPair : RegionPair.Value)
		{
			// Gradually reduce outbreak severity over time
			OutbreakPair.Value = FMath::Max(0.0f, OutbreakPair.Value - 0.01f * (DeltaTime / 3600.0f)); // Reduce by 1% per hour
			
			// If outbreak is over, remove it
			if (OutbreakPair.Value <= 0.0f)
			{
				RegionPair.Value.Remove(OutbreakPair.Key);
				break; // Break to avoid iterator invalidation
			}
		}
		
		// If no more outbreaks in region, remove region
		if (RegionPair.Value.Num() == 0)
		{
			RegionalOutbreaks.Remove(RegionPair.Key);
			break; // Break to avoid iterator invalidation
		}
	}
	
	// Potentially generate new outbreaks based on regional and seasonal factors
	// This would integrate with other systems like TimeSystem for seasonal effects
}