#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DAQuestLogWidget.generated.h"

class UDAQuestLogComponent;
class UVerticalBox;
class UDAQuestEntryWidget; // Forward declaration

UCLASS()
class DARKAGE_API UDAQuestLogWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void Init(UDAQuestLogComponent* QuestLogComponent);

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnQuestStateChanged(FName QuestID, EQuestState NewState);

    void UpdateQuestList();

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* QuestListBox;

    UPROPERTY()
    UDAQuestLogComponent* QuestLog;

    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (DisplayName = "Quest Entry Widget Class"))
    TSubclassOf<UDAQuestEntryWidget> QuestEntryWidgetClass;
};