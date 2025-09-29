#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/AchievementData.h"
#include "DAUIManager.generated.h"

class UDAPlayerStatusWidget;
class UDAInventoryWidget;
class UDANotificationWidget;
class UDAInteractionPromptWidget;
class UDADialogueWidget;
class UDAQuestLogWidget;
class UDAPlayerHUDWidget;

/**
 * Manages the creation and lifecycle of UI widgets.
 */
UCLASS()
class DARKAGE_API UDAUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UDAUIManager();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void InitializeForPlayer(class APlayerController* PlayerController);
    void Teardown();

    void ToggleInventory();
    void ToggleQuestLog();

    void ShowNotification(const FText& Message, float Duration = 3.0f);
    void ShowInteractionPrompt(const FText& PromptText, const FText& ActionText);
    void HideInteractionPrompt();
    void ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions);
    void HideDialogue();
    void UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst);

public:
    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnAchievementUnlocked OnAchievementUnlocked;

private:
    UPROPERTY()
    TSubclassOf<UDAPlayerStatusWidget> PlayerStatusWidgetClass;

    UPROPERTY()
    TSubclassOf<UDAInventoryWidget> InventoryWidgetClass;

    UPROPERTY()
    TSubclassOf<UDANotificationWidget> NotificationWidgetClass;

    UPROPERTY()
    TSubclassOf<UDAInteractionPromptWidget> InteractionPromptWidgetClass;

    UPROPERTY()
    TSubclassOf<UDADialogueWidget> DialogueWidgetClass;

    UPROPERTY()
    TSubclassOf<UDAQuestLogWidget> QuestLogWidgetClass;

    UPROPERTY()
    TSubclassOf<UDAPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY()
    UDAPlayerStatusWidget* PlayerStatusWidget;

    UPROPERTY()
    UDAInventoryWidget* InventoryWidget;

    UPROPERTY()
    UDANotificationWidget* NotificationWidget;

    UPROPERTY()
    UDAInteractionPromptWidget* InteractionPromptWidget;

    UPROPERTY()
    UDADialogueWidget* DialogueWidget;

    UPROPERTY()
    UDAQuestLogWidget* QuestLogWidget;

    UPROPERTY()
    UDAPlayerHUDWidget* PlayerHUDWidget;

    UPROPERTY()
    APlayerController* OwningPlayerController;

    void InitializeWidgets();
    void InitializeHUDSubWidgets();
    void InitializeStandaloneWidgets();

    UFUNCTION()
    void HandlePrincipleDiscovered(const struct FPrinciple& Principle);

    UFUNCTION()
    void HandleEpochAdvanced(const struct FEpochData& NewEpoch);

    UFUNCTION()
    void HandleCultureEvolved(FName FactionID, const struct FCultureProfile& NewProfile);

    UFUNCTION()
    void HandleWorldStateChanged(FName RegionID, const struct FRegionState& NewState);

    UFUNCTION()
    void HandleWorldEvent(FName EventType, FName RegionID, const FString& EventDescription);

    UFUNCTION()
    void HandleQuestStateChanged(FName QuestID, enum EQuestState NewState);

    UFUNCTION()
    void HandleQuestStarted(FName QuestID);

    UFUNCTION()
    void HandleQuestCompleted(FName QuestID);

    UFUNCTION()
    void HandleQuestFailed(FName QuestID);

    UFUNCTION()
    void HandleQuestAbandoned(FName QuestID);
};