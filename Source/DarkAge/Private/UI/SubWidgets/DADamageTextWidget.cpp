#include "UI/SubWidgets/DADamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDADamageTextWidget::SetWorldLocation(const FVector& WorldLocation)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
        {
            SetPositionInViewport(ScreenLocation);
        }
    }
}