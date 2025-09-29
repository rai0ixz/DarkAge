#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SettingsManager.generated.h"

UENUM(BlueprintType)
enum class EColorBlindMode : uint8
{
    None UMETA(DisplayName = "None"),
    Protanopia UMETA(DisplayName = "Protanopia"),
    Deuteranopia UMETA(DisplayName = "Deuteranopia"),
    Tritanopia UMETA(DisplayName = "Tritanopia")
};

UCLASS(Blueprintable, Config=Game)
class DARKAGE_API USettingsManager : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    float MasterVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    float MusicVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    float SFXVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    float UIScale = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    EColorBlindMode ColorBlindMode = EColorBlindMode::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Settings")
    bool bSubtitlesEnabled = true;
    // Add more as needed (graphics, controls, etc.)

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void LoadSettings();
};
