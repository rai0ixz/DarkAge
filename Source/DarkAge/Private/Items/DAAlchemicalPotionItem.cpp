#include "Items/DAAlchemicalPotionItem.h"
#include "Components/StatusEffectComponent.h"
#include "Data/ItemData.h"
#include "Core/DAGameInstance.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void ADAAlchemicalPotionItem::Consume(APawn* InstigatorPawn)
{
    if (!InstigatorPawn)
    {
        return;
    }

    UDAGameInstance* GameInstance = Cast<UDAGameInstance>(UGameplayStatics::GetGameInstance(this));
    UStatusEffectComponent* StatusEffectComponent = InstigatorPawn->FindComponentByClass<UStatusEffectComponent>();

    if (GameInstance && StatusEffectComponent && ItemData.StatusEffectsToApply.Num() > 0)
    {
        UDataTable* StatusEffectTable = GameInstance->StatusEffectDataTablePtr.LoadSynchronous();
        if (StatusEffectTable)
        {
            for (const FName& EffectID : ItemData.StatusEffectsToApply)
            {
                FDataTableRowHandle EffectDataRowHandle;
                EffectDataRowHandle.DataTable = StatusEffectTable;
                EffectDataRowHandle.RowName = EffectID;
                StatusEffectComponent->ApplyEffect(EffectDataRowHandle);
            }
        }
    }

    // Call the parent implementation if it has any logic
    Super::Consume(InstigatorPawn);
}