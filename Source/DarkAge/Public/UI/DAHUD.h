#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/SubWidgets/DAInventoryWidget.h"
#include "UI/SubWidgets/DAPlayerStatusWidget.h"
#include "UI/SubWidgets/DANotificationWidget.h"
#include "DAInteractionPromptWidget.h"
#include "DADialogueWidget.h"
#include "UI/DAPlayerHUDWidget.h"

#include "DAHUD.generated.h"

// Forward declarations
class UDAUIManager;

/**
 * @brief Main HUD class for Dark Age.
 *
 * Manages and displays all in-game UI widgets, including player status, inventory, notifications, and interaction prompts.
 *
 * @see [API Doc](../../../docs/api/ADAHUD.md)
 */
UCLASS()
class DARKAGE_API ADAHUD : public AHUD
{
    GENERATED_BODY()

public:
    /**
     * Default constructor
     */
    ADAHUD();

    /**
     * Called when the game starts or when spawned
     */
    virtual void BeginPlay() override;

    /**
     * Called when the game ends
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /**
     * Toggle the inventory UI
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleInventory();

    /**
     * Toggle the quest log UI
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleQuestLog();

    /**
     * Show a notification to the player
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowNotification(const FText& Message, float Duration = 3.0f);

    /**
     * Show the interaction prompt
     */
    void ShowInteractionPrompt(const FText& PromptText, const FText& ActionText);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideInteractionPrompt();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideDialogue();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst);

protected:
    UPROPERTY(Transient)
    UDAUIManager* UIManager;
};