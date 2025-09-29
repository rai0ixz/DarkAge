#include "UI/SubWidgets/DANotificationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

UDANotificationWidget::UDANotificationWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDANotificationWidget::InitializeWidget()
{
    Super::InitializeWidget();
    
    // Set initial text
    if (NotificationText)
    {
        NotificationText->SetText(CurrentNotificationText);
    }
}

void UDANotificationWidget::SetNotificationText(const FText& Text)
{
    CurrentNotificationText = Text;
    
    if (NotificationText)
    {
        NotificationText->SetText(CurrentNotificationText);
    }
}

void UDANotificationWidget::EnqueueNotification(const FGameNotification& Notification)
{
    NotificationQueue.Add(Notification);
    if (!bIsNotificationActive)
    {
        ShowNextNotification();
    }
}

void UDANotificationWidget::ShowNextNotification()
{
    if (NotificationQueue.Num() > 0)
    {
        bIsNotificationActive = true;
        PlayNotification(NotificationQueue[0]);
    }
    else
    {
        bIsNotificationActive = false;
    }
}

void UDANotificationWidget::PlayNotification(const FGameNotification& Notification)
{
    SetNotificationText(Notification.Message);
    ShowWidget();
    if (IconImage)
    {
        IconImage->SetBrushFromTexture(Notification.Icon);
    }

    if (NotificationText)
    {
        FLinearColor TextColor = FLinearColor::White;
        switch (Notification.Category)
        {
            case ENotificationCategory::Quest:
                TextColor = FLinearColor::Yellow;
                break;
            case ENotificationCategory::Item:
                TextColor = FLinearColor::Green;
                break;
            case ENotificationCategory::System:
                TextColor = FLinearColor::Blue;
                break;
            case ENotificationCategory::Error:
                TextColor = FLinearColor::Red;
                break;
            default:
                break;
        }
        NotificationText->SetColorAndOpacity(TextColor);
    }

    if (Notification.Sound)
    {
        UGameplayStatics::PlaySound2D(this, Notification.Sound);
    }
    float Duration = Notification.Duration > 0.f ? Notification.Duration : 3.0f;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            NotificationTimerHandle,
            [this]() {
                HideWidget();
                if (NotificationQueue.Num() > 0) NotificationQueue.RemoveAt(0);
                ShowNextNotification();
            },
            Duration,
            false
        );
    }
}