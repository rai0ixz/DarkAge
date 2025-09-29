#include "Core/PlayerProfileManager.h"
#include "Core/DAGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "BaseClass/DACharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/StatlineComponent.h"
#include "Components/DAQuestLogComponent.h"
#include "Components/PlayerSkillsComponent.h"

bool UPlayerProfileManager::SaveProfile(const FString& SlotName, UPlayerSaveGame* SaveData)
{
    if (!SaveData) return false;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        if (ADACharacter* PlayerCharacter = Cast<ADACharacter>(PlayerController->GetPawn()))
        {
            if (UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
            {
                SaveData->InventoryItems.Empty();
                for (const FInventorySlot& Slot : InventoryComponent->Items)
                {
                    FInventoryItem NewItem;
                    NewItem.ItemID = Slot.Item.ItemID.ToString();
                    NewItem.Quantity = Slot.Quantity;
                    SaveData->InventoryItems.Add(NewItem);
                }
            }
            SaveData->PlayerLocation = PlayerCharacter->GetActorLocation();
            SaveData->PlayerRotation = PlayerCharacter->GetActorRotation();

            if (UStatlineComponent* StatlineComponent = PlayerCharacter->FindComponentByClass<UStatlineComponent>())
            {
                SaveData->PlayerStats.Empty();
                for (const auto& StatEntry : StatlineComponent->GetAllStats())
                {
                    SaveData->PlayerStats.Add(StatEntry.StatName, StatEntry.Stat);
                }
            }

            if (UDAQuestLogComponent* QuestLogComponent = PlayerCharacter->FindComponentByClass<UDAQuestLogComponent>())
            {
                SaveData->QuestLog.Empty();
                for (const auto& QuestEntry : QuestLogComponent->GetQuestLog())
                {
                    SaveData->QuestLog.Add(QuestEntry.QuestID, QuestEntry);
                }
               }
            
               if (UPlayerSkillsComponent* SkillsComponent = PlayerCharacter->FindComponentByClass<UPlayerSkillsComponent>())
               {
                SaveData->PlayerSkills = SkillsComponent->GetAllSkillsData();
               }
        }
    }

    SaveData->LastSaveTime = FDateTime::Now();
    return UGameplayStatics::SaveGameToSlot(SaveData, SlotName, 0);
}

UPlayerSaveGame* UPlayerProfileManager::LoadProfile(const FString& SlotName)
{
    if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, 0))
    {
        if (UPlayerSaveGame* SaveData = Cast<UPlayerSaveGame>(Loaded))
        {
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
            if (PlayerController)
            {
                if (ADACharacter* PlayerCharacter = Cast<ADACharacter>(PlayerController->GetPawn()))
                {
                    if (UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
                    {
                        InventoryComponent->Items.Empty();
                        for (const FInventoryItem& Item : SaveData->InventoryItems)
                        {
                            FItemData ItemData;
                            UDAGameInstance* GameInstance = Cast<UDAGameInstance>(GetGameInstance());
                            if (GameInstance && GameInstance->GetItemData(FName(*Item.ItemID), ItemData))
                            {
                                InventoryComponent->AddItem(ItemData, Item.Quantity);
                            }
                        }
                    }
                    PlayerCharacter->SetActorLocationAndRotation(SaveData->PlayerLocation, SaveData->PlayerRotation);

                    if (UStatlineComponent* StatlineComponent = PlayerCharacter->FindComponentByClass<UStatlineComponent>())
                    {
                        TArray<FStatEntry> Stats;
                        for (const auto& Pair : SaveData->PlayerStats)
                        {
                            Stats.Add({ Pair.Key, Pair.Value });
                        }
                        StatlineComponent->SetStats(Stats);
                    }

                    if (UDAQuestLogComponent* QuestLogComponent = PlayerCharacter->FindComponentByClass<UDAQuestLogComponent>())
                    {
                        TArray<FQuestLogEntry> Quests;
                        for (const auto& Pair : SaveData->QuestLog)
                        {
                            Quests.Add(Pair.Value);
                        }
                        QuestLogComponent->SetQuestLog(Quests);
                       }
                  
                       if (UPlayerSkillsComponent* SkillsComponent = PlayerCharacter->FindComponentByClass<UPlayerSkillsComponent>())
                       {
                        SkillsComponent->SetPlayerSkills(SaveData->PlayerSkills);
                    }
                }
            }
            return SaveData;
        }
    }
    return nullptr;
}

TArray<FString> UPlayerProfileManager::GetAllSaveSlots() const
{
    // Unreal does not provide a built-in way to enumerate slots; implement with a custom system or platform API as needed.
    return {};
}

void UPlayerProfileManager::DeleteProfile(const FString& SlotName)
{
    UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}
