// SurvivalIntegrationHelper.cpp
#include "Core/SurvivalIntegrationHelper.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "Components/StatlineComponent.h"
#include "Components/DiseaseManagementComponent.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UAdvancedSurvivalSubsystem* USurvivalIntegrationHelper::GetSurvivalSubsystem(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }
    
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    if (!GameInstance)
    {
        return nullptr;
    }
    
    return GameInstance->GetSubsystem<UAdvancedSurvivalSubsystem>();
}

void USurvivalIntegrationHelper::RegisterWithSurvivalSystem(const UObject* WorldContextObject, ADAPlayerCharacter* Character, UStatlineComponent* StatlineComp)
{
    UAdvancedSurvivalSubsystem* SurvivalSystem = GetSurvivalSubsystem(WorldContextObject);
    if (SurvivalSystem && IsValid(Character))
    {
        SurvivalSystem->RegisterCharacter(Character);
    }
}

void USurvivalIntegrationHelper::UnregisterFromSurvivalSystem(const UObject* WorldContextObject, ADAPlayerCharacter* Character)
{
    UAdvancedSurvivalSubsystem* SurvivalSystem = GetSurvivalSubsystem(WorldContextObject);
    if (SurvivalSystem && IsValid(Character))
    {
        SurvivalSystem->UnregisterCharacter(Character);
    }
}

void USurvivalIntegrationHelper::UpdateHunger(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character, float Delta)
{
    if (IsValid(Character))
    {
        UStatlineComponent* Statline = Character->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            Statline->UpdateStat(FName("Hunger"), Delta);
        }
    }
}

void USurvivalIntegrationHelper::UpdateThirst(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character, float Delta)
{
    if (IsValid(Character))
    {
        UStatlineComponent* Statline = Character->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            Statline->UpdateStat(FName("Thirst"), Delta);
        }
    }
}

void USurvivalIntegrationHelper::SetDiseaseState(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character, ESurvivalDiseaseType DiseaseType, bool bHasDisease)
{
    if (IsValid(Character))
    {
        UDiseaseManagementComponent* DiseaseComp = Character->FindComponentByClass<UDiseaseManagementComponent>();
        if (DiseaseComp)
        {
            // For demo: only handle TheShakes
            if (DiseaseType == ESurvivalDiseaseType::TheShakes)
            {
                if (bHasDisease)
                {
                    DiseaseComp->Infect(FName("TheShakes"));
                }
                else
                {
                    DiseaseComp->TreatDisease(FName("TheShakes"), FName("DefaultTreatment"));
                }
            }
        }
    }
}

float USurvivalIntegrationHelper::GetCurrentHunger(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character)
{
    if (IsValid(Character))
    {
        UStatlineComponent* Statline = Character->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            return Statline->GetCurrentStatValue(FName("Hunger"));
        }
    }
    return 0.0f;
}

float USurvivalIntegrationHelper::GetCurrentThirst(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character)
{
    if (IsValid(Character))
    {
        UStatlineComponent* Statline = Character->FindComponentByClass<UStatlineComponent>();
        if (Statline)
        {
            return Statline->GetCurrentStatValue(FName("Thirst"));
        }
    }
    return 0.0f;
}

bool USurvivalIntegrationHelper::HasDisease(const UObject* /*WorldContextObject*/, ADAPlayerCharacter* Character, ESurvivalDiseaseType DiseaseType)
{
    if (IsValid(Character))
    {
        UDiseaseManagementComponent* DiseaseComp = Character->FindComponentByClass<UDiseaseManagementComponent>();
        if (DiseaseComp)
        {
            if (DiseaseType == ESurvivalDiseaseType::TheShakes)
            {
                return DiseaseComp->IsInfectedWith(FName("TheShakes"));
            }
        }
    }
    return false;
}