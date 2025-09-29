#include "UI/DADialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/UserWidget.h"
#include "UI/DAResponseOptionWidget.h"

UDADialogueWidget::UDADialogueWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDADialogueWidget::InitializeWidget()
{
    Super::InitializeWidget();
    
    // Set initial UI
    UpdateUI();
}

void UDADialogueWidget::SetDialogueData(const FText& SpeakerName, const FText& InDialogueText, const TArray<FText>& ResponseOptions)
{
    CurrentSpeakerName = SpeakerName;
    CurrentDialogueText = InDialogueText;
    CurrentResponseOptions = ResponseOptions;
    
    UpdateUI();
}

void UDADialogueWidget::OnResponseReceived(int32 ResponseIndex)
{
    OnDialogueResponseSelected.Broadcast(ResponseIndex);
}

void UDADialogueWidget::UpdateUI()
{
    // Update speaker name
    if (SpeakerNameText)
    {
        SpeakerNameText->SetText(CurrentSpeakerName);
    }
    
    // Update dialogue text
    if (DialogueText)
    {
        DialogueText->SetText(CurrentDialogueText);
    }
    
    // Update response options
    ClearResponseOptions();
    AddResponseOptions();
}

void UDADialogueWidget::ClearResponseOptions()
{
    if (ResponseOptionsBox)
    {
        ResponseOptionsBox->ClearChildren();
    }
}

void UDADialogueWidget::AddResponseOptions()
{
    if (!ResponseOptionsBox || !ResponseOptionWidgetClass)
    {
        return;
    }
    
    for (int32 i = 0; i < CurrentResponseOptions.Num(); ++i)
    {
        UDAResponseOptionWidget* ResponseWidget = CreateWidget<UDAResponseOptionWidget>(GetOwningPlayer(), ResponseOptionWidgetClass);
        if (ResponseWidget)
        {
            ResponseWidget->ResponseIndex = i;
            ResponseWidget->OnResponseOptionClicked.AddDynamic(this, &UDADialogueWidget::OnResponseReceived);

            // Find the text block in the response widget
            UTextBlock* ResponseText = Cast<UTextBlock>(ResponseWidget->GetWidgetFromName(TEXT("ResponseText")));
            if(ResponseText)
            {
                ResponseText->SetText(CurrentResponseOptions[i]);
            }
            
            ResponseOptionsBox->AddChild(ResponseWidget);
        }
    }
}