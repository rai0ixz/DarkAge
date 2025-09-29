#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GovernanceData.h"
#include "Data/PoliticalData.h"
#include "PoliticalSystem.generated.h"

class UFactionManagerSubsystem;

USTRUCT(BlueprintType)
struct FPoliticalEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid EventID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPoliticalEventType EventType = EPoliticalEventType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PrimaryFactionID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SecondaryFactionID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RegionID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Intensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> InvolvedFactions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Severity = 0.0f;
};

USTRUCT(BlueprintType)
struct FPoliticalTreaty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid TreatyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TreatyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETreatyType TreatyType = ETreatyType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> InvolvedFactions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime EstablishedDate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime ExpirationDate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TreatyTerms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Signatories;
};

USTRUCT(BlueprintType)
struct FDiplomaticMission
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SourceFaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetFaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MissionObjective;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoliticalEventOccurred, const FPoliticalEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTreatyChanged, const FPoliticalTreaty&, Treaty);

UCLASS()
class DARKAGE_API UPoliticalSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UPoliticalSystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void CalculateFactionDiplomaticInfluence(FName FactionName);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	FGuid CreateTreaty(ETreatyType TreatyType, const TArray<FName>& InvolvedFactions, const FString& TreatyTerms, int32 DurationDays);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	FGuid CreatePoliticalEvent(EPoliticalEventType EventType, FName Faction1ID, FName Faction2ID, FName RegionID, float Intensity, int32 Duration);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	float CalculateRegionalPoliticalStability(FName RegionID);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	bool NegotiatePeace(FName Faction1ID, FName Faction2ID, const FString& PeaceTerms);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	TArray<FPoliticalEvent> GetActivePoliticalEvents();

	UFUNCTION(BlueprintCallable, Category = "Political System")
	bool ResolvePoliticalEvent(FGuid EventID, bool bSuccess, const FString& Outcome);

	UFUNCTION(BlueprintCallable, Category = "Political System")
	TArray<FPoliticalTreaty> GetAllActiveTreaties();

	UFUNCTION(BlueprintCallable, Category = "Political System")
	TArray<FPoliticalTreaty> GetFactionTreaties(FName FactionID);

public:
	UPROPERTY(BlueprintAssignable, Category = "Political System")
	FOnPoliticalEventOccurred OnPoliticalEventOccurred;

	UPROPERTY(BlueprintAssignable, Category = "Political System")
	FOnTreatyChanged OnTreatyChanged;

private:
	UPROPERTY()
	TArray<FPoliticalEvent> ActivePoliticalEvents;

	UPROPERTY()
	TArray<FPoliticalTreaty> ActiveTreaties;

	UPROPERTY()
	TArray<FDiplomaticMission> ActiveDiplomaticMissions;

	UPROPERTY()
	TMap<FName, float> FactionInfluences;

	UPROPERTY()
	TObjectPtr<UFactionManagerSubsystem> FactionManager;
};