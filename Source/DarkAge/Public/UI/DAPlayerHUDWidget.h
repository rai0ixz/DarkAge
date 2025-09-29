#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/GameNotification.h"
class UDADamageTextWidget;
class UDAPlayerStatusWidget;
class UDAInventoryWidget;
class UDANotificationWidget;
class UDAInteractionPromptWidget;
class UDADialogueWidget;
class UDAQuestLogWidget;
class UCompassWidget;
class UMinimapWidget;

#include "DAPlayerHUDWidget.generated.h"

UCLASS()
class DARKAGE_API UDAPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()
public:
     // References to all sub-widgets (bind in UMG Designer)
         UPROPERTY()
         UDAPlayerStatusWidget* PlayerStatusWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UDAInventoryWidget* InventoryWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UDANotificationWidget* NotificationWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UDAInteractionPromptWidget* InteractionPromptWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UDADialogueWidget* DialogueWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UDAQuestLogWidget* QuestLogWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UCompassWidget* CompassWidget;
    UPROPERTY(meta = (BindWidgetOptional))
    UMinimapWidget* MinimapWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDADamageTextWidget> DamageTextWidgetClass;

    // BlueprintCallable helpers for showing/hiding panels
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotification(const FText& Message, float Duration = 3.0f);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleInventory();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleQuestLog();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowInteractionPrompt(const FText& PromptText, const FText& ActionText);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideInteractionPrompt();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideDialogue();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst);
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameNotification(const FGameNotification& Notification);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowDamageText(float DamageAmount, FVector WorldLocation);
};
