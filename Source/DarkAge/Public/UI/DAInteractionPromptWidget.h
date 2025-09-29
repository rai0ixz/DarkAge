#pragma once

#include "CoreMinimal.h"
#include "UI/DAUserInterface.h"
#include "DAInteractionPromptWidget.generated.h"

/**
 * Widget for displaying interaction prompts to the player
 */
UCLASS()
class DARKAGE_API UDAInteractionPromptWidget : public UDAUserInterface
{
    GENERATED_BODY()

public:
    UDAInteractionPromptWidget(const FObjectInitializer& ObjectInitializer);

    // Initialize the widget
    virtual void InitializeWidget() override;

    // Set the prompt text
    UFUNCTION(BlueprintCallable, Category = "UI|Interaction")
    void SetPromptText(const FText& Text);

    // Set the action text
    UFUNCTION(BlueprintCallable, Category = "UI|Interaction")
    void SetActionText(const FText& Text);

protected:
    // Prompt text block
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PromptText;

    // Action text block
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ActionText;

    // Current prompt text
    UPROPERTY(BlueprintReadOnly, Category = "UI|Interaction")
    FText CurrentPromptText;

    // Current action text
    UPROPERTY(BlueprintReadOnly, Category = "UI|Interaction")
    FText CurrentActionText;
};