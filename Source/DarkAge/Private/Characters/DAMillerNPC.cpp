#include "Characters/DAMillerNPC.h"

ADAMillerNPC::ADAMillerNPC()
{
    NPC_ID = "NPC_Miller";
    DisplayName = FText::FromString("Miller");
}
void ADAMillerNPC::BeginFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADAMillerNPC::EndFocus_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}

void ADAMillerNPC::OnInteract_Implementation(AActor* InteractingActor)
{
	// Handled in Blueprint
}