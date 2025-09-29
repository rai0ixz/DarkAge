#include "Characters/DATavernKeeperNPC.h"

ADATavernKeeperNPC::ADATavernKeeperNPC()
{
    NPC_ID = "NPC_TavernKeeper";
    DisplayName = FText::FromString("Tavern Keeper");
}
void ADATavernKeeperNPC::BeginFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADATavernKeeperNPC::EndFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADATavernKeeperNPC::OnInteract_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}