#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Templates/SubclassOf.h"
#include "Engine/DataTable.h" // Required for UDataTable
#include "Data/ItemData.h"    // Required for FItemData (ensure correct path)
#include "DAGameInstance.generated.h"

// Forward declarations
class UAdvancedSurvivalSubsystem;
class UWorldPersistenceSystem;

UCLASS()
class DARKAGE_API UDAGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UDAGameInstance();

    // DataTable containing all item definitions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Tables")
    TSoftObjectPtr<UDataTable> ItemDataTablePtr; // Use TSoftObjectPtr for safe loading
    
    // DataTable containing all faction definitions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Tables")
    TSoftObjectPtr<UDataTable> FactionDefinitionsTablePtr;

    // DataTable containing all quest definitions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Tables")
    TSoftObjectPtr<UDataTable> QuestDataTablePtr;

    // DataTable containing all status effect definitions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Tables")
    TSoftObjectPtr<UDataTable> StatusEffectDataTablePtr;

    // Helper function to get item data by its ID
    // Returns true if item data was found, false otherwise
    UFUNCTION(BlueprintCallable, Category = "Items")
    bool GetItemData(FName ItemID, FItemData& OutItemData) const;

    // Helper function to get quest data by its ID
    // Returns true if quest data was found, false otherwise
    UFUNCTION(BlueprintCallable, Category = "Quests")
    bool GetQuestData(FName QuestID, FQuestData& OutQuestData) const;

    UFUNCTION(BlueprintPure, Category = "Data Tables")
    UDataTable* GetItemDataTable() const { return LoadedItemDataTable; }

    UFUNCTION(BlueprintPure, Category = "Data Tables")
    UDataTable* GetStatusEffectDataTable() const { return LoadedStatusEffectDataTable; }
    
    // Record a player action in the world persistence system
    UFUNCTION(BlueprintCallable, Category = "World Persistence")
    void RecordPlayerAction(uint8 ActionType, FVector Location, FName RegionID, 
        float Magnitude, FName TargetID, const FString& AdditionalData);

protected:
    // To store the loaded DataTable for quicker access
    UPROPERTY()
    UDataTable* LoadedItemDataTable;

    UPROPERTY()
    UDataTable* LoadedQuestDataTable;

    UPROPERTY()
    UDataTable* LoadedStatusEffectDataTable;

    // UGameInstance interface
    virtual void Init() override;
    virtual void Shutdown() override;

private:
    void OnStartup();

    void OnDataTablesLoaded();
};