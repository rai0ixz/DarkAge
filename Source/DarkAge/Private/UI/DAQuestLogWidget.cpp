#include "UI/DAQuestLogWidget.h"
#include "Components/DAQuestLogComponent.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Items/DAQuestItem.h"
#include "Data/QuestData.h"
#include "UI/SubWidgets/DAQuestEntryWidget.h"
#include "Core/QuestManagementSubsystem.h"
#include "Core/QuestSystem.h"
#include "Kismet/GameplayStatics.h"

void UDAQuestLogWidget::Init(UDAQuestLogComponent* QuestLogComponent)
{
	if (QuestLogComponent)
	{
		QuestLog = QuestLogComponent;
		QuestLog->OnQuestStateChanged.AddDynamic(this, &UDAQuestLogWidget::OnQuestStateChanged);
		UpdateQuestList();
	}
}

void UDAQuestLogWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDAQuestLogWidget::OnQuestStateChanged(FName QuestID, EQuestState NewState)
{
	UpdateQuestList();
}

void UDAQuestLogWidget::UpdateQuestList()
{
	if (QuestListBox && QuestLog && QuestEntryWidgetClass)
	{
		QuestListBox->ClearChildren();

		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			UQuestManagementSubsystem* QuestManager = GameInstance->GetSubsystem<UQuestManagementSubsystem>();
			if (QuestManager)
			{
				TArray<FQuestLogEntry> ActiveQuests = QuestLog->GetActiveQuests();
				for (const FQuestLogEntry& LogEntry : ActiveQuests)
				{
					FQuestData QuestData;
					if (QuestManager->GetQuestData(LogEntry.QuestID, QuestData))
					{
						UDAQuestEntryWidget* QuestEntry = CreateWidget<UDAQuestEntryWidget>(this, QuestEntryWidgetClass);
						if (QuestEntry)
						{
							QuestEntry->SetQuestName(QuestData.QuestName);

							// Get current stage and objectives
							FQuestStage CurrentStage;
							if (QuestManager->GetQuestStage(LogEntry.QuestID, CurrentStage))
							{
								QuestEntry->SetStageName(CurrentStage.StageName);
								TArray<FText> ObjectiveTexts;
								for (const FBasicQuestObjective& Objective : CurrentStage.Objectives)
								{
									// Here you could format the text to include progress, e.g., " (0/5)"
									ObjectiveTexts.Add(Objective.Description);
								}
								QuestEntry->SetObjectives(ObjectiveTexts);
							}

							// Set required items
							TArray<FText> RequiredItemTexts;
							for (const auto& ItemClass : QuestData.RequiredItems)
							{
								if (ItemClass)
								{
									UDAQuestItem* DefaultItem = ItemClass->GetDefaultObject<UDAQuestItem>();
									if (DefaultItem)
									{
										RequiredItemTexts.Add(FText::FromName(DefaultItem->ItemID));
									}
								}
							}
							QuestEntry->SetRequiredItems(RequiredItemTexts);

							QuestListBox->AddChildToVerticalBox(QuestEntry);
						}
					}
				}
			}
		}
	}
}