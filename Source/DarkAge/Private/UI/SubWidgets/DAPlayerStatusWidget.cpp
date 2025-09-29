#include "UI/SubWidgets/DAPlayerStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UDAPlayerStatusWidget::UDAPlayerStatusWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentHealth(100.0f)
    , MaxHealthValue(100.0f)
    , CurrentStamina(100.0f)
    , MaxStaminaValue(100.0f)
    , CurrentHunger(100.0f)
    , MaxHungerValue(100.0f)
    , CurrentThirst(100.0f)
    , MaxThirstValue(100.0f)
{
}

void UDAPlayerStatusWidget::InitializeWidget()
{
    Super::InitializeWidget();
    
    // Set initial values
    UpdateUI();
}

void UDAPlayerStatusWidget::UpdateStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst)
{
    // Update values
    CurrentHealth = Health;
    MaxHealthValue = MaxHealth;
    CurrentStamina = Stamina;
    MaxStaminaValue = MaxStamina;
    CurrentHunger = Hunger;
    MaxHungerValue = MaxHunger;
    CurrentThirst = Thirst;
    MaxThirstValue = MaxThirst;
    
    // Update UI
    UpdateUI();
}

void UDAPlayerStatusWidget::UpdateUI()
{
    // Update health bar and text
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealthValue);
    }
    
    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentHealth, MaxHealthValue)));
    }
    
    // Update stamina bar and text
    if (StaminaBar)
    {
        StaminaBar->SetPercent(CurrentStamina / MaxStaminaValue);
    }
    
    if (StaminaText)
    {
        StaminaText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentStamina, MaxStaminaValue)));
    }
    
    // Update hunger bar and text
    if (HungerBar)
    {
        HungerBar->SetPercent(CurrentHunger / MaxHungerValue);
    }
    
    if (HungerText)
    {
        HungerText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentHunger, MaxHungerValue)));
    }
    
    // Update thirst bar and text
    if (ThirstBar)
    {
        ThirstBar->SetPercent(CurrentThirst / MaxThirstValue);
    }
    
    if (ThirstText)
    {
        ThirstText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentThirst, MaxThirstValue)));
    }
}