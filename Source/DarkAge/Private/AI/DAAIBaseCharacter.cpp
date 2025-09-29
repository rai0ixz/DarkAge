#include "AI/DAAIBaseCharacter.h"
#include "Components/AIMoraleComponent.h"

ADAAIBaseCharacter::ADAAIBaseCharacter()
{
	// The MoraleComponent is created in the parent DACharacter, so this is empty.
	CharacterID = FGuid::NewGuid();
}

UAIMoraleComponent* ADAAIBaseCharacter::GetMoraleComponent() const
{
	return FindComponentByClass<UAIMoraleComponent>();
}

void ADAAIBaseCharacter::Surrender()
{
	UE_LOG(LogTemp, Warning, TEXT("%s surrenders!"), *GetName());
}

void ADAAIBaseCharacter::DropWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("%s drops their weapon."), *GetName());
}

FGuid ADAAIBaseCharacter::GetCharacterID() const
{
	return CharacterID;
}