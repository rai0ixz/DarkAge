#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CrimeTypes.h"
#include "NotorietyComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKAGE_API UNotorietyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNotorietyComponent();

	UFUNCTION(BlueprintCallable, Category = "Notoriety")
	void RecordCrime(ECrimeType CrimeCommitted, float NotorietyValue, const FString& RegionID = TEXT("DefaultRegion"));

	UFUNCTION(BlueprintPure, Category = "Notoriety")
	float GetNotorietyInRegion(const FString& RegionID) const;

protected:
	virtual void BeginPlay() override;

	// A map to store the player's notoriety level in different regions
	// FString for RegionID, float for NotorietyAmount
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Notoriety", meta = (AllowPrivateAccess = "true"))
	TMap<FString, float> RegionalNotorietyMap;
};