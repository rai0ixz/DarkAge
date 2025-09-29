#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DAQuestEntryWidget.generated.h"

class UTextBlock;

UCLASS()
class DARKAGE_API UDAQuestEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetQuestName(const FText& InQuestName);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetStageName(const FText& InStageName);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetObjectives(const TArray<FText>& InObjectives);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetRequiredItems(const TArray<FText>& InRequiredItems);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StageNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ObjectivesText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RequiredItemsText;
};
