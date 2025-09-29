// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SurvivalSkillData.h"
#include "SurvivalSkillsComponent.generated.h"

// Delegate for when a skill levels up
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSurvivalSkillLeveledUp, ESurvivalSkill, Skill, int32, NewLevel, int32, OldLevel);
// Delegate for when XP is added to a skill
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSurvivalSkillXPAdded, ESurvivalSkill, Skill, float, XPAmount, float, NewTotalXP);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API USurvivalSkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivalSkillsComponent();

protected:
	virtual void BeginPlay() override;

	// Map holding the player's current skill instances
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival Skills", SaveGame)
	TMap<ESurvivalSkill, FSurvivalSkillInstanceData> SurvivalSkills;

	// DataTable containing definitions for all available survival skills
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival Skills")
	UDataTable* SkillDefinitionsDataTable;

	// Helper function to calculate XP needed for a given level
	float CalculateXPForNextLevel(ESurvivalSkill Skill, int32 CurrentLevel) const;

public:
	// Initializes all skills from the data table
	UFUNCTION(BlueprintCallable, Category = "Survival Skills")
	void InitializeSkills();

	UFUNCTION(BlueprintPure, Category = "Survival Skills")
	int32 GetSkillLevel(ESurvivalSkill Skill) const;

	UFUNCTION(BlueprintPure, Category = "Survival Skills")
	float GetSkillXP(ESurvivalSkill Skill) const;

	UFUNCTION(BlueprintPure, Category = "Survival Skills")
	bool GetSkillProgress(ESurvivalSkill Skill, float& OutCurrentXP, float& OutXPForNext, int32& OutCurrentLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Survival Skills")
	void AddXP(ESurvivalSkill Skill, float XPAmount);

	UFUNCTION(BlueprintPure, Category = "Survival Skills")
	FText GetSkillDisplayName(ESurvivalSkill Skill) const;

	UFUNCTION(BlueprintPure, Category = "Survival Skills")
	const TMap<ESurvivalSkill, FSurvivalSkillInstanceData>& GetAllSurvivalSkills() const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Survival Skills")
	FOnSurvivalSkillLeveledUp OnSkillLeveledUp_Event;

	UPROPERTY(BlueprintAssignable, Category = "Survival Skills")
	FOnSurvivalSkillXPAdded OnSkillXPAdded_Event;
};