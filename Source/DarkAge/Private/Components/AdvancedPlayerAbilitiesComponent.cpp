#include "Components/AdvancedPlayerAbilitiesComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StatlineComponent.h"
#include "Components/PlayerSkillsComponent.h"
#include "Components/InventoryComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UAdvancedPlayerAbilitiesComponent::UAdvancedPlayerAbilitiesComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize ability cooldowns
    AbilityCooldowns.Add(TEXT("Sprint"), 0.0f);
    AbilityCooldowns.Add(TEXT("Dodge"), 0.0f);
    AbilityCooldowns.Add(TEXT("PowerAttack"), 0.0f);
    AbilityCooldowns.Add(TEXT("Stealth"), 0.0f);
    AbilityCooldowns.Add(TEXT("Intimidate"), 0.0f);
    AbilityCooldowns.Add(TEXT("Inspire"), 0.0f);
    AbilityCooldowns.Add(TEXT("Analyze"), 0.0f);
    AbilityCooldowns.Add(TEXT("Craft"), 0.0f);
    
    // Initialize ability states
    bIsSprintActive = false;
    bIsStealthActive = false;
    bIsInspireActive = false;
    
    // Initialize ability parameters
    SprintSpeedMultiplier = 1.8f;
    SprintStaminaCost = 15.0f;
    DodgeDistance = 300.0f;
    DodgeStaminaCost = 20.0f;
    StealthDuration = 30.0f;
    StealthMovementPenalty = 0.5f;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedPlayerAbilitiesComponent initialized"));
}

void UAdvancedPlayerAbilitiesComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Cache component references
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        StatlineComponent = OwnerCharacter->FindComponentByClass<UStatlineComponent>();
        SkillsComponent = OwnerCharacter->FindComponentByClass<UPlayerSkillsComponent>();
        InventoryComponent = OwnerCharacter->FindComponentByClass<UInventoryComponent>();
        
        if (OwnerCharacter->GetCharacterMovement())
        {
            DefaultWalkSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update ability cooldowns
    UpdateAbilityCooldowns(DeltaTime);
    
    // Update active abilities
    UpdateActiveAbilities(DeltaTime);
    
    // Handle stamina regeneration
    HandleStaminaRegeneration(DeltaTime);
}

void UAdvancedPlayerAbilitiesComponent::UpdateAbilityCooldowns(float DeltaTime)
{
    for (auto& CooldownPair : AbilityCooldowns)
    {
        if (CooldownPair.Value > 0.0f)
        {
            CooldownPair.Value = FMath::Max(0.0f, CooldownPair.Value - DeltaTime);
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::UpdateActiveAbilities(float DeltaTime)
{
    // Update sprint
    if (bIsSprintActive)
    {
        if (StatlineComponent && StatlineComponent->GetCurrentStamina() > 0.0f)
        {
            StatlineComponent->ModifyStamina(-SprintStaminaCost * DeltaTime);
        }
        else
        {
            DeactivateSprint();
        }
    }
    
    // Update stealth
    if (bIsStealthActive)
    {
        StealthRemainingTime -= DeltaTime;
        if (StealthRemainingTime <= 0.0f)
        {
            DeactivateStealth();
        }
    }
    
    // Update inspire buff
    if (bIsInspireActive)
    {
        InspireRemainingTime -= DeltaTime;
        if (InspireRemainingTime <= 0.0f)
        {
            DeactivateInspire();
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::HandleStaminaRegeneration(float DeltaTime)
{
    if (StatlineComponent && !bIsSprintActive)
    {
        float RegenRate = 10.0f; // Base regeneration rate
        
        // Modify regen rate based on conditions
        if (bIsStealthActive)
        {
            RegenRate *= 0.5f; // Slower regen while stealthed
        }
        
        StatlineComponent->ModifyStamina(RegenRate * DeltaTime);
    }
}

// Combat Abilities
bool UAdvancedPlayerAbilitiesComponent::ActivatePowerAttack()
{
    if (!CanUseAbility(TEXT("PowerAttack"), 25.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player activated Power Attack!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-25.0f);
    }
    
    // Set cooldown
    AbilityCooldowns[TEXT("PowerAttack")] = 8.0f;
    
    // Apply power attack effects (would be implemented with animation system)
    ApplyPowerAttackEffects();
    
    return true;
}

bool UAdvancedPlayerAbilitiesComponent::ActivateDodge()
{
    if (!CanUseAbility(TEXT("Dodge"), DodgeStaminaCost))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player performed dodge roll!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-DodgeStaminaCost);
    }
    
    // Set cooldown
    AbilityCooldowns[TEXT("Dodge")] = 3.0f;
    
    // Apply dodge movement
    ApplyDodgeMovement();
    
    return true;
}

// Movement Abilities
bool UAdvancedPlayerAbilitiesComponent::ActivateSprint()
{
    if (bIsSprintActive || !CanUseAbility(TEXT("Sprint"), 5.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player started sprinting!"));
    
    bIsSprintActive = true;
    
    // Increase movement speed
    if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
    {
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
    }
    
    return true;
}

void UAdvancedPlayerAbilitiesComponent::DeactivateSprint()
{
    if (!bIsSprintActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player stopped sprinting"));
    
    bIsSprintActive = false;
    
    // Restore normal movement speed
    if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
    {
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
    }
}

// Stealth Abilities
bool UAdvancedPlayerAbilitiesComponent::ActivateStealth()
{
    if (bIsStealthActive || !CanUseAbility(TEXT("Stealth"), 30.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player entered stealth mode!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-30.0f);
    }
    
    bIsStealthActive = true;
    StealthRemainingTime = StealthDuration;
    
    // Set cooldown
    AbilityCooldowns[TEXT("Stealth")] = 45.0f;
    
    // Apply stealth effects
    ApplyStealthEffects();
    
    return true;
}

void UAdvancedPlayerAbilitiesComponent::DeactivateStealth()
{
    if (!bIsStealthActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player exited stealth mode"));
    
    bIsStealthActive = false;
    StealthRemainingTime = 0.0f;
    
    // Remove stealth effects
    RemoveStealthEffects();
}

// Social Abilities
bool UAdvancedPlayerAbilitiesComponent::ActivateIntimidate()
{
    if (!CanUseAbility(TEXT("Intimidate"), 20.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player used intimidation!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-20.0f);
    }
    
    // Set cooldown
    AbilityCooldowns[TEXT("Intimidate")] = 30.0f;
    
    // Apply intimidation effects to nearby NPCs
    ApplyIntimidationEffects();
    
    return true;
}

bool UAdvancedPlayerAbilitiesComponent::ActivateInspire()
{
    if (bIsInspireActive || !CanUseAbility(TEXT("Inspire"), 25.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player inspired nearby allies!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-25.0f);
    }
    
    bIsInspireActive = true;
    InspireRemainingTime = 60.0f; // 1 minute buff
    
    // Set cooldown
    AbilityCooldowns[TEXT("Inspire")] = 120.0f;
    
    // Apply inspiration effects
    ApplyInspirationEffects();
    
    return true;
}

void UAdvancedPlayerAbilitiesComponent::DeactivateInspire()
{
    if (!bIsInspireActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Inspiration effect ended"));
    
    bIsInspireActive = false;
    InspireRemainingTime = 0.0f;
    
    // Remove inspiration effects
    RemoveInspirationEffects();
}

// Utility Abilities
bool UAdvancedPlayerAbilitiesComponent::ActivateAnalyze()
{
    if (!CanUseAbility(TEXT("Analyze"), 15.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player analyzed the environment!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-15.0f);
    }
    
    // Set cooldown
    AbilityCooldowns[TEXT("Analyze")] = 20.0f;
    
    // Reveal information about nearby objects/NPCs
    PerformEnvironmentalAnalysis();
    
    return true;
}

bool UAdvancedPlayerAbilitiesComponent::ActivateAdvancedCrafting()
{
    if (!CanUseAbility(TEXT("Craft"), 40.0f))
    {
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player activated advanced crafting mode!"));
    
    // Consume stamina
    if (StatlineComponent)
    {
        StatlineComponent->ModifyStamina(-40.0f);
    }
    
    // Set cooldown
    AbilityCooldowns[TEXT("Craft")] = 60.0f;
    
    // Enable advanced crafting options
    EnableAdvancedCraftingMode();
    
    return true;
}

// Helper Functions
bool UAdvancedPlayerAbilitiesComponent::CanUseAbility(const FString& AbilityName, float StaminaCost) const
{
    // Check cooldown
    if (const float* Cooldown = AbilityCooldowns.Find(AbilityName))
    {
        if (*Cooldown > 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Ability %s is on cooldown: %f seconds remaining"), *AbilityName, *Cooldown);
            return false;
        }
    }
    
    // Check stamina
    if (StatlineComponent && StatlineComponent->GetCurrentStamina() < StaminaCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for ability %s. Required: %f, Current: %f"), 
            *AbilityName, StaminaCost, StatlineComponent->GetCurrentStamina());
        return false;
    }
    
    return true;
}

float UAdvancedPlayerAbilitiesComponent::GetAbilityCooldown(const FString& AbilityName) const
{
    if (const float* Cooldown = AbilityCooldowns.Find(AbilityName))
    {
        return *Cooldown;
    }
    return 0.0f;
}

bool UAdvancedPlayerAbilitiesComponent::IsAbilityActive(const FString& AbilityName) const
{
    if (AbilityName == TEXT("Sprint"))
    {
        return bIsSprintActive;
    }
    else if (AbilityName == TEXT("Stealth"))
    {
        return bIsStealthActive;
    }
    else if (AbilityName == TEXT("Inspire"))
    {
        return bIsInspireActive;
    }
    
    return false;
}

// Effect Implementation Functions
void UAdvancedPlayerAbilitiesComponent::ApplyPowerAttackEffects()
{
    // Increase next attack damage by 150%
    UE_LOG(LogTemp, Log, TEXT("Power Attack: Next attack damage increased by 150%%"));
    
    // Could integrate with combat system to apply damage multiplier
    // This would typically set a flag that the combat system checks
}

void UAdvancedPlayerAbilitiesComponent::ApplyDodgeMovement()
{
    if (OwnerCharacter)
    {
        // Get forward vector and apply impulse
        FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
        FVector DodgeVector = ForwardVector * DodgeDistance;
        
        UE_LOG(LogTemp, Log, TEXT("Dodge: Moving player %f units forward"), DodgeDistance);
        
        // In a real implementation, this would use character movement or physics
        // OwnerCharacter->LaunchCharacter(DodgeVector, true, false);
    }
}

void UAdvancedPlayerAbilitiesComponent::ApplyStealthEffects()
{
    UE_LOG(LogTemp, Log, TEXT("Stealth Effects Applied: Reduced visibility, slower movement"));
    
    // Reduce movement speed
    if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
    {
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * StealthMovementPenalty;
    }
    
    // In a real implementation, this would:
    // - Reduce player visibility to NPCs
    // - Modify detection ranges
    // - Change player appearance/transparency
}

void UAdvancedPlayerAbilitiesComponent::RemoveStealthEffects()
{
    UE_LOG(LogTemp, Log, TEXT("Stealth Effects Removed: Normal visibility and movement restored"));
    
    // Restore normal movement speed
    if (OwnerCharacter && OwnerCharacter->GetCharacterMovement() && !bIsSprintActive)
    {
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
    }
}

void UAdvancedPlayerAbilitiesComponent::ApplyIntimidationEffects()
{
    UE_LOG(LogTemp, Log, TEXT("Intimidation: Affecting nearby NPCs"));
    
    // Find nearby NPCs and apply fear effect
    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> NearbyActors;
        UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), NearbyActors);
        
        FVector PlayerLocation = OwnerCharacter->GetActorLocation();
        
        for (AActor* Actor : NearbyActors)
        {
            if (Actor && Actor != OwnerCharacter)
            {
                float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
                if (Distance < 500.0f) // 5 meter range
                {
                    if (Actor->GetName().Contains(TEXT("NPC")))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Intimidated NPC: %s"), *Actor->GetName());
                        // Apply fear effect to NPC
                    }
                }
            }
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::ApplyInspirationEffects()
{
    UE_LOG(LogTemp, Log, TEXT("Inspiration: Boosting nearby allies"));
    
    // Find nearby friendly NPCs and apply inspiration buff
    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> NearbyActors;
        UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), NearbyActors);
        
        FVector PlayerLocation = OwnerCharacter->GetActorLocation();
        
        for (AActor* Actor : NearbyActors)
        {
            if (Actor && Actor != OwnerCharacter)
            {
                float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
                if (Distance < 800.0f) // 8 meter range
                {
                    if (Actor->GetName().Contains(TEXT("NPC")) || Actor->GetName().Contains(TEXT("Ally")))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Inspired ally: %s"), *Actor->GetName());
                        // Apply inspiration buff to ally
                    }
                }
            }
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::RemoveInspirationEffects()
{
    UE_LOG(LogTemp, Log, TEXT("Inspiration effects ended"));
    // Remove inspiration buffs from all affected allies
}

void UAdvancedPlayerAbilitiesComponent::PerformEnvironmentalAnalysis()
{
    UE_LOG(LogTemp, Log, TEXT("Environmental Analysis: Revealing hidden information"));
    
    // Analyze nearby objects and NPCs
    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> NearbyActors;
        UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), NearbyActors);
        
        FVector PlayerLocation = OwnerCharacter->GetActorLocation();
        
        for (AActor* Actor : NearbyActors)
        {
            if (Actor && Actor != OwnerCharacter)
            {
                float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
                if (Distance < 1000.0f) // 10 meter analysis range
                {
                    UE_LOG(LogTemp, Log, TEXT("Analyzed: %s at distance %f"), *Actor->GetName(), Distance);
                    
                    // Reveal information about the object/NPC
                    if (Actor->GetName().Contains(TEXT("NPC")))
                    {
                        UE_LOG(LogTemp, Log, TEXT("NPC Analysis: Health, mood, faction revealed"));
                    }
                    else if (Actor->GetName().Contains(TEXT("Chest")) || Actor->GetName().Contains(TEXT("Container")))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Container Analysis: Contents partially revealed"));
                    }
                }
            }
        }
    }
}

void UAdvancedPlayerAbilitiesComponent::EnableAdvancedCraftingMode()
{
    UE_LOG(LogTemp, Log, TEXT("Advanced Crafting Mode: Unlocking rare recipes and quality bonuses"));
    
    // Enable advanced crafting options for a limited time
    // This would typically integrate with the crafting system to:
    // - Unlock rare recipes
    // - Provide quality bonuses
    // - Reduce material requirements
    // - Increase success chances
}

// Getters for UI and other systems
bool UAdvancedPlayerAbilitiesComponent::IsSprintActive() const
{
    return bIsSprintActive;
}

bool UAdvancedPlayerAbilitiesComponent::IsStealthActive() const
{
    return bIsStealthActive;
}

float UAdvancedPlayerAbilitiesComponent::GetStealthRemainingTime() const
{
    return StealthRemainingTime;
}

bool UAdvancedPlayerAbilitiesComponent::IsInspireActive() const
{
    return bIsInspireActive;
}

float UAdvancedPlayerAbilitiesComponent::GetInspireRemainingTime() const
{
    return InspireRemainingTime;
}