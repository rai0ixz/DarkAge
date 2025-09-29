#include "UI/DAUserInterface.h"
#include "Animation/WidgetAnimation.h"

UDAUserInterface::UDAUserInterface(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsVisible(false)
    , ShowAnimation(nullptr)
    , HideAnimation(nullptr)
{
}

void UDAUserInterface::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Initialize the widget when it's constructed
    InitializeWidget();
}

void UDAUserInterface::NativeDestruct()
{
    Super::NativeDestruct();
}

void UDAUserInterface::InitializeWidget()
{
    // Base implementation does nothing, override in child classes
}

void UDAUserInterface::UpdateWidget()
{
    // Base implementation does nothing, override in child classes
}

void UDAUserInterface::ShowWidget()
{
    if (bIsVisible)
    {
        return;
    }

    bIsVisible = true;
    SetVisibility(ESlateVisibility::Visible);

    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation);
    }
}

void UDAUserInterface::HideWidget()
{
    if (!bIsVisible)
    {
        return;
    }

    bIsVisible = false;

    if (HideAnimation)
    {
        PlayAnimation(HideAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
        
        // Set up a timer to hide the widget after the animation completes
        UWorld* World = GetWorld();
        if (World)
        {
            FTimerHandle TimerHandle;
            World->GetTimerManager().SetTimer(
                TimerHandle,
                [this]()
                {
                    SetVisibility(ESlateVisibility::Hidden);
                },
                HideAnimation->GetEndTime(),
                false
            );
        }
        else
        {
            // If world is not available, just hide immediately
            SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

bool UDAUserInterface::IsWidgetVisible() const
{
    return bIsVisible;
}