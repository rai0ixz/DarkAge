// DAPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"  // For IAbilitySystemInterface
#include "GameplayEffectTypes.h"
#include "GAS/DASurvivalAttributeSet.h"  // For UDASurvivalAttributeSet
#include "Interfaces/PoliticalActorInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DAPlayerCharacter.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;
class UCraftingComponent;
class UPlayerSkillsComponent;
class UNotorietyComponent;
class UFactionReputationComponent;
class UDAQuestLogComponent;
class UDAInteractionComponent;
class UDiseaseManagementComponent;
class UInventoryComponent;
class UAdvancedSurvivalSubsystem;
class UDAUIManager;

UCLASS()
class DARKAGE_API ADAPlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IPoliticalActorInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

protected:
	// Components specific to the Player Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCraftingComponent> CraftingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerSkillsComponent> SkillsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNotorietyComponent> NotorietyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFactionReputationComponent> FactionReputationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDAQuestLogComponent> QuestLogComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDAInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDiseaseManagementComponent> DiseaseManagementComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

public:

ADAPlayerCharacter(const FObjectInitializer& ObjectInitializer);


	// --- Gameplay Actions ---
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter|Actions")
	void AttemptLockpick();

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns QuestLogComponent subobject **/
	FORCEINLINE UDAQuestLogComponent* GetQuestLogComponent() const { return QuestLogComponent; }

	FORCEINLINE UDiseaseManagementComponent* GetDiseaseManagementComponent() const { return DiseaseManagementComponent; }


	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	// Grant initial abilities/effects here
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void InitializeAbilities();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Political Actor Interface Implementation ---
	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool NegotiateTreaty_Implementation(ETreatyType TreatyType, FName Faction1ID, FName Faction2ID, int32 DurationDays) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool SabotageRelations_Implementation(FName TargetFaction1ID, FName TargetFaction2ID) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool InciteRebellion_Implementation(FName RegionID, FName RebelFactionID) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool SpreadPropaganda_Implementation(FName FactionID, FName RegionID) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool MediatePeace_Implementation(FName Faction1ID, FName Faction2ID) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual bool InfluencePoliticalEvent_Implementation(FGuid EventID, bool DesiredOutcome) override;

	UFUNCTION(BlueprintCallable, Category = "Political Actions")
	virtual int32 GetPoliticalInfluence_Implementation(FName FactionID) override;

	// --- Enhanced Input Properties ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;


	// --- Animation Montages ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> ShiverAnimationMontage;

	// Called when player should shiver due to cold
	void PlayShiverAnimation();

	// --- Input Action Handlers ---
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleInteract(const FInputActionValue& Value);

public:
	// --- Faction & Politics Debug ---
	UFUNCTION(Exec, Category = "Debug|Factions")
	void PrintFactionReputationDebug(FString FactionIDString);

	UFUNCTION(Exec, Category = "Debug|Factions")
	void ModifyFactionReputationDebug(FString FactionIDString, float Delta);

	UFUNCTION(Exec, Category = "Debug|Factions")
	void PrintFactionRelationshipsDebug(FString FactionIDString);

	UFUNCTION(Exec, Category = "Debug|Factions")
	void SimulateFactionAIDebug();

	UFUNCTION(Exec, Category = "Debug|Factions")
	void DeclareWarDebug(FString Faction1String, FString Faction2String);

	UFUNCTION(Exec, Category = "Debug|Factions")
	void FormAllianceDebug(FString Faction1String, FString Faction2String);
	
	UFUNCTION(Exec, Category = "Debug|Factions")
	void NegotiatePeaceDebug(FString Faction1String, FString Faction2String);
	
	// --- Additional Debug Commands ---
	UFUNCTION(Exec, Category = "Debug|AI")
	void TestAINeedsDebug();
	
	UFUNCTION(Exec, Category = "Debug|Politics")
	void TestPoliticalActionsDebug();
	
	UFUNCTION(Exec, Category = "Debug|Performance")
	void TestPerformanceDebug();
	
	// --- Survival & Stats Debug Commands ---
	UFUNCTION(Exec, Category = "Debug|Survival")
	void PrintPlayerStatsDebug();

	UFUNCTION(Exec, Category = "Debug|Survival")
	void ModifyStatDebug(FString StatName, float Delta);

	UFUNCTION(Exec, Category = "Debug|Survival")
	void TestSurvivalMechanicsDebug();

	UFUNCTION(Exec, Category = "Debug|Interaction")
	void TestInteractionDebug();


	// GAS attribute change callback
	void OnAttributeChanged(const FOnAttributeChangeData& Data);
};