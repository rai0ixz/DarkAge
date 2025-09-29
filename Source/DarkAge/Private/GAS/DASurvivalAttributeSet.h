// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"  // For FGameplayEffectModCallbackData
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"  // For UAbilitySystemComponent
#include "DASurvivalAttributeSet.generated.h"

// Macro for attribute accessors
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DARKAGE_API UDASurvivalAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UDASurvivalAttributeSet();

    // Survival attributes
    UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Hunger)
    FGameplayAttributeData Hunger;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Hunger)

    UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Thirst)
    FGameplayAttributeData Thirst;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Thirst)

    UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Warmth)
    FGameplayAttributeData Warmth;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Warmth)

    UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Sleep)
    FGameplayAttributeData Sleep;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Sleep)

    // Combat/RPG attributes
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Strength)
    FGameplayAttributeData Strength;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Strength)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Dexterity)
    FGameplayAttributeData Dexterity;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Dexterity)

    // Health, etc. (expand as needed)
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_BaseDamage)
    FGameplayAttributeData BaseDamage;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, BaseDamage)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_STRModifier)
    FGameplayAttributeData STRModifier;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, STRModifier)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_ArmorDR)
    FGameplayAttributeData ArmorDR;
    ATTRIBUTE_ACCESSORS(UDASurvivalAttributeSet, ArmorDR)

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    // virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
    UFUNCTION()
    void OnRep_Hunger(const FGameplayAttributeData& OldHunger) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Hunger, OldHunger); }

    UFUNCTION()
    void OnRep_Thirst(const FGameplayAttributeData& OldThirst) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Thirst, OldThirst); }

    UFUNCTION()
    void OnRep_Warmth(const FGameplayAttributeData& OldWarmth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Warmth, OldWarmth); }

    UFUNCTION()
    void OnRep_Sleep(const FGameplayAttributeData& OldSleep) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Sleep, OldSleep); }

    UFUNCTION()
    void OnRep_Strength(const FGameplayAttributeData& OldStrength) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Strength, OldStrength); }

    UFUNCTION()
    void OnRep_Dexterity(const FGameplayAttributeData& OldDexterity) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Dexterity, OldDexterity); }

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Health, OldHealth); }

    UFUNCTION()
    void OnRep_Stamina(const FGameplayAttributeData& OldStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, Stamina, OldStamina); }

    UFUNCTION()
    void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, BaseDamage, OldBaseDamage); }

    UFUNCTION()
    void OnRep_STRModifier(const FGameplayAttributeData& OldSTRModifier) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, STRModifier, OldSTRModifier); }

    UFUNCTION()
    void OnRep_ArmorDR(const FGameplayAttributeData& OldArmorDR) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDASurvivalAttributeSet, ArmorDR, OldArmorDR); }
};