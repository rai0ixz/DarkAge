#include "UI/SubWidgets/DAQuestEntryWidget.h"
#include "Components/TextBlock.h"

void UDAQuestEntryWidget::SetQuestName(const FText& InQuestName)
{
    if (QuestNameText)
    {
        QuestNameText->SetText(InQuestName);
    }
}

void UDAQuestEntryWidget::SetStageName(const FText& InStageName)
{
    if (StageNameText)
    {
        StageNameText->SetText(InStageName);
    }
}

void UDAQuestEntryWidget::SetRequiredItems(const TArray<FText>& InRequiredItems)
{
    if (RequiredItemsText)
    {
        FString ItemsString;
        for (const FText& Item : InRequiredItems)
        {
            ItemsString += Item.ToString() + TEXT("\n");
        }
        RequiredItemsText->SetText(FText::FromString(ItemsString));
    }
}

void UDAQuestEntryWidget::SetObjectives(const TArray<FText>& InObjectives)
{
    if (ObjectivesText)
    {
        FString ObjectivesString;
        for (const FText& Objective : InObjectives)
        {
            ObjectivesString += Objective.ToString() + TEXT("\n");
        }
        ObjectivesText->SetText(FText::FromString(ObjectivesString));
    }
}
