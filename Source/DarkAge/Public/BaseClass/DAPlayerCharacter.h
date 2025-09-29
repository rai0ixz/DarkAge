// DAPlayerCharacter.h
// Clean recreation with all fixes and improvements

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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
class UStatlineComponent;
class UInventoryComponent;
class UAdvancedSurvivalSubsystem;
class UDAUIManager;

UCLASS()
class DARKAGE_API ADAPlayerCharacter : public ACharacter, public IPoliticalActorInterface
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
	TObjectPtr<UStatlineComponent> StatlineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

public:
	ADAPlayerCharacter();

	// Called by StatlineComponent when shiver should occur
	void PlayShiverAnimation();

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

	FORCEINLINE UStatlineComponent* GetStatlineComponent() const { return StatlineComponent; }

	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

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

	// Stat change callback
	void OnStatChanged(FName StatName, float NewValue);
};