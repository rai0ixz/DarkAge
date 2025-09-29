#pragma once

#include "CoreMinimal.h"
#include "BaseClass/DACharacter.h"
#include "DAAIBaseCharacter.generated.h"

class UAIMoraleComponent;

UCLASS()
class DARKAGE_API ADAAIBaseCharacter : public ADACharacter
{
	GENERATED_BODY()

public:
	ADAAIBaseCharacter();

	// These functions must be public so the controller can call them.
	void Surrender();
	void DropWeapon();

	UAIMoraleComponent* GetMoraleComponent() const;

	FGuid GetCharacterID() const;

private:
	UPROPERTY()
	FGuid CharacterID;
};