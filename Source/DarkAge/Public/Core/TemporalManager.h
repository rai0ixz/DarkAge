// Copyright (C) 2024, Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/TimeSystem.h"
#include "TemporalManager.generated.h"

/**
 * Manages temporal manipulation abilities, such as time dilation.
 */
UCLASS()
class DARKAGE_API UTemporalManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Delegate for when a time loop resets
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeLoopReset);

	// Sets the global time dilation.
	UFUNCTION(BlueprintCallable, Category = "Temporal")
	void SetGlobalTimeDilation(float Dilation);

	// Returns the current global time dilation.
	UFUNCTION(BlueprintPure, Category = "Temporal")
	float GetGlobalTimeDilation() const;

	// Starts a global time loop between the specified start and end times.
	UFUNCTION(BlueprintCallable, Category = "Temporal")
	void StartGlobalTimeLoop(const FGameDateTime& StartTime, const FGameDateTime& EndTime);

	// Stops the global time loop.
	UFUNCTION(BlueprintCallable, Category = "Temporal")
	void StopGlobalTimeLoop();

	// Returns true if a time loop is currently active.
	UFUNCTION(BlueprintPure, Category = "Temporal")
	bool IsTimeLoopActive() const;

	UPROPERTY(BlueprintAssignable, Category = "Temporal")
	FOnTimeLoopReset OnTimeLoopReset;

private:
	// The current time dilation value.
	float GlobalTimeDilation = 1.0f;

	// True if a time loop is active.
	bool bIsTimeLoopActive = false;

	// The time the loop starts.
	FGameDateTime TimeLoopStartPoint;

	// The time the loop ends and resets.
	FGameDateTime TimeLoopEndPoint;

	// Reference to the time system
	class UTimeSystem* TimeSystem;

	// Handler for when the time is updated
	UFUNCTION()
	void HandleTimeUpdated(const FGameDateTime& NewTime);
};