#include "UI/DAPlayerHUDWidget.h"
#include "UI/SubWidgets/DAPlayerStatusWidget.h"
#include "UI/SubWidgets/DAInventoryWidget.h"
#include "UI/SubWidgets/DANotificationWidget.h"
#include "UI/DAInteractionPromptWidget.h"
#include "UI/DADialogueWidget.h"
#include "UI/DAQuestLogWidget.h"
#include "UI/GameNotification.h"
#include "TimerManager.h"
#include "UI/SubWidgets/DADamageTextWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UDAPlayerHUDWidget::ShowNotification(const FText& Message, float Duration)
{
    if (NotificationWidget)
    {
        NotificationWidget->SetNotificationText(Message);
        NotificationWidget->ShowWidget();
        // Hide after duration (can be improved with queueing)
        FTimerHandle TimerHandle;
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                TimerHandle,
                [this]() { if (NotificationWidget) NotificationWidget->HideWidget(); },
                Duration,
                false
            );
        }
    }
}

void UDAPlayerHUDWidget::ShowGameNotification(const FGameNotification& Notification)
{
    if (NotificationWidget)
    {
        NotificationWidget->EnqueueNotification(Notification);
    }
}

void UDAPlayerHUDWidget::ToggleInventory()
{
    if (InventoryWidget)
    {
        if (InventoryWidget->IsWidgetVisible())
            InventoryWidget->HideWidget();
        else
            InventoryWidget->ShowWidget();
    }
}

void UDAPlayerHUDWidget::ToggleQuestLog()
{
    if (QuestLogWidget)
    {
        if (QuestLogWidget->GetVisibility() == ESlateVisibility::Visible)
            QuestLogWidget->SetVisibility(ESlateVisibility::Hidden);
        else
            QuestLogWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void UDAPlayerHUDWidget::ShowInteractionPrompt(const FText& PromptText, const FText& ActionText)
{
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->SetPromptText(PromptText);
        InteractionPromptWidget->SetActionText(ActionText);
        InteractionPromptWidget->ShowWidget();
    }
}

void UDAPlayerHUDWidget::HideInteractionPrompt()
{
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->HideWidget();
    }
}

void UDAPlayerHUDWidget::ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions)
{
    if (DialogueWidget)
    {
        DialogueWidget->SetDialogueData(SpeakerName, DialogueText, ResponseOptions);
        DialogueWidget->ShowWidget();
    }
}

void UDAPlayerHUDWidget::HideDialogue()
{
    if (DialogueWidget)
    {
        DialogueWidget->HideWidget();
    }
}

void UDAPlayerHUDWidget::UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->UpdateStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
    }
    else
    {
        // Fallback: try to find the widget if it exists but binding failed
        if (UWorld* World = GetWorld())
        {
            if (APlayerController* PC = World->GetFirstPlayerController())
            {
                TArray<UUserWidget*> Widgets;
                UWidgetBlueprintLibrary::GetAllWidgetsOfClass(PC, Widgets, UDAPlayerStatusWidget::StaticClass(), false);
                for (UUserWidget* Widget : Widgets)
                {
                    if (UDAPlayerStatusWidget* StatusWidget = Cast<UDAPlayerStatusWidget>(Widget))
                    {
                        StatusWidget->UpdateStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
                        break;
                    }
                }
            }
        }
    }
}

void UDAPlayerHUDWidget::ShowDamageText(float DamageAmount, FVector WorldLocation)
{
    if (DamageTextWidgetClass)
    {
        UDADamageTextWidget* DamageTextWidget = CreateWidget<UDADamageTextWidget>(GetOwningPlayer(), DamageTextWidgetClass);
        if (DamageTextWidget)
        {
            DamageTextWidget->SetDamageText(DamageAmount);
            DamageTextWidget->SetWorldLocation(WorldLocation);
            DamageTextWidget->AddToViewport();
        }
    }
}
