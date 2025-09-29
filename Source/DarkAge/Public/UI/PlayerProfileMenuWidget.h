#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/PlayerSaveGame.h"
#include "PlayerProfileMenuWidget.generated.h"

UCLASS()
class DARKAGE_API UPlayerProfileMenuWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Profile")
    void RefreshProfileList(const TArray<FString>& SlotNames);

    UFUNCTION(BlueprintCallable, Category = "Profile")
    void RequestLoadProfile(const FString& SlotName);
    UFUNCTION(BlueprintCallable, Category = "Profile")
    void RequestDeleteProfile(const FString& SlotName);
    UFUNCTION(BlueprintCallable, Category = "Profile")
    void RequestCreateProfile(const FString& PlayerName);
};
