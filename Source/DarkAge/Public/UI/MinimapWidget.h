#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MinimapIconData.h"
#include "MinimapWidget.generated.h"

class UImage;
class UCanvasPanel;
class UDataTable;

UCLASS()
class DARKAGE_API UMinimapWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void UpdateMinimap(FVector PlayerLocation, FRotator PlayerRotation);

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void AddIcon(FVector WorldLocation, EMinimapIconType IconType);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
    UDataTable* IconDataTable;

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapImage;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* IconCanvas;
};