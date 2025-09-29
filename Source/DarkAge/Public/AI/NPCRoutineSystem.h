// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NPCRoutineSystem.generated.h"

/**
 * Activity type enum
 */
UENUM(BlueprintType)
enum class EActivityType : uint8
{
	Work        UMETA(DisplayName = "Work"),
	Sleep       UMETA(DisplayName = "Sleep"),
	Eat         UMETA(DisplayName = "Eat"),
	Socialize   UMETA(DisplayName = "Socialize"),
	Recreation  UMETA(DisplayName = "Recreation"),
	Shopping    UMETA(DisplayName = "Shopping"),
	Worship     UMETA(DisplayName = "Worship"),
	Travel      UMETA(DisplayName = "Travel"),
	Idle        UMETA(DisplayName = "Idle")
};

/**
 * Scheduled activity structure
 */
USTRUCT(BlueprintType)
struct FScheduledActivity
{
	GENERATED_BODY()
	
	// Activity type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	EActivityType ActivityType;
	
	// Start time (hour of day, 0-23)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	int32 StartHour;
	
	// Duration in hours
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	int32 Duration;
	
	// Location for activity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	FName LocationID;
	
	// Priority (higher values take precedence)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	int32 Priority;
	
	// Days of week when this activity occurs (0-6, where 0 is Sunday)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	TArray<int32> DaysOfWeek;
	
	// Is this a mandatory activity?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	bool bIsMandatory;
	
	FScheduledActivity()
		: ActivityType(EActivityType::Idle)
		, StartHour(8)
		, Duration(1)
		, LocationID(NAME_None)
		, Priority(1)
		, bIsMandatory(false)
	{
		// Default to all days
		DaysOfWeek = { 0, 1, 2, 3, 4, 5, 6 };
	}
};

/**
 * Daily schedule structure
 */
USTRUCT(BlueprintType)
struct FDailySchedule
{
	GENERATED_BODY()
	
	// Scheduled activities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	TArray<FScheduledActivity> Activities;
	
	// Occupation of the NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	FName Occupation;
	
	// Social class of the NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	FName SocialClass;
	
	// Home location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	FName HomeLocation;
	
	// Work location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Routine")
	FName WorkLocation;
	
	FDailySchedule()
		: Occupation(NAME_None)
		, SocialClass("Commoner")
		, HomeLocation(NAME_None)
		, WorkLocation(NAME_None)
	{
	}
};

/**
 * NPC Routine System for Dark Age
 * 
 * Manages daily schedules and activities for NPCs.
 * Integrates with time system, weather, and other environmental factors.
 */
UCLASS()
class DARKAGE_API UNPCRoutineSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UNPCRoutineSystem();
	
	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Deinitialize the subsystem
	virtual void Deinitialize() override;
	
	// Update NPC routines
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void UpdateNPCRoutines(float DeltaTime);
	
	// Register an NPC with the system
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void RegisterNPC(AActor* NPCCharacter, FName Occupation, FName SocialClass);
	
	// Unregister an NPC from the system
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void UnregisterNPC(AActor* NPCCharacter);
	
	// Get current activity for an NPC
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	FScheduledActivity GetCurrentActivity(AActor* NPCCharacter) const;
	
	// Get next activity for an NPC
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	FScheduledActivity GetNextActivity(AActor* NPCCharacter) const;
	
	// Get daily schedule for an NPC
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	FDailySchedule GetDailySchedule(AActor* NPCCharacter) const;
	
	// Set home location for an NPC
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void SetHomeLocation(AActor* NPCCharacter, FName LocationID);
	
	// Set work location for an NPC
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void SetWorkLocation(AActor* NPCCharacter, FName LocationID);
	
	// Add a custom activity to an NPC's schedule
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void AddCustomActivity(AActor* NPCCharacter, const FScheduledActivity& Activity);
	
	// Remove a custom activity from an NPC's schedule
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void RemoveCustomActivity(AActor* NPCCharacter, int32 ActivityIndex);
	
	// Interrupt current activity and assign a new one
	UFUNCTION(BlueprintCallable, Category = "NPC Routine")
	void InterruptWithActivity(AActor* NPCCharacter, const FScheduledActivity& Activity);
	
	// Delegate for activity changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActivityChanged, AActor*, NPCCharacter, EActivityType, OldActivity, EActivityType, NewActivity);
	UPROPERTY(BlueprintAssignable, Category = "NPC Routine")
	FOnActivityChanged OnActivityChanged;
	
private:
	// NPC schedules
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FDailySchedule> NPCSchedules;
	
	// Current activities
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FScheduledActivity> CurrentActivities;
	
	// Temporary activity interruptions
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FScheduledActivity> ActivityInterruptions;
	
	// Time since last update
	float TimeSinceLastUpdate;
	
	// Generate a default schedule based on occupation and social class
	FDailySchedule GenerateDefaultSchedule(FName Occupation, FName SocialClass);
	
	// Get appropriate activity for current time
	FScheduledActivity GetActivityForTime(const FDailySchedule& Schedule, int32 CurrentHour, int32 CurrentDay) const;
	
	// Check if an activity should be active at the given time
	bool IsActivityActiveAtTime(const FScheduledActivity& Activity, int32 CurrentHour, int32 CurrentDay) const;
	
	// Modify schedules based on weather and other factors
	void ModifySchedulesForEnvironment();
};