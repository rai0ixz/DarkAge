#include "UI/MinimapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Canvas.h"
#include "Materials/MaterialInstanceDynamic.h"

void UMinimapWidget::UpdateMinimap(FVector PlayerLocation, FRotator PlayerRotation)
{
    if (MinimapImage)
    {
        UMaterialInstanceDynamic* MinimapMaterial = Cast<UMaterialInstanceDynamic>(MinimapImage->GetBrush().GetResourceObject());
        if (!MinimapMaterial)
        {
            UMaterialInterface* Material = Cast<UMaterialInterface>(MinimapImage->GetBrush().GetResourceObject());
            if (Material)
            {
                MinimapMaterial = UMaterialInstanceDynamic::Create(Material, MinimapImage);
                MinimapImage->SetBrushFromMaterial(MinimapMaterial);
            }
        }

        if (MinimapMaterial)
        {
            MinimapMaterial->SetVectorParameterValue("PlayerLocation", FLinearColor(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z, 0));
            MinimapMaterial->SetScalarParameterValue("PlayerRotation", PlayerRotation.Yaw);
        }
    }

    if (IconCanvas)
    {
        IconCanvas->ClearChildren();
    }
}

void UMinimapWidget::AddIcon(FVector WorldLocation, EMinimapIconType IconType)
{
    if (!IconCanvas || !IconDataTable)
    {
        return;
    }

    FMinimapIconData* IconData = IconDataTable->FindRow<FMinimapIconData>(*UEnum::GetValueAsString(IconType), TEXT(""));
    if (!IconData)
    {
        return;
    }

    UImage* NewIcon = NewObject<UImage>(this);
    NewIcon->SetBrushFromTexture(IconData->IconTexture.LoadSynchronous());

    UCanvasPanelSlot* CanvasSlot = IconCanvas->AddChildToCanvas(NewIcon);

    // Assuming the minimap texture corresponds to a square region in the world
    // defined by WorldMapBounds, centered at WorldMapCenter.
    FVector2D WorldMapBounds(10000.0f, 10000.0f); // Example: 10000x10000 units in world space
    FVector2D WorldMapCenter(0.0f, 0.0f);

    // Get the size of the canvas panel
    FVector2D CanvasSize = IconCanvas->GetCachedGeometry().GetLocalSize();

    // Translate world location to be relative to the map center
    FVector2D RelativeLocation = FVector2D(WorldLocation.X - WorldMapCenter.X, WorldLocation.Y - WorldMapCenter.Y);

    // Convert to a normalized UV coordinate (0 to 1)
    float U = (RelativeLocation.X / WorldMapBounds.X) + 0.5f;
    float V = (RelativeLocation.Y / WorldMapBounds.Y) + 0.5f;

    // Scale by the canvas size to get the final position
    FVector2D MinimapCoordinates(U * CanvasSize.X, V * CanvasSize.Y);

    // Center the icon on the coordinates
    MinimapCoordinates -= CanvasSize * 0.5f;

    CanvasSlot->SetPosition(MinimapCoordinates);
}