#include "Characters/DALandownerNPC.h"
#include "Components/InventoryComponent.h"
#include "UI/DAUserInterface.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ADALandownerNPC::ADALandownerNPC()
{
    NPC_ID = "NPC_Landowner";
    DisplayName = FText::FromString("Landowner");
}
void ADALandownerNPC::BeginFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADALandownerNPC::EndFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADALandownerNPC::OnInteract_Implementation(AActor* InteractingActor)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		BeginTrade(PlayerController);
	}
}

void ADALandownerNPC::BeginTrade(APlayerController* PlayerController)
{
	if (!TradeWidget)
	{
		      // This should be a UPROPERTY(EditDefaultsOnly) on the class
		if (TSubclassOf<UDAUserInterface> TradeWidgetClass = LoadClass<UDAUserInterface>(nullptr, TEXT("/Game/_DA/UI/WBP_Trade.WBP_Trade_C")))
		{
			TradeWidget = CreateWidget<UDAUserInterface>(PlayerController, TradeWidgetClass);
		}
	}

	if (TradeWidget)
	{
		TradeWidget->AddToViewport();
		PlayerController->SetShowMouseCursor(true);
		
		      UInventoryComponent* PlayerInventory = PlayerController->GetPawn()->FindComponentByClass<UInventoryComponent>();
		      UInventoryComponent* VendorInventory = this->FindComponentByClass<UInventoryComponent>();

		      // Assuming the TradeWidget has these functions
		      // TradeWidget->SetInventories(VendorInventory, PlayerInventory);
	}
}