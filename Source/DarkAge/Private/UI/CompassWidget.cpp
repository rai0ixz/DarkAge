#include "UI/CompassWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UCompassWidget::UpdateCompass(float PlayerYaw)
{
    if (CompassImage)
    {
        UMaterialInstanceDynamic* CompassMaterial = Cast<UMaterialInstanceDynamic>(CompassImage->GetBrush().GetResourceObject());
        if (!CompassMaterial)
        {
            UMaterialInterface* Material = Cast<UMaterialInterface>(CompassImage->GetBrush().GetResourceObject());
            if (Material)
            {
                CompassMaterial = UMaterialInstanceDynamic::Create(Material, CompassImage);
                CompassImage->SetBrushFromMaterial(CompassMaterial);
            }
        }

        if (CompassMaterial)
        {
            CompassMaterial->SetScalarParameterValue("RotationAngle", PlayerYaw);
        }
    }
}