#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DAUserInterface.generated.h"

/**
 * Base class for all Dark Age UI widgets
 */
UCLASS(Abstract)
class DARKAGE_API UDAUserInterface : public UUserWidget
{
    GENERATED_BODY()

public:
    UDAUserInterface(const FObjectInitializer& ObjectInitializer);

    // Initialize the widget with any required data
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void InitializeWidget();

    // Update the widget with new data
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void UpdateWidget();

    // Show the widget with animation if available
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void ShowWidget();

    // Hide the widget with animation if available
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void HideWidget();

    // Check if the widget is currently visible
    UFUNCTION(BlueprintPure, Category = "UI")
    bool IsWidgetVisible() const;

protected:
    // Whether the widget is currently visible
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    bool bIsVisible;

    // Animation to play when showing the widget (optional)
    UPROPERTY(Transient, BlueprintReadWrite)
    UWidgetAnimation* ShowAnimation;

    // Animation to play when hiding the widget (optional)
    UPROPERTY(Transient, BlueprintReadWrite)
    UWidgetAnimation* HideAnimation;

    // Called when the widget is constructed
    virtual void NativeConstruct() override;

    // Called when the widget is destroyed
    virtual void NativeDestruct() override;
};