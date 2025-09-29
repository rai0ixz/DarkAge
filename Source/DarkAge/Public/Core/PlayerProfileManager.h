#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/PlayerSaveGame.h"
#include "PlayerProfileManager.generated.h"

UCLASS()
class DARKAGE_API UPlayerProfileManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Profile")
    bool SaveProfile(const FString& SlotName, UPlayerSaveGame* SaveData);
    UFUNCTION(BlueprintCallable, Category = "Profile")
    UPlayerSaveGame* LoadProfile(const FString& SlotName);
    UFUNCTION(BlueprintCallable, Category = "Profile")
    TArray<FString> GetAllSaveSlots() const;
    UFUNCTION(BlueprintCallable, Category = "Profile")
    void DeleteProfile(const FString& SlotName);
};
