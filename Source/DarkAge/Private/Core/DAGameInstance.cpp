#include "Core/DAGameInstance.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "Data/ItemData.h"
#include "Data/FactionData.h"
#include "Data/QuestData.h"
#include "UObject/SoftObjectPath.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "Core/WorldPersistenceSystem.h"
#include "Core/QuestManagementSubsystem.h"
#include "TimerManager.h"

UDAGameInstance::UDAGameInstance()
	: LoadedItemDataTable(nullptr)
	, LoadedQuestDataTable(nullptr)
	, LoadedStatusEffectDataTable(nullptr)
{
}

void UDAGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Log, TEXT("UDAGameInstance::Init() called."));

    // Asynchronously load all data tables
    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
    TArray<FSoftObjectPath> ItemsToStream;

    if (ItemDataTablePtr.IsValid())
    {
        ItemsToStream.Add(ItemDataTablePtr.ToSoftObjectPath());
    }
    if (QuestDataTablePtr.IsValid())
    {
        ItemsToStream.Add(QuestDataTablePtr.ToSoftObjectPath());
    }
    if (StatusEffectDataTablePtr.IsValid())
    {
        ItemsToStream.Add(StatusEffectDataTablePtr.ToSoftObjectPath());
    }

    StreamableManager.RequestAsyncLoad(ItemsToStream, FStreamableDelegate::CreateUObject(this, &UDAGameInstance::OnDataTablesLoaded));
}

void UDAGameInstance::OnDataTablesLoaded()
{
    UE_LOG(LogTemp, Log, TEXT("All data tables loaded successfully."));

    // Safely assign the loaded data tables
    if (ItemDataTablePtr.IsValid())
    {
        LoadedItemDataTable = ItemDataTablePtr.Get();
        if (!LoadedItemDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get LoadedItemDataTable after async load."));
        }
    }

    if (QuestDataTablePtr.IsValid())
    {
        LoadedQuestDataTable = QuestDataTablePtr.Get();
        if (!LoadedQuestDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get LoadedQuestDataTable after async load."));
        }
    }

    if (StatusEffectDataTablePtr.IsValid())
    {
        LoadedStatusEffectDataTable = StatusEffectDataTablePtr.Get();
        if (!LoadedStatusEffectDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get LoadedStatusEffectDataTable after async load."));
        }
    }

    OnStartup();
}

void UDAGameInstance::OnStartup()
{
	// Subsystems are automatically initialized. No need for manual setup.

	// Trigger the initial questline
	UQuestManagementSubsystem* QuestSubsystem = GetSubsystem<UQuestManagementSubsystem>();
	if (QuestSubsystem)
	{
		// QuestSubsystem->TriggerFirstFewHoursQuestline();
	}
}

void UDAGameInstance::Shutdown()
{
	Super::Shutdown();
}

bool UDAGameInstance::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (!LoadedItemDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UDAGameInstance::GetItemData: LoadedItemDataTable is NULL. Check messages from UDAGameInstance::Init() during startup. ItemDataTablePtr path: '%s'."), *ItemDataTablePtr.ToSoftObjectPath().ToString());
		return false;
	}

	FString ContextString(TEXT("Getting Item Data"));
	FItemData* Row = LoadedItemDataTable->FindRow<FItemData>(ItemID, ContextString);

	if (Row)
	{
		OutItemData = *Row;
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDAGameInstance::GetItemData: ItemID '%s' not found in the loaded ItemDataTable ('%s')."), *ItemID.ToString(), *GetNameSafe(LoadedItemDataTable));
	return false;
}

bool UDAGameInstance::GetQuestData(FName QuestID, FQuestData& OutQuestData) const
{
	if (!LoadedQuestDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UDAGameInstance::GetQuestData: LoadedQuestDataTable is NULL."));
		return false;
	}

	FString ContextString(TEXT("Getting Quest Data"));
	FQuestData* Row = LoadedQuestDataTable->FindRow<FQuestData>(QuestID, ContextString);

	if (Row)
	{
		OutQuestData = *Row;
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDAGameInstance::GetQuestData: QuestID '%s' not found in the loaded QuestDataTable."), *QuestID.ToString());
	return false;
}

void UDAGameInstance::RecordPlayerAction(uint8 ActionType, FVector Location, FName RegionID,
	float Magnitude, FName TargetID, const FString& AdditionalData)
{
	if (UWorldPersistenceSystem* WorldPersistence = GetSubsystem<UWorldPersistenceSystem>())
	{
		EWorldActionType TypeEnum = static_cast<EWorldActionType>(ActionType);
		WorldPersistence->RecordPlayerAction(TypeEnum, Location, RegionID, Magnitude, TargetID, AdditionalData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UDAGameInstance::RecordPlayerAction: WorldPersistenceSystem is NULL."));
	}
}