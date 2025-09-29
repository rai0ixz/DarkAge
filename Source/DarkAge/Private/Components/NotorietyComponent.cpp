#include "Components/NotorietyComponent.h"

UNotorietyComponent::UNotorietyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNotorietyComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNotorietyComponent::RecordCrime(ECrimeType CrimeCommitted, float NotorietyValue, const FString& RegionID)
{
	if (NotorietyValue <= 0)
	{
		return;
	}

	float& CurrentNotorietyInRegion = RegionalNotorietyMap.FindOrAdd(RegionID, 0.0f);
	CurrentNotorietyInRegion += NotorietyValue;

	// For now, we just log this. This will later trigger more complex systems.
	// Using GetEnumValueAsString for a cleaner log message.
	const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/DarkAge.ECrimeType"), true);
	FString CrimeString = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CrimeCommitted)) : TEXT("UnknownCrime");

	UE_LOG(LogTemp, Warning, TEXT("NotorietyComponent: Crime '%s' recorded in region '%s'. Notoriety added: %.2f. Total for region: %.2f"),
		*CrimeString, *RegionID, NotorietyValue, CurrentNotorietyInRegion);
}

float UNotorietyComponent::GetNotorietyInRegion(const FString& RegionID) const
{
	const float* FoundNotoriety = RegionalNotorietyMap.Find(RegionID);
	return FoundNotoriety ? *FoundNotoriety : 0.0f;
}