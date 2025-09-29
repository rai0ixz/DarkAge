#include "UI/DAInteractionPromptWidget.h"
#include "Components/TextBlock.h"

UDAInteractionPromptWidget::UDAInteractionPromptWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDAInteractionPromptWidget::InitializeWidget()
{
    Super::InitializeWidget();
    
    // Set initial text
    if (PromptText)
    {
        PromptText->SetText(CurrentPromptText);
    }
    
    if (ActionText)
    {
        ActionText->SetText(CurrentActionText);
    }
}

void UDAInteractionPromptWidget::SetPromptText(const FText& Text)
{
    CurrentPromptText = Text;
    
    if (PromptText)
    {
        PromptText->SetText(CurrentPromptText);
    }
}

void UDAInteractionPromptWidget::SetActionText(const FText& Text)
{
    CurrentActionText = Text;
    
    if (ActionText)
    {
        ActionText->SetText(CurrentActionText);
    }
}