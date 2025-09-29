#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DABaseNPC.generated.h"

class UDialogueComponent;
class UInventoryComponent;

UCLASS()
class DARKAGE_API ADABaseNPC : public ACharacter
{
    GENERATED_BODY()

public:
    ADABaseNPC();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UDialogueComponent* DialogueComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInventoryComponent* InventoryComponent;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Details")
    FName NPC_ID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Details")
    FText DisplayName;
};