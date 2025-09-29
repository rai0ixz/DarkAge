#include "Core/AdvancedSurvivalSubsystem.h"
#include "Core/NeedsCalculationSubsystem.h"
#include "Components/DiseaseManagementComponent.h"
#include "Components/ClimateAdaptationComponent.h"
#include "Components/StatlineComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UAdvancedSurvivalSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Optionally, set up a timer or hook into the game loop for ticking
}

void UAdvancedSurvivalSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAdvancedSurvivalSubsystem::RegisterCharacter(ACharacter* Character)
{
	if (Character)
	{
		RegisteredCharacters.Add(Character);
	}
	if (UNeedsCalculationSubsystem* NeedsSubsystem = GetGameInstance()->GetSubsystem<UNeedsCalculationSubsystem>())
	{
		NeedsSubsystem->RegisterCharacter(Character);
	}
}

void UAdvancedSurvivalSubsystem::UnregisterCharacter(ACharacter* Character)
{
	if (Character)
	{
		RegisteredCharacters.Remove(Character);
	}
	if (UNeedsCalculationSubsystem* NeedsSubsystem = GetGameInstance()->GetSubsystem<UNeedsCalculationSubsystem>())
	{
		NeedsSubsystem->UnregisterCharacter(Character);
	}
}

void UAdvancedSurvivalSubsystem::Tick(float DeltaTime)
{
    if (!bSurvivalEnabled)
    {
        return;
    }

	for (auto It = RegisteredCharacters.CreateIterator(); It; ++It)
	{
		if (ACharacter* Character = It->Get())
		{
			UpdateCharacterSurvival(Character, DeltaTime);
		}
	}
}

void UAdvancedSurvivalSubsystem::UpdateCharacterSurvival(ACharacter* Character, float DeltaTime)
{
	if (!Character)
	{
		return;
	}

	// --- Disease Check based on Climate ---
	UDiseaseManagementComponent* DiseaseComp = Character->FindComponentByClass<UDiseaseManagementComponent>();
	UClimateAdaptationComponent* ClimateComp = Character->FindComponentByClass<UClimateAdaptationComponent>();

	if (DiseaseComp && ClimateComp)
	{
		     if (const FClimateSurvivalInfo* ClimateInfo = ClimateSurvivalData.Find(ClimateComp->GetCurrentClimate()))
		     {
		         for (const auto& DiseaseRiskPair : ClimateInfo->DiseaseRisk)
		         {
		             float ChanceOfInfection = DiseaseRiskPair.Value * DeltaTime;
		             if (FMath::FRand() < ChanceOfInfection)
		             {
		                 // Assuming ESurvivalDiseaseType can be converted to a FName for the Infect function
		                 DiseaseComp->Infect(FName(*UEnum::GetValueAsString(DiseaseRiskPair.Key)));
		             }
		         }
		     }
	}

	UStatlineComponent* StatlineComp = Character->FindComponentByClass<UStatlineComponent>();
	if (StatlineComp)
	{
		// Decrement hunger and thirst over time. The actual rate would be defined by game design.
		float HungerDecrement = 0.1f * DeltaTime;
		float ThirstDecrement = 0.2f * DeltaTime;

		StatlineComp->UpdateStat(FName("Hunger"), -HungerDecrement);
		StatlineComp->UpdateStat(FName("Thirst"), -ThirstDecrement);

		// Apply damage if hunger or thirst are at zero
		if (StatlineComp->GetCurrentStatValue(FName("Hunger")) <= 0.0f)
		{
			StatlineComp->ModifyHealth(-0.5f * DeltaTime); // Small damage over time
		}
		if (StatlineComp->GetCurrentStatValue(FName("Thirst")) <= 0.0f)
		{
			StatlineComp->ModifyHealth(-1.0f * DeltaTime); // More damage for thirst
		}
	}
}

void UAdvancedSurvivalSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	   DOREPLIFETIME(UAdvancedSurvivalSubsystem, RegisteredCharacters);
}

void UAdvancedSurvivalSubsystem::ServerRegisterCharacter_Implementation(ACharacter* Character)
{
	   RegisterCharacter(Character);
}

void UAdvancedSurvivalSubsystem::ServerUnregisterCharacter_Implementation(ACharacter* Character)
{
	   UnregisterCharacter(Character);
}