#include "Items/DAConsumableItem.h"
#include "Components/StatlineComponent.h"
#include "BaseClass/DACharacter.h"
#include "GameFramework/Pawn.h"
#include "Core/DAGameInstance.h"
#include "Engine/DataTable.h"

ADAConsumableItem::ADAConsumableItem()
{
}

void ADAConsumableItem::Consume(APawn* InstigatorPawn)
{
    if (!InstigatorPawn)
    {
        return;
    }

    ADACharacter* Character = Cast<ADACharacter>(InstigatorPawn);
    if (!Character)
    {
        return;
    }

    UStatlineComponent* StatlineComponent = Character->GetStatlineComponent();
    if (StatlineComponent)
    {
        for (const FStatModifier& Modifier : ItemData.StatModifiers)
        {
            if (FMath::Abs(Modifier.Value) > KINDA_SMALL_NUMBER)
            {
                StatlineComponent->UpdateStat(Modifier.StatName, Modifier.Value);
            }
        }

        if (ItemData.DiseaseToCure != NAME_None)
        {
            StatlineComponent->RemoveStatusEffect(ItemData.DiseaseToCure);
        }
        
        if (UDAGameInstance* GameInstance = GetGameInstance<UDAGameInstance>())
        {
            if (UDataTable* StatusEffectTable = GameInstance->GetStatusEffectDataTable())
            {
                for (const FName& EffectName : ItemData.StatusEffectsToApply)
                {
                    FDataTableRowHandle EffectRowHandle;
                    EffectRowHandle.DataTable = StatusEffectTable;
                    EffectRowHandle.RowName = EffectName;
                    StatlineComponent->ApplyStatusEffect(EffectRowHandle, InstigatorPawn);
                }
            }
        }
    }

    // Destroy the item actor after it has been consumed.
    Destroy();
}