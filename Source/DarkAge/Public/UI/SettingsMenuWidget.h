#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SettingsManager.h"
#include "SettingsMenuWidget.generated.h"

UCLASS()
class DARKAGE_API USettingsMenuWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplySettings();
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void LoadSettings();
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();
    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    USettingsManager* SettingsManager;
};
