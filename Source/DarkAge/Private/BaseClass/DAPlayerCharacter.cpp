// DAPlayerCharacter.cpp
// Clean recreation with all fixes and improvements

#include "BaseClass/DAPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/InventoryComponent.h"
#include "Components/CraftingComponent.h"
#include "Components/PlayerSkillsComponent.h"
#include "Components/NotorietyComponent.h"
#include "Components/FactionReputationComponent.h"
#include "Components/DAQuestLogComponent.h"
#include "Components/DAInteractionComponent.h"
#include "Components/DiseaseManagementComponent.h"
#include "Core/FactionManagerSubsystem.h"
#include "AI/DAAIController.h"
#include "AI/DAAIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Core/DAGameInstance.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "UI/DAUIManager.h"
#include "Data/ItemData.h"
#include "Data/PlayerSkillData.h"
#include "Data/InventoryData.h"
#include "Core/DA_NPCManagerSubsystem.h"
#include "Components/AINeedsPlanningComponent.h"
#include "GAS/DASurvivalDepletionAbility.h"  // For UDASurvivalDepletionAbility

ADAPlayerCharacter::ADAPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Camera setup
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);
	CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Movement setup - FIX LEDGE WALKING ISSUE
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// CRITICAL: Disable all perching behavior to match ThirdPersonCharacter
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->PerchRadiusThreshold = 0.0f;
	GetCharacterMovement()->PerchAdditionalHeight = 0.0f;
	GetCharacterMovement()->LedgeCheckThreshold = 0.0f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->bForceNextFloorCheck = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create player-specific components
	CraftingComponent = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));
	SkillsComponent = CreateDefaultSubobject<UPlayerSkillsComponent>(TEXT("SkillsComponent"));
	NotorietyComponent = CreateDefaultSubobject<UNotorietyComponent>(TEXT("NotorietyComponent"));
	FactionReputationComponent = CreateDefaultSubobject<UFactionReputationComponent>(TEXT("FactionReputationComponent"));
	QuestLogComponent = CreateDefaultSubobject<UDAQuestLogComponent>(TEXT("QuestLogComponent"));
	InteractionComponent = CreateDefaultSubobject<UDAInteractionComponent>(TEXT("InteractionComponent"));
	DiseaseManagementComponent = CreateDefaultSubobject<UDiseaseManagementComponent>(TEXT("DiseaseManagementComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);  // Balanced for RPG
}

void ADAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// FORCE: Re-apply ledge settings in BeginPlay to ensure they take effect
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bCanWalkOffLedges = true;
		Movement->bCanWalkOffLedgesWhenCrouching = true;
		Movement->PerchRadiusThreshold = 0.0f;
		Movement->PerchAdditionalHeight = 0.0f;
		Movement->LedgeCheckThreshold = 0.0f;
		Movement->bUseFlatBaseForFloorChecks = true;
		Movement->bForceNextFloorCheck = true;
		Movement->SetMovementMode(MOVE_Walking);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// GAS attributes will be monitored via AbilitySystemComponent delegates
	if (AbilitySystemComponent)
	{
		// Bind to attribute changes for HUD updates
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDASurvivalAttributeSet::GetHealthAttribute()).AddUObject(this, &ADAPlayerCharacter::OnAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDASurvivalAttributeSet::GetStaminaAttribute()).AddUObject(this, &ADAPlayerCharacter::OnAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDASurvivalAttributeSet::GetHungerAttribute()).AddUObject(this, &ADAPlayerCharacter::OnAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDASurvivalAttributeSet::GetThirstAttribute()).AddUObject(this, &ADAPlayerCharacter::OnAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DAPlayerCharacter: No AbilitySystemComponent found!"));
	}

	// Register with survival subsystem
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UAdvancedSurvivalSubsystem* SurvivalSubsystem = GameInstance->GetSubsystem<UAdvancedSurvivalSubsystem>())
			{
				SurvivalSubsystem->RegisterCharacter(this);
			}
		}
	}

	if (AbilitySystemComponent)
	{
		// Initialize AttributeSets here (e.g., SurvivalAttributeSet)
		// Load DataTable for initial values
		UDataTable* AttributeDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_DA/Data/InitialAttributes.InitialAttributes"));
		if (AttributeDataTable)
		{
			AbilitySystemComponent->InitStats(UDASurvivalAttributeSet::StaticClass(), AttributeDataTable);
		}
		else
		{
			// Fallback to default values
			AbilitySystemComponent->InitStats(UDASurvivalAttributeSet::StaticClass(), nullptr);
		}

		InitializeAbilities();
	}
}

void ADAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADAPlayerCharacter::HandleMove);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADAPlayerCharacter::HandleLook);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADAPlayerCharacter::HandleInteract);
		}
	}
}

void ADAPlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADAPlayerCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADAPlayerCharacter::HandleInteract(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->Interact();
	}
}

void ADAPlayerCharacter::PlayShiverAnimation()
{
	if (ShiverAnimationMontage)
	{
		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(ShiverAnimationMontage))
		{
			AnimInstance->Montage_Play(ShiverAnimationMontage);
		}
	}
}
void ADAPlayerCharacter::InitializeAbilities()
{
	if (!HasAuthority() && !IsLocallyControlled()) return;  // Server/client handling

	// Example: Grant passive survival depletion ability
	FGameplayAbilitySpec DepletionSpec(UDASurvivalDepletionAbility::StaticClass(), 1, INDEX_NONE);
	AbilitySystemComponent->GiveAbility(DepletionSpec);

	// Grant other abilities, e.g., combat or crafting
	// FGameplayAbilitySpec CombatSpec(YourCombatAbilityClass::StaticClass(), 1, INDEX_NONE);
	// AbilitySystemComponent->GiveAbility(CombatSpec);
}

void ADAPlayerCharacter::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!AbilitySystemComponent || !IsValid(this))
	{
		return;
	}

	// Update HUD with current attribute values
	float Health = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHealthAttribute());
	float MaxHealth = AbilitySystemComponent->GetNumericAttributeBase(UDASurvivalAttributeSet::GetHealthAttribute());
	float Stamina = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetStaminaAttribute());
	float MaxStamina = AbilitySystemComponent->GetNumericAttributeBase(UDASurvivalAttributeSet::GetStaminaAttribute());
	float Hunger = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHungerAttribute());
	float MaxHunger = 100.0f; // Assuming max is 100
	float Thirst = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetThirstAttribute());
	float MaxThirst = 100.0f; // Assuming max is 100

	// Update HUD
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UDAUIManager* UIManager = GameInstance->GetSubsystem<UDAUIManager>())
			{
				UIManager->UpdatePlayerStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
			}
		}
	}
}

void ADAPlayerCharacter::AttemptLockpick()
{
	if (!SkillsComponent || !AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AttemptLockpick: Missing required components (SkillsComponent or AbilitySystemComponent)."));
		return;
	}

	float BaseSuccessChance = 50.0f;
	float BaseCritFailChance = 10.0f;

	const FName LockpickingSkillID = "SKILL_Lockpicking";
	const FName SteadyHandsPerkID = "PERK_LP01";

	if (SkillsComponent->HasPerk(LockpickingSkillID, SteadyHandsPerkID))
	{
		BaseCritFailChance -= 5.0f;
		UE_LOG(LogTemp, Log, TEXT("AttemptLockpick: 'Steady Hands' perk is active. Reducing critical failure chance."));
	}

	// Modify success chance based on Dexterity attribute
	float Dexterity = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetDexterityAttribute());
	BaseSuccessChance += (Dexterity - 10.0f) * 2.0f; // +2% per point above 10

	BaseSuccessChance = FMath::Clamp(BaseSuccessChance, 0.f, 100.f);
	BaseCritFailChance = FMath::Clamp(BaseCritFailChance, 0.f, 100.f);

	UE_LOG(LogTemp, Log, TEXT("--- Attempting Lockpick ---"));
	UE_LOG(LogTemp, Log, TEXT("Dexterity: %.1f"), Dexterity);
	UE_LOG(LogTemp, Log, TEXT("Final Success Chance: %.1f%%"), BaseSuccessChance);
	UE_LOG(LogTemp, Log, TEXT("Final Critical Failure (Noise) Chance: %.1f%%"), BaseCritFailChance);
}


// --- Faction Debug Commands ---
void ADAPlayerCharacter::PrintFactionReputationDebug(FString FactionIDString)
{
	if (FactionReputationComponent)
	{
		const float Reputation = FactionReputationComponent->GetFactionReputation(FName(*FactionIDString));
		UE_LOG(LogTemp, Log, TEXT("Faction: %s, Reputation: %f"), *FactionIDString, Reputation);
	}
}

void ADAPlayerCharacter::ModifyFactionReputationDebug(FString FactionIDString, float Delta)
{
	if (FactionReputationComponent)
	{
		FactionReputationComponent->ModifyFactionReputation(FName(*FactionIDString), Delta);
	}
}

void ADAPlayerCharacter::PrintFactionRelationshipsDebug(FString FactionIDString)
{
	if (!GetWorld()) return;
	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (FactionManager)
	{
		// FactionManager->PrintFactionRelationships(FName(*FactionIDString));
	}
}

void ADAPlayerCharacter::SimulateFactionAIDebug()
{
	if (!GetWorld()) return;
	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (FactionManager)
	{
		// FactionManager->UpdateFactionAI();
	}
}

int32 ADAPlayerCharacter::GetPoliticalInfluence_Implementation(FName FactionID)
{
	int32 BaseInfluence = 0;

	// Base influence from faction reputation
	if (FactionReputationComponent)
	{
		float Reputation = FactionReputationComponent->GetFactionReputation(FactionID);
		BaseInfluence += FMath::FloorToInt(Reputation * 0.5f); // 0.5 influence per reputation point
	}

	// Influence from player level/stats (if available) - using Strength as level proxy for now
	if (AbilitySystemComponent)
	{
		float PlayerStrength = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetStrengthAttribute());
		BaseInfluence += FMath::FloorToInt(PlayerStrength * 2.0f); // 2 influence per strength point
	}

	// Influence from notoriety (criminals have different political influence)
	if (NotorietyComponent)
	{
		// Use regional notoriety; default region if no regional system is wired yet
		float Notoriety = NotorietyComponent->GetNotorietyInRegion(TEXT("DefaultRegion"));
		if (Notoriety > 50.0f)
		{
			// High notoriety can give influence in certain circles but reduce it in others
			BaseInfluence += FMath::FloorToInt((Notoriety - 50.0f) * 0.3f);
		}
	}

	// Minimum influence of 10 for any character
	BaseInfluence = FMath::Max(BaseInfluence, 10);

	UE_LOG(LogTemp, Log, TEXT("Political Influence for %s: %d"), *FactionID.ToString(), BaseInfluence);
	return BaseInfluence;
}

// --- Missing Political Actor Interface Implementations ---
bool ADAPlayerCharacter::NegotiateTreaty_Implementation(ETreatyType TreatyType, FName Faction1ID, FName Faction2ID, int32 DurationDays)
{
	if (!GetWorld()) return false;

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if player has sufficient political influence
	int32 PlayerInfluence = GetPoliticalInfluence_Implementation(Faction1ID);
	if (PlayerInfluence < 50) // Minimum influence required
	{
		UE_LOG(LogTemp, Warning, TEXT("NegotiateTreaty: Insufficient political influence (%d < 50)"), PlayerInfluence);
		return false;
	}

	// Check current relationship between factions
	EFactionRelationshipType CurrentRelation = FactionManager->GetFactionRelationship(Faction1ID, Faction2ID);

	// Can't negotiate treaty if already allied or at war
	if (CurrentRelation == EFactionRelationshipType::Allied)
	{
		UE_LOG(LogTemp, Warning, TEXT("NegotiateTreaty: Factions are already allied"));
		return false;
	}

	// Attempt negotiation based on treaty type
	float SuccessChance = 0.4f; // Base 40% success rate
	SuccessChance += (PlayerInfluence - 50) * 0.01f; // +1% per point of influence above minimum

	if (FMath::FRand() < SuccessChance)
	{
		// Success - establish treaty
		switch (TreatyType)
		{
		case ETreatyType::Alliance:
			FactionManager->SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Allied);
			UE_LOG(LogTemp, Log, TEXT("Treaty Negotiation Success: Alliance formed between %s and %s"), *Faction1ID.ToString(), *Faction2ID.ToString());
			break;
		case ETreatyType::Trade:
			FactionManager->SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Neutral);
			UE_LOG(LogTemp, Log, TEXT("Treaty Negotiation Success: Trade agreement established between %s and %s"), *Faction1ID.ToString(), *Faction2ID.ToString());
			break;
		case ETreatyType::NonAggression:
			FactionManager->SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Neutral);
			UE_LOG(LogTemp, Log, TEXT("Treaty Negotiation Success: Non-aggression pact signed between %s and %s"), *Faction1ID.ToString(), *Faction2ID.ToString());
			break;
		}

		// Award reputation bonuses for successful diplomacy
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(Faction1ID, 10.0f);
			FactionReputationComponent->ModifyFactionReputation(Faction2ID, 10.0f);
		}

		return true;
	}
	else
	{
		// Failure - slight reputation penalty
		UE_LOG(LogTemp, Warning, TEXT("Treaty Negotiation Failed: %s and %s rejected the proposal"), *Faction1ID.ToString(), *Faction2ID.ToString());

		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(Faction1ID, -5.0f);
			FactionReputationComponent->ModifyFactionReputation(Faction2ID, -5.0f);
		}

		return false;
	}
}

bool ADAPlayerCharacter::SabotageRelations_Implementation(FName TargetFaction1ID, FName TargetFaction2ID)
{
	if (!GetWorld()) return false;

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if player has sufficient political influence
	int32 PlayerInfluence = GetPoliticalInfluence_Implementation(TargetFaction1ID);
	if (PlayerInfluence < 75) // Higher requirement for sabotage
	{
		UE_LOG(LogTemp, Warning, TEXT("SabotageRelations: Insufficient political influence (%d < 75)"), PlayerInfluence);
		return false;
	}

	// Check current relationship - can't sabotage if already hostile
	EFactionRelationshipType CurrentRelation = FactionManager->GetFactionRelationship(TargetFaction1ID, TargetFaction2ID);
	if (CurrentRelation == EFactionRelationshipType::Hostile)
	{
		UE_LOG(LogTemp, Warning, TEXT("SabotageRelations: Factions are already hostile"));
		return false;
	}

	// Calculate success chance based on influence and current relationship
	float BaseSuccessChance = 0.3f; // Base 30% success for sabotage
	BaseSuccessChance += (PlayerInfluence - 75) * 0.005f; // +0.5% per point above minimum

	// Relationship modifier - harder to sabotage allied factions
	if (CurrentRelation == EFactionRelationshipType::Allied)
	{
		BaseSuccessChance *= 0.5f; // 50% penalty for allied factions
	}

	if (FMath::FRand() < BaseSuccessChance)
	{
		// Success - damage relations
		EFactionRelationshipType NewRelation = EFactionRelationshipType::Hostile;
		if (CurrentRelation == EFactionRelationshipType::Allied)
		{
			NewRelation = EFactionRelationshipType::Neutral; // Downgrade from allied to neutral
		}

		FactionManager->SetFactionRelationship(TargetFaction1ID, TargetFaction2ID, NewRelation);

		UE_LOG(LogTemp, Log, TEXT("Sabotage Success: Relations between %s and %s have been damaged"),
			*TargetFaction1ID.ToString(), *TargetFaction2ID.ToString());

		// Reputation consequences
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(TargetFaction1ID, -15.0f);
			FactionReputationComponent->ModifyFactionReputation(TargetFaction2ID, -15.0f);
		}

		return true;
	}
	else
	{
		// Failure - severe reputation penalty
		UE_LOG(LogTemp, Warning, TEXT("Sabotage Failed: The plot was discovered!"));

		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(TargetFaction1ID, -25.0f);
			FactionReputationComponent->ModifyFactionReputation(TargetFaction2ID, -25.0f);
		}

		// Possible notoriety increase for failed sabotage
		if (NotorietyComponent)
		{
			NotorietyComponent->RecordCrime(ECrimeType::None, 10.0f, TEXT("DefaultRegion"));
		}

		return false;
	}
}

bool ADAPlayerCharacter::InciteRebellion_Implementation(FName RegionID, FName RebelFactionID)
{
	if (!GetWorld()) return false;

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if player has sufficient political influence
	int32 PlayerInfluence = GetPoliticalInfluence_Implementation(RebelFactionID);
	if (PlayerInfluence < 100) // Very high requirement for inciting rebellion
	{
		UE_LOG(LogTemp, Warning, TEXT("InciteRebellion: Insufficient political influence (%d < 100)"), PlayerInfluence);
		return false;
	}

	// Check if the region exists and get its current controlling faction
	// This would need integration with a world management system
	FName ControllingFactionID = FactionManager->GetTerritoryController(RegionID);
	if (ControllingFactionID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("InciteRebellion: Invalid region or no controlling faction"));
		return false;
	}

	// Can't incite rebellion against your own faction
	if (ControllingFactionID == RebelFactionID)
	{
		UE_LOG(LogTemp, Warning, TEXT("InciteRebellion: Cannot incite rebellion against your own faction"));
		return false;
	}

	// Calculate success chance based on various factors
	float BaseSuccessChance = 0.2f; // Base 20% success for rebellion
	BaseSuccessChance += (PlayerInfluence - 100) * 0.003f; // +0.3% per point above minimum

	// Regional stability modifier (would need world management integration)
	float RegionalStability = 0.5f; // Placeholder - should come from world system
	BaseSuccessChance += (1.0f - RegionalStability) * 0.3f; // More unstable = easier rebellion

	if (FMath::FRand() < BaseSuccessChance)
	{
		// Success - region changes control (full control for now)
		FactionManager->SetTerritoryControl(RegionID, RebelFactionID, 1.0f);

		UE_LOG(LogTemp, Log, TEXT("Rebellion Success: %s now controls region %s!"),
			*RebelFactionID.ToString(), *RegionID.ToString());

		// Major reputation changes
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(RebelFactionID, 50.0f); // Rebels love you
			FactionReputationComponent->ModifyFactionReputation(ControllingFactionID, -75.0f); // Old rulers hate you
		}

		// High notoriety increase for inciting rebellion
		if (NotorietyComponent)
		{
			NotorietyComponent->RecordCrime(ECrimeType::None, 30.0f, TEXT("DefaultRegion"));
		}

		return true;
	}
	else
	{
		// Failure - severe consequences
		UE_LOG(LogTemp, Warning, TEXT("Rebellion Failed: The plot was crushed!"));

		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(RebelFactionID, -40.0f);
			FactionReputationComponent->ModifyFactionReputation(ControllingFactionID, -20.0f); // They still don't like you
		}

		// Very high notoriety penalty
		if (NotorietyComponent)
		{
			NotorietyComponent->RecordCrime(ECrimeType::None, 50.0f, TEXT("DefaultRegion"));
		}

		return false;
	}
}

bool ADAPlayerCharacter::SpreadPropaganda_Implementation(FName FactionID, FName RegionID)
{
	if (!GetWorld()) return false;

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if player has sufficient political influence
	int32 PlayerInfluence = GetPoliticalInfluence_Implementation(FactionID);
	if (PlayerInfluence < 60) // Moderate requirement for propaganda
	{
		UE_LOG(LogTemp, Warning, TEXT("SpreadPropaganda: Insufficient political influence (%d < 60)"), PlayerInfluence);
		return false;
	}

	// Calculate success chance
	float BaseSuccessChance = 0.5f; // Base 50% success for propaganda
	BaseSuccessChance += (PlayerInfluence - 60) * 0.007f; // +0.7% per point above minimum

	// Time of day modifier - propaganda is more effective at certain times
	float TimeOfDay = GetWorld()->GetTimeSeconds() / 3600.0f; // Convert to hours
	if (TimeOfDay >= 18.0f && TimeOfDay <= 22.0f) // Evening hours
	{
		BaseSuccessChance *= 1.3f; // 30% bonus in evening
	}

	if (FMath::FRand() < BaseSuccessChance)
	{
		// Success - improve faction reputation (global player-faction reputation as regional proxy)
		float ReputationGain = FMath::RandRange(15.0f, 30.0f);
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(FactionID, ReputationGain);
		}

		UE_LOG(LogTemp, Log, TEXT("Propaganda Success: %s reputation improved by %.1f in %s"),
			*FactionID.ToString(), ReputationGain, *RegionID.ToString());

		// Small reputation boost for player
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(FactionID, 5.0f);
		}

		return true;
	}
	else
	{
		// Failure - small reputation penalty
		UE_LOG(LogTemp, Warning, TEXT("Propaganda Failed: The message didn't resonate"));

		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(FactionID, -8.0f);
		}

		return false;
	}
}

bool ADAPlayerCharacter::MediatePeace_Implementation(FName Faction1ID, FName Faction2ID)
{
	if (!GetWorld()) return false;

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if factions are actually hostile
	EFactionRelationshipType CurrentRelation = FactionManager->GetFactionRelationship(Faction1ID, Faction2ID);
	if (CurrentRelation != EFactionRelationshipType::Hostile)
	{
		UE_LOG(LogTemp, Warning, TEXT("MediatePeace: Factions are not hostile"));
		return false;
	}

	// Check if player has sufficient diplomatic influence
	int32 PlayerInfluence1 = GetPoliticalInfluence_Implementation(Faction1ID);
	int32 PlayerInfluence2 = GetPoliticalInfluence_Implementation(Faction2ID);
	int32 AvgInfluence = (PlayerInfluence1 + PlayerInfluence2) / 2;

	if (AvgInfluence < 70) // Moderate requirement for mediation
	{
		UE_LOG(LogTemp, Warning, TEXT("MediatePeace: Insufficient diplomatic influence (%d < 70)"), AvgInfluence);
		return false;
	}

	// Calculate success chance
	float BaseSuccessChance = 0.4f; // Base 40% success for mediation
	BaseSuccessChance += (AvgInfluence - 70) * 0.006f; // +0.6% per point above minimum

	// Duration of hostility could affect success; no API for duration yet, keep neutral for now
	// float HostilityDuration = 0.0f;
	// if (HostilityDuration > 30.0f) { BaseSuccessChance *= 0.7f; }

	if (FMath::FRand() < BaseSuccessChance)
	{
		// Success - negotiate peace
		FactionManager->SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Neutral);

		UE_LOG(LogTemp, Log, TEXT("Peace Mediation Success: %s and %s have agreed to peace!"),
			*Faction1ID.ToString(), *Faction2ID.ToString());

		// Reputation bonuses for successful mediation
		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(Faction1ID, 20.0f);
			FactionReputationComponent->ModifyFactionReputation(Faction2ID, 20.0f);
		}

		return true;
	}
	else
	{
		// Failure - small reputation penalty
		UE_LOG(LogTemp, Warning, TEXT("Peace Mediation Failed: Negotiations broke down"));

		if (FactionReputationComponent)
		{
			FactionReputationComponent->ModifyFactionReputation(Faction1ID, -10.0f);
			FactionReputationComponent->ModifyFactionReputation(Faction2ID, -10.0f);
		}

		return false;
	}
}

bool ADAPlayerCharacter::InfluencePoliticalEvent_Implementation(FGuid EventID, bool DesiredOutcome)
{
	if (!GetWorld()) return false;

	// This would need integration with a political event system
	// For now, implement a basic version that affects faction reputation

	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (!FactionManager) return false;

	// Check if player has sufficient political influence
	// Since we don't have a specific faction for the event, use a general influence check
	int32 PlayerInfluence = 0;
	if (FactionReputationComponent)
	{
		// Use average reputation across all factions as influence
		if (FactionManager)
		{
			TArray<FName> FactionIDs = FactionManager->GetAllFactions();
			if (FactionIDs.Num() > 0)
			{
				float TotalRep = 0.0f;
				for (const FName& FactionID : FactionIDs)
				{
					TotalRep += FactionReputationComponent->GetFactionReputation(FactionID);
				}
				PlayerInfluence = FMath::FloorToInt((TotalRep / FactionIDs.Num()) * 0.5f);
			}
		}
	}

	if (PlayerInfluence < 80) // High requirement for event influence
	{
		UE_LOG(LogTemp, Warning, TEXT("InfluencePoliticalEvent: Insufficient political influence (%d < 80)"), PlayerInfluence);
		return false;
	}

	// Calculate success chance
	float BaseSuccessChance = 0.35f; // Base 35% success for event influence
	BaseSuccessChance += (PlayerInfluence - 80) * 0.004f; // +0.4% per point above minimum

	if (FMath::FRand() < BaseSuccessChance)
	{
		// Success - influence the event outcome
		UE_LOG(LogTemp, Log, TEXT("Event Influence Success: Political event %s outcome influenced to %s"),
			*EventID.ToString(), DesiredOutcome ? TEXT("positive") : TEXT("negative"));

		// Generic reputation effects (would be more specific in a full implementation)
		if (FactionReputationComponent)
		{
			float ReputationChange = DesiredOutcome ? 15.0f : -15.0f;
			// Apply to a random faction (in full implementation, would be event-specific)
			TArray<FName> FactionIDs = FactionManager->GetAllFactions();
			if (FactionIDs.Num() > 0)
			{
				FName RandomFaction = FactionIDs[FMath::RandRange(0, FactionIDs.Num() - 1)];
				FactionReputationComponent->ModifyFactionReputation(RandomFaction, ReputationChange);
			}
		}

		return true;
	}
	else
	{
		// Failure - reputation penalty
		UE_LOG(LogTemp, Warning, TEXT("Event Influence Failed: Your efforts had no effect"));

		if (FactionReputationComponent)
		{
			// Small penalty to all factions
			TArray<FName> FactionIDs = FactionManager->GetAllFactions();
			for (FName FactionID : FactionIDs)
			{
				FactionReputationComponent->ModifyFactionReputation(FactionID, -5.0f);
			}
		}

		return false;
	}
}

// --- Missing Debug Command Implementations ---
void ADAPlayerCharacter::DeclareWarDebug(FString Faction1String, FString Faction2String)
{
	UE_LOG(LogTemp, Log, TEXT("DeclareWarDebug: %s declares war on %s (Debug command - not yet implemented)"), *Faction1String, *Faction2String);

	if (!GetWorld()) return;
	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (FactionManager)
	{
		FactionManager->SetFactionRelationship(FName(*Faction1String), FName(*Faction2String), EFactionRelationshipType::Hostile);

		// Apply reputation penalties for declaring war
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction1String), -25.0f);
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction2String), -25.0f);

		UE_LOG(LogTemp, Warning, TEXT("War declared between %s and %s! Both factions lose reputation with player."), *Faction1String, *Faction2String);
	}
}

void ADAPlayerCharacter::FormAllianceDebug(FString Faction1String, FString Faction2String)
{
	UE_LOG(LogTemp, Log, TEXT("FormAllianceDebug: %s forms alliance with %s (Debug command - not yet implemented)"), *Faction1String, *Faction2String);

	if (!GetWorld()) return;
	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (FactionManager)
	{
		FactionManager->SetFactionRelationship(FName(*Faction1String), FName(*Faction2String), EFactionRelationshipType::Allied);

		// Apply reputation bonuses for forming alliances
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction1String), 15.0f);
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction2String), 15.0f);

		UE_LOG(LogTemp, Log, TEXT("Alliance formed between %s and %s! Both factions gain reputation with player."), *Faction1String, *Faction2String);
	}
}

void ADAPlayerCharacter::NegotiatePeaceDebug(FString Faction1String, FString Faction2String)
{
	UE_LOG(LogTemp, Log, TEXT("NegotiatePeaceDebug: Negotiating peace between %s and %s (Debug command - not yet implemented)"), *Faction1String, *Faction2String);

	if (!GetWorld()) return;
	UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>();
	if (FactionManager)
	{
		FactionManager->SetFactionRelationship(FName(*Faction1String), FName(*Faction2String), EFactionRelationshipType::Neutral);

		// Apply moderate reputation bonuses for peace negotiations
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction1String), 10.0f);
		FactionReputationComponent->ModifyFactionReputation(FName(*Faction2String), 10.0f);

		UE_LOG(LogTemp, Log, TEXT("Peace negotiated between %s and %s! Both factions gain reputation with player."), *Faction1String, *Faction2String);
	}
}

// --- Survival & Stats Debug Commands ---
void ADAPlayerCharacter::PrintPlayerStatsDebug()
{
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("=== Player GAS Attributes ==="));
		UE_LOG(LogTemp, Log, TEXT("Health: %.1f/%.1f"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHealthAttribute()),
			AbilitySystemComponent->GetNumericAttributeBase(UDASurvivalAttributeSet::GetHealthAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Hunger: %.1f/100.0"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHungerAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Thirst: %.1f/100.0"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetThirstAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Warmth: %.1f/100.0"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetWarmthAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Sleep: %.1f/100.0"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetSleepAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Strength: %.1f"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetStrengthAttribute()));
		UE_LOG(LogTemp, Log, TEXT("Dexterity: %.1f"),
			AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetDexterityAttribute()));
	}
}

void ADAPlayerCharacter::ModifyStatDebug(FString StatName, float Delta)
{
	if (AbilitySystemComponent)
	{
		FName StatFName = FName(*StatName);
		if (StatFName == "Health")
		{
			float NewValue = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHealthAttribute()) + Delta;
			AbilitySystemComponent->SetNumericAttributeBase(UDASurvivalAttributeSet::GetHealthAttribute(), FMath::Clamp(NewValue, 0.f, 100.f));
		}
		else if (StatFName == "Hunger")
		{
			float NewValue = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHungerAttribute()) + Delta;
			AbilitySystemComponent->SetNumericAttributeBase(UDASurvivalAttributeSet::GetHungerAttribute(), FMath::Clamp(NewValue, 0.f, 100.f));
		}
		else if (StatFName == "Thirst")
		{
			float NewValue = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetThirstAttribute()) + Delta;
			AbilitySystemComponent->SetNumericAttributeBase(UDASurvivalAttributeSet::GetThirstAttribute(), FMath::Clamp(NewValue, 0.f, 100.f));
		}
		// Add more stats as needed
		UE_LOG(LogTemp, Log, TEXT("Modified %s by %.2f"), *StatName, Delta);
	}
}

void ADAPlayerCharacter::TestSurvivalMechanicsDebug()
{
	if (!GetWorld()) return;
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance) return;

	UAdvancedSurvivalSubsystem* SurvivalSubsystem = GameInstance->GetSubsystem<UAdvancedSurvivalSubsystem>();
	if (SurvivalSubsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("=== Survival System Test ==="));
		UE_LOG(LogTemp, Log, TEXT("Survival subsystem found and active"));

		// Test stat updates (this will trigger HUD updates)
		if (AbilitySystemComponent)
		{
			float currentHunger = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetHungerAttribute());
			float currentThirst = AbilitySystemComponent->GetNumericAttribute(UDASurvivalAttributeSet::GetThirstAttribute());
			UE_LOG(LogTemp, Log, TEXT("Current Hunger: %.1f, Thirst: %.1f"), currentHunger, currentThirst);
		}
	}
}

void ADAPlayerCharacter::TestInteractionDebug()
{
	if (InteractionComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("=== Interaction System Test ==="));
		UE_LOG(LogTemp, Log, TEXT("Interaction component is active and ready"));

		// Test interaction by calling the interact function
		InteractionComponent->Interact();
		UE_LOG(LogTemp, Log, TEXT("Interaction attempted"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No InteractionComponent found on player"));
	}
}

// --- New Test Commands for Validation ---
void ADAPlayerCharacter::TestAINeedsDebug()
{
	UE_LOG(LogTemp, Log, TEXT("=== AI Needs System Test ==="));

	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			TArray<UAINeedsPlanningComponent*> Components = Manager->GetRegisteredComponents();
			UE_LOG(LogTemp, Log, TEXT("Found %d AI components managed by subsystem"), Components.Num());

			for (UAINeedsPlanningComponent* Comp : Components)
			{
				if (Comp && Comp->GetOwner())
				{
					FString OwnerName = Comp->GetOwner()->GetName();
					float Hunger = Comp->GetNeedValue(EAINeed::Hunger);
					float Social = Comp->GetNeedValue(EAINeed::Social);
					UE_LOG(LogTemp, Log, TEXT("NPC %s - Hunger: %.1f, Social: %.1f"), *OwnerName, Hunger, Social);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No NPC Manager Subsystem found"));
		}
	}
}

void ADAPlayerCharacter::TestPoliticalActionsDebug()
{
	UE_LOG(LogTemp, Log, TEXT("=== Political System Test ==="));

	// Test influence calculation
	int32 Influence = GetPoliticalInfluence_Implementation(FName("TestFaction"));
	UE_LOG(LogTemp, Log, TEXT("Player political influence: %d"), Influence);

	// Test treaty negotiation (would need valid factions)
	UE_LOG(LogTemp, Log, TEXT("Political actions are implemented and ready for testing"));
}

void ADAPlayerCharacter::TestPerformanceDebug()
{
	UE_LOG(LogTemp, Log, TEXT("=== Performance Test ==="));

	if (UWorld* World = GetWorld())
	{
		if (UDA_NPCManagerSubsystem* Manager = World->GetSubsystem<UDA_NPCManagerSubsystem>())
		{
			Manager->ForceUpdateWorldCache();
			UE_LOG(LogTemp, Log, TEXT("World cache updated for performance optimization"));

			TArray<AActor*> Hostiles = Manager->GetNearbyHostiles(GetActorLocation(), 1000.0f);
			TArray<AActor*> NPCs = Manager->GetNearbyFriendlyNPCs(GetActorLocation(), 1000.0f);

			UE_LOG(LogTemp, Log, TEXT("Cached %d hostiles and %d friendly NPCs within 1000 units"),
				Hostiles.Num(), NPCs.Num());
		}
	}
}