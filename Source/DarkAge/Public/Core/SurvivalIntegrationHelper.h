// SurvivalIntegrationHelper.h
// Helper class to integrate existing components with the new survival subsystem

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/AdvancedSurvivalData.h"
#include "Components/StatlineComponent.h"
#include "Components/DiseaseManagementComponent.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "SurvivalIntegrationHelper.generated.h"

class UStatlineComponent;
class UAdvancedSurvivalSubsystem;
class ADAPlayerCharacter;

/**
 * Helper functions to integrate existing components with the AdvancedSurvivalSubsystem
 */
UCLASS()
class DARKAGE_API USurvivalIntegrationHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Get the AdvancedSurvivalSubsystem from the game instance
     */
    UFUNCTION(BlueprintPure, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static UAdvancedSurvivalSubsystem* GetSurvivalSubsystem(const UObject* WorldContextObject);

    /**
     * Register a character and its StatlineComponent with the survival subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static void RegisterWithSurvivalSystem(const UObject* WorldContextObject, ADAPlayerCharacter* Character, UStatlineComponent* StatlineComp);

    /**
     * Unregister a character from the survival subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static void UnregisterFromSurvivalSystem(const UObject* WorldContextObject, ADAPlayerCharacter* Character);

    /**
     * Update hunger value in the survival subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static void UpdateHunger(const UObject* WorldContextObject, ADAPlayerCharacter* Character, float Delta);

    /**
     * Update thirst value in the survival subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static void UpdateThirst(const UObject* WorldContextObject, ADAPlayerCharacter* Character, float Delta);

    /**
     * Set disease state in the survival subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static void SetDiseaseState(const UObject* WorldContextObject, ADAPlayerCharacter* Character, ESurvivalDiseaseType DiseaseType, bool bHasDisease);

    /**
     * Get current hunger value from the survival subsystem
     */
    UFUNCTION(BlueprintPure, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static float GetCurrentHunger(const UObject* WorldContextObject, ADAPlayerCharacter* Character);

    /**
     * Get current thirst value from the survival subsystem
     */
    UFUNCTION(BlueprintPure, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static float GetCurrentThirst(const UObject* WorldContextObject, ADAPlayerCharacter* Character);

    /**
     * Check if character has a specific disease
     */
    UFUNCTION(BlueprintPure, Category = "Survival|Integration", meta = (WorldContext = "WorldContextObject"))
    static bool HasDisease(const UObject* WorldContextObject, ADAPlayerCharacter* Character, ESurvivalDiseaseType DiseaseType);
};