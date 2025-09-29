#pragma once

#include "CoreMinimal.h"
#include "UI/DAUserInterface.h"
#include "DAPlayerStatusWidget.generated.h"

/**
 * Widget for displaying player status (health, stamina, hunger, thirst)
 */
UCLASS()
class DARKAGE_API UDAPlayerStatusWidget : public UDAUserInterface
{
    GENERATED_BODY()

public:
    UDAPlayerStatusWidget(const FObjectInitializer& ObjectInitializer);

    // Initialize the widget
    virtual void InitializeWidget() override;

    // Update the widget with new status values
    UFUNCTION(BlueprintCallable, Category = "UI|Player Status")
    void UpdateStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst);

protected:
    // Health progress bar
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;

    // Stamina progress bar
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaBar;

    // Hunger progress bar
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HungerBar;

    // Thirst progress bar
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ThirstBar;

    // Health text
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    // Stamina text
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StaminaText;

    // Hunger text
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HungerText;

    // Thirst text
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ThirstText;

    // Current health value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float CurrentHealth;

    // Maximum health value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float MaxHealthValue;

    // Current stamina value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float CurrentStamina;

    // Maximum stamina value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float MaxStaminaValue;

    // Current hunger value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float CurrentHunger;

    // Maximum hunger value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float MaxHungerValue;

    // Current thirst value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float CurrentThirst;

    // Maximum thirst value
    UPROPERTY(BlueprintReadOnly, Category = "UI|Player Status")
    float MaxThirstValue;

    // Update the progress bars and text
    void UpdateUI();
};