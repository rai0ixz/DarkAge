#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundBase.h"
#include "GameNotification.generated.h"

UENUM(BlueprintType)
enum class ENotificationCategory : uint8
{
    General     UMETA(DisplayName = "General"),
    Quest       UMETA(DisplayName = "Quest"),
    Discovery   UMETA(DisplayName = "Discovery"),
    World       UMETA(DisplayName = "World"),
    System      UMETA(DisplayName = "System"),
    Item        UMETA(DisplayName = "Item"),
    Error       UMETA(DisplayName = "Error")
};

USTRUCT(BlueprintType)
struct FGameNotification
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Message;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENotificationCategory Category = ENotificationCategory::General;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* Sound = nullptr;
};
