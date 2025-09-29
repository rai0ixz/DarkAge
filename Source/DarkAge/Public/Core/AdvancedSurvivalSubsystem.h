#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/AdvancedSurvivalData.h"
#include "AdvancedSurvivalSubsystem.generated.h"

class ACharacter;
class UNeedsCalculationSubsystem;
class UDiseaseManagementComponent;

UCLASS()
class DARKAGE_API UAdvancedSurvivalSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	   /** If false, the entire survival simulation update will be skipped. Can be toggled with the 'ToggleSurvival' console command. */
	   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	   bool bSurvivalEnabled = true;

	//~ Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem interface

	// Tick/update all registered characters' survival needs
	virtual void Tick(float DeltaTime);

	// Client-side call, routes to server
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void RegisterCharacter(ACharacter* Character);
	// Server RPC for registering character
	UFUNCTION(Server, Reliable)
	void ServerRegisterCharacter(ACharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Survival")
	void UnregisterCharacter(ACharacter* Character);
	// Server RPC for unregistering character
	UFUNCTION(Server, Reliable)
	void ServerUnregisterCharacter(ACharacter* Character);

	// Replication (future-proofing for survival state)
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
   UPROPERTY(EditDefaultsOnly, Category = "Survival|Config")
   TMap<EClimateType, FClimateSurvivalInfo> ClimateSurvivalData;

	UPROPERTY()
	TSet<TWeakObjectPtr<ACharacter>> RegisteredCharacters;

	// Integration stubs for Statline, Disease, Climate
	void UpdateCharacterSurvival(ACharacter* Character, float DeltaTime);
};
