// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/SurvivalSkillsComponent.h"
#include "Data/SurvivalSkillData.h"

USurvivalSkillsComponent::USurvivalSkillsComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // No need to tick every frame
}

void USurvivalSkillsComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeSkills();
}

void USurvivalSkillsComponent::InitializeSkills()
{
	if (!SkillDefinitionsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Survival Skill Definitions Data Table not set!"));
		return;
	}

	TArray<FName> RowNames = SkillDefinitionsDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FSurvivalSkillDefinition* SkillDef = SkillDefinitionsDataTable->FindRow<FSurvivalSkillDefinition>(RowName, TEXT(""));
		if (SkillDef)
		{
			SurvivalSkills.Add(SkillDef->Skill, FSurvivalSkillInstanceData());
		}
	}
}

int32 USurvivalSkillsComponent::GetSkillLevel(ESurvivalSkill Skill) const
{
	const FSurvivalSkillInstanceData* SkillData = SurvivalSkills.Find(Skill);
	return SkillData ? SkillData->CurrentLevel : 0;
}

float USurvivalSkillsComponent::GetSkillXP(ESurvivalSkill Skill) const
{
	const FSurvivalSkillInstanceData* SkillData = SurvivalSkills.Find(Skill);
	return SkillData ? SkillData->CurrentXP : 0.0f;
}

bool USurvivalSkillsComponent::GetSkillProgress(ESurvivalSkill Skill, float& OutCurrentXP, float& OutXPForNext, int32& OutCurrentLevel) const
{
	const FSurvivalSkillInstanceData* SkillData = SurvivalSkills.Find(Skill);
	if (SkillData)
	{
		OutCurrentLevel = SkillData->CurrentLevel;
		OutCurrentXP = SkillData->CurrentXP;
		OutXPForNext = CalculateXPForNextLevel(Skill, OutCurrentLevel);
		return true;
	}
	return false;
}

void USurvivalSkillsComponent::AddXP(ESurvivalSkill Skill, float XPAmount)
{
	if (XPAmount <= 0.0f)
	{
		return;
	}

	FSurvivalSkillInstanceData* SkillData = SurvivalSkills.Find(Skill);
	if (!SkillData)
	{
		return;
	}

	SkillData->CurrentXP += XPAmount;
	OnSkillXPAdded_Event.Broadcast(Skill, XPAmount, SkillData->CurrentXP);

	float XPForNextLevel = CalculateXPForNextLevel(Skill, SkillData->CurrentLevel);
	while (SkillData->CurrentXP >= XPForNextLevel)
	{
		int32 OldLevel = SkillData->CurrentLevel;
		SkillData->CurrentLevel++;
		SkillData->CurrentXP -= XPForNextLevel;
		OnSkillLeveledUp_Event.Broadcast(Skill, SkillData->CurrentLevel, OldLevel);
		XPForNextLevel = CalculateXPForNextLevel(Skill, SkillData->CurrentLevel);
	}
}

FText USurvivalSkillsComponent::GetSkillDisplayName(ESurvivalSkill Skill) const
{
	if (SkillDefinitionsDataTable)
	{
		FSurvivalSkillDefinition* SkillDef = SkillDefinitionsDataTable->FindRow<FSurvivalSkillDefinition>(*UEnum::GetValueAsString(Skill), TEXT(""));
		if (SkillDef)
		{
			return SkillDef->DisplayName;
		}
	}
	return FText::GetEmpty();
}

const TMap<ESurvivalSkill, FSurvivalSkillInstanceData>& USurvivalSkillsComponent::GetAllSurvivalSkills() const
{
	return SurvivalSkills;
}

float USurvivalSkillsComponent::CalculateXPForNextLevel(ESurvivalSkill Skill, int32 CurrentLevel) const
{
	// This is a simple example. You would likely have a more complex formula, possibly defined in the data table.
	return 100.0f * FMath::Pow(1.5f, CurrentLevel - 1);
}