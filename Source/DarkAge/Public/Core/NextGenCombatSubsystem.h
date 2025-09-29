#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NextGenCombatSubsystem.generated.h"

struct FDamageResult
{
    bool bDidHit;
    float FinalDamage;
    bool bIsCritical;
    bool bIsFatal;
};

/**
 * Manages the next-generation combat system.
 */
UCLASS()
class DARKAGE_API UNextGenCombatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UNextGenCombatSubsystem();

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    void Tick(float DeltaTime);

    FDamageResult ProcessDamage(AActor* Attacker, AActor* Victim, float BaseDamage);
};