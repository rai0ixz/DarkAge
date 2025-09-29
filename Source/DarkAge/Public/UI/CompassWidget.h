#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassWidget.generated.h"

class UImage;

UCLASS()
class DARKAGE_API UCompassWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Compass")
    void UpdateCompass(float PlayerYaw);

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* CompassImage;
};