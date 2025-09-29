#pragma once

#include "CoreMinimal.h"
#include "UI/DAUserInterface.h"
#include "DADialogueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueResponseSelected, int32, ResponseIndex);

class UDAResponseOptionWidget;

/**
 * Widget for displaying dialogue with NPCs
 */
UCLASS()
class DARKAGE_API UDADialogueWidget : public UDAUserInterface
{
	GENERATED_BODY()

public:
	UDADialogueWidget(const FObjectInitializer& ObjectInitializer);

	// Initialize the widget
	virtual void InitializeWidget() override;

	// Set the dialogue data
	UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
	void SetDialogueData(const FText& SpeakerName, const FText& InDialogueText, const TArray<FText>& ResponseOptions);

	// Called when a response is selected
	UFUNCTION()
	void OnResponseReceived(int32 ResponseIndex);

	// Delegate for when a response is selected
	UPROPERTY(BlueprintAssignable, Category = "UI|Dialogue")
	FOnDialogueResponseSelected OnDialogueResponseSelected;

protected:
	// Speaker name text block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SpeakerNameText;

	// Dialogue text block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DialogueText;

	// Response options vertical box
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ResponseOptionsBox;

	// Response option button class
	UPROPERTY(EditDefaultsOnly, Category = "UI|Dialogue")
	TSubclassOf<UDAResponseOptionWidget> ResponseOptionWidgetClass;

	// Current speaker name
	UPROPERTY(BlueprintReadOnly, Category = "UI|Dialogue")
	FText CurrentSpeakerName;

	// Current dialogue text
	UPROPERTY(BlueprintReadOnly, Category = "UI|Dialogue")
	FText CurrentDialogueText;

	// Current response options
	UPROPERTY(BlueprintReadOnly, Category = "UI|Dialogue")
	TArray<FText> CurrentResponseOptions;

	// Update the UI with current data
	void UpdateUI();

	// Clear response options
	void ClearResponseOptions();

	// Add response options
	void AddResponseOptions();
};