#pragma once

#include "CoreMinimal.h"
#include "UI/DAUserInterface.h"
#include "UI/GameNotification.h"
#include "DANotificationWidget.generated.h"

/**
 * Widget for displaying notifications to the player
 */
UCLASS()
class DARKAGE_API UDANotificationWidget : public UDAUserInterface
{
    GENERATED_BODY()

public:
    UDANotificationWidget(const FObjectInitializer& ObjectInitializer);

    // Initialize the widget
    virtual void InitializeWidget() override;

    // Enqueue a notification
    UFUNCTION(BlueprintCallable, Category = "UI|Notification")
    void EnqueueNotification(const FGameNotification& Notification);

    // Set the notification text
    UFUNCTION(BlueprintCallable, Category = "UI|Notification")
    void SetNotificationText(const FText& Text);

public:
    void ShowNextNotification();

protected:
    void PlayNotification(const FGameNotification& Notification);

    // Notification text block
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NotificationText;

    UPROPERTY(meta = (BindWidget))
    class UImage* IconImage;

    // Current notification text
    UPROPERTY(BlueprintReadOnly, Category = "UI|Notification")
    FText CurrentNotificationText;

    // --- Enhanced Notification System ---
    UPROPERTY()
    TArray<FGameNotification> NotificationQueue;

    UPROPERTY()
    bool bIsNotificationActive = false;

    FTimerHandle NotificationTimerHandle;
};