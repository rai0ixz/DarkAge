// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/NPCRoutineSystem.h"

UNPCRoutineSystem::UNPCRoutineSystem()
	: TimeSinceLastUpdate(0.0f)
{
}

void UNPCRoutineSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize any resources needed
}

void UNPCRoutineSystem::Deinitialize()
{
	Super::Deinitialize();
	
	// Clean up resources
	NPCSchedules.Empty();
	CurrentActivities.Empty();
	ActivityInterruptions.Empty();
}

void UNPCRoutineSystem::UpdateNPCRoutines(float DeltaTime)
{
	// Accumulate time since last update
	TimeSinceLastUpdate += DeltaTime;
	
	// Update routines periodically
	const float RoutineUpdateInterval = 60.0f; // 1 minute real time
	
	if (TimeSinceLastUpdate >= RoutineUpdateInterval)
	{
		// Get current game time
		// This would integrate with the TimeSystem
		int32 CurrentHour = 8; // Placeholder - would get from TimeSystem
		int32 CurrentDay = 1; // Placeholder - would get from TimeSystem
		
		// Modify schedules based on environmental factors
		ModifySchedulesForEnvironment();
		
		// Update activities for all NPCs
		TArray<TWeakObjectPtr<AActor>> NPCsToUpdate;
		NPCSchedules.GetKeys(NPCsToUpdate);
		
		for (const TWeakObjectPtr<AActor>& NPC : NPCsToUpdate)
		{
			if (!NPC.IsValid())
			{
				// Remove invalid NPCs
				NPCSchedules.Remove(NPC);
				CurrentActivities.Remove(NPC);
				ActivityInterruptions.Remove(NPC);
				continue;
			}
			
			// Check if NPC has an activity interruption
			if (ActivityInterruptions.Contains(NPC))
			{
				// Keep the interruption as current activity
				if (!CurrentActivities.Contains(NPC) || 
					CurrentActivities[NPC].ActivityType != ActivityInterruptions[NPC].ActivityType)
				{
					EActivityType OldActivity = CurrentActivities.Contains(NPC) ? 
						CurrentActivities[NPC].ActivityType : EActivityType::Idle;
					
					// Update current activity
					CurrentActivities.Add(NPC, ActivityInterruptions[NPC]);
					
					// Broadcast activity change
					OnActivityChanged.Broadcast(NPC.Get(), OldActivity, ActivityInterruptions[NPC].ActivityType);
				}
				
				continue;
			}
			
			// Get appropriate activity for current time
			const FDailySchedule& Schedule = NPCSchedules[NPC];
			FScheduledActivity NewActivity = GetActivityForTime(Schedule, CurrentHour, CurrentDay);
			
			// Check if activity has changed
			if (!CurrentActivities.Contains(NPC) || 
				CurrentActivities[NPC].ActivityType != NewActivity.ActivityType ||
				CurrentActivities[NPC].LocationID != NewActivity.LocationID)
			{
				EActivityType OldActivity = CurrentActivities.Contains(NPC) ? 
					CurrentActivities[NPC].ActivityType : EActivityType::Idle;
				
				// Update current activity
				CurrentActivities.Add(NPC, NewActivity);
				
				// Broadcast activity change
				OnActivityChanged.Broadcast(NPC.Get(), OldActivity, NewActivity.ActivityType);
			}
		}
		
		// Reset timer
		TimeSinceLastUpdate = 0.0f;
	}
}

void UNPCRoutineSystem::RegisterNPC(AActor* NPCCharacter, FName Occupation, FName SocialClass)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	// Generate default schedule based on occupation and social class
	FDailySchedule Schedule = GenerateDefaultSchedule(Occupation, SocialClass);
	
	// Store schedule
	NPCSchedules.Add(NPCPtr, Schedule);
	
	// Initialize with idle activity
	FScheduledActivity IdleActivity;
	IdleActivity.ActivityType = EActivityType::Idle;
	CurrentActivities.Add(NPCPtr, IdleActivity);
}

void UNPCRoutineSystem::UnregisterNPC(AActor* NPCCharacter)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	// Remove NPC from all maps
	NPCSchedules.Remove(NPCPtr);
	CurrentActivities.Remove(NPCPtr);
	ActivityInterruptions.Remove(NPCPtr);
}

FScheduledActivity UNPCRoutineSystem::GetCurrentActivity(AActor* NPCCharacter) const
{
	if (!NPCCharacter)
	{
		return FScheduledActivity();
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (CurrentActivities.Contains(NPCPtr))
	{
		return CurrentActivities[NPCPtr];
	}
	
	return FScheduledActivity();
}

FScheduledActivity UNPCRoutineSystem::GetNextActivity(AActor* NPCCharacter) const
{
	if (!NPCCharacter)
	{
		return FScheduledActivity();
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (!NPCSchedules.Contains(NPCPtr))
	{
		return FScheduledActivity();
	}
	
	// Get current game time
	// This would integrate with the TimeSystem
	int32 CurrentHour = 8; // Placeholder - would get from TimeSystem
	int32 CurrentDay = 1; // Placeholder - would get from TimeSystem
	
	const FDailySchedule& Schedule = NPCSchedules[NPCPtr];
	
	// Find current activity
	FScheduledActivity CurrentActivity = GetActivityForTime(Schedule, CurrentHour, CurrentDay);
	
	// Find next activity
	for (int32 Hour = CurrentHour + 1; Hour < CurrentHour + 24; ++Hour)
	{
		int32 HourOfDay = Hour % 24;
		int32 DayOffset = Hour / 24;
		int32 NextDay = (CurrentDay + DayOffset) % 7;
		
		for (const FScheduledActivity& Activity : Schedule.Activities)
		{
			if (Activity.StartHour == HourOfDay && Activity.DaysOfWeek.Contains(NextDay))
			{
				return Activity;
			}
		}
	}
	
	// If no specific next activity found, return idle
	return FScheduledActivity();
}

FDailySchedule UNPCRoutineSystem::GetDailySchedule(AActor* NPCCharacter) const
{
	if (!NPCCharacter)
	{
		return FDailySchedule();
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (NPCSchedules.Contains(NPCPtr))
	{
		return NPCSchedules[NPCPtr];
	}
	
	return FDailySchedule();
}

void UNPCRoutineSystem::SetHomeLocation(AActor* NPCCharacter, FName LocationID)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (!NPCSchedules.Contains(NPCPtr))
	{
		return;
	}
	
	// Update home location
	FDailySchedule& Schedule = NPCSchedules[NPCPtr];
	Schedule.HomeLocation = LocationID;
	
	// Update activities that use home location
	for (FScheduledActivity& Activity : Schedule.Activities)
	{
		if (Activity.LocationID == NAME_None && 
			(Activity.ActivityType == EActivityType::Sleep || Activity.ActivityType == EActivityType::Eat))
		{
			Activity.LocationID = LocationID;
		}
	}
}

void UNPCRoutineSystem::SetWorkLocation(AActor* NPCCharacter, FName LocationID)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (!NPCSchedules.Contains(NPCPtr))
	{
		return;
	}
	
	// Update work location
	FDailySchedule& Schedule = NPCSchedules[NPCPtr];
	Schedule.WorkLocation = LocationID;
	
	// Update activities that use work location
	for (FScheduledActivity& Activity : Schedule.Activities)
	{
		if (Activity.LocationID == NAME_None && Activity.ActivityType == EActivityType::Work)
		{
			Activity.LocationID = LocationID;
		}
	}
}

void UNPCRoutineSystem::AddCustomActivity(AActor* NPCCharacter, const FScheduledActivity& Activity)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (!NPCSchedules.Contains(NPCPtr))
	{
		return;
	}
	
	// Add activity to schedule
	FDailySchedule& Schedule = NPCSchedules[NPCPtr];
	Schedule.Activities.Add(Activity);
	
	// Sort activities by start hour for easier lookup
	Schedule.Activities.Sort([](const FScheduledActivity& A, const FScheduledActivity& B) {
		return A.StartHour < B.StartHour;
	});
}

void UNPCRoutineSystem::RemoveCustomActivity(AActor* NPCCharacter, int32 ActivityIndex)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	if (!NPCSchedules.Contains(NPCPtr))
	{
		return;
	}
	
	FDailySchedule& Schedule = NPCSchedules[NPCPtr];
	
	if (!Schedule.Activities.IsValidIndex(ActivityIndex))
	{
		return;
	}
	
	// Remove activity
	Schedule.Activities.RemoveAt(ActivityIndex);
}

void UNPCRoutineSystem::InterruptWithActivity(AActor* NPCCharacter, const FScheduledActivity& Activity)
{
	if (!NPCCharacter)
	{
		return;
	}
	
	TWeakObjectPtr<AActor> NPCPtr(NPCCharacter);
	
	// Store interruption
	ActivityInterruptions.Add(NPCPtr, Activity);
	
	// Update will happen on next routine update
}

FDailySchedule UNPCRoutineSystem::GenerateDefaultSchedule(FName Occupation, FName SocialClass)
{
	FDailySchedule Schedule;
	Schedule.Occupation = Occupation;
	Schedule.SocialClass = SocialClass;
	
	// Generate different schedules based on occupation
	if (Occupation == "Farmer")
	{
		// Farmers wake up early and work long hours
		
		// Sleep
		FScheduledActivity SleepActivity;
		SleepActivity.ActivityType = EActivityType::Sleep;
		SleepActivity.StartHour = 21; // 9 PM
		SleepActivity.Duration = 7; // 7 hours
		SleepActivity.Priority = 5;
		SleepActivity.bIsMandatory = true;
		Schedule.Activities.Add(SleepActivity);
		
		// Morning meal
		FScheduledActivity BreakfastActivity;
		BreakfastActivity.ActivityType = EActivityType::Eat;
		BreakfastActivity.StartHour = 5; // 5 AM
		BreakfastActivity.Duration = 1;
		BreakfastActivity.Priority = 4;
		Schedule.Activities.Add(BreakfastActivity);
		
		// Morning work
		FScheduledActivity MorningWorkActivity;
		MorningWorkActivity.ActivityType = EActivityType::Work;
		MorningWorkActivity.StartHour = 6; // 6 AM
		MorningWorkActivity.Duration = 6;
		MorningWorkActivity.Priority = 3;
		Schedule.Activities.Add(MorningWorkActivity);
		
		// Midday meal
		FScheduledActivity LunchActivity;
		LunchActivity.ActivityType = EActivityType::Eat;
		LunchActivity.StartHour = 12; // 12 PM
		LunchActivity.Duration = 1;
		LunchActivity.Priority = 4;
		Schedule.Activities.Add(LunchActivity);
		
		// Afternoon work
		FScheduledActivity AfternoonWorkActivity;
		AfternoonWorkActivity.ActivityType = EActivityType::Work;
		AfternoonWorkActivity.StartHour = 13; // 1 PM
		AfternoonWorkActivity.Duration = 5;
		AfternoonWorkActivity.Priority = 3;
		Schedule.Activities.Add(AfternoonWorkActivity);
		
		// Evening meal
		FScheduledActivity DinnerActivity;
		DinnerActivity.ActivityType = EActivityType::Eat;
		DinnerActivity.StartHour = 18; // 6 PM
		DinnerActivity.Duration = 1;
		DinnerActivity.Priority = 4;
		Schedule.Activities.Add(DinnerActivity);
		
		// Evening recreation
		FScheduledActivity EveningActivity;
		EveningActivity.ActivityType = EActivityType::Recreation;
		EveningActivity.StartHour = 19; // 7 PM
		EveningActivity.Duration = 2;
		EveningActivity.Priority = 2;
		Schedule.Activities.Add(EveningActivity);
		
		// Sunday worship
		FScheduledActivity WorshipActivity;
		WorshipActivity.ActivityType = EActivityType::Worship;
		WorshipActivity.StartHour = 10; // 10 AM
		WorshipActivity.Duration = 2;
		WorshipActivity.Priority = 4;
		WorshipActivity.DaysOfWeek = { 0 }; // Sunday only
		Schedule.Activities.Add(WorshipActivity);
	}
	else if (Occupation == "Merchant")
	{
		// Merchants have more regular hours and social activities
		
		// Sleep
		FScheduledActivity SleepActivity;
		SleepActivity.ActivityType = EActivityType::Sleep;
		SleepActivity.StartHour = 22; // 10 PM
		SleepActivity.Duration = 8; // 8 hours
		SleepActivity.Priority = 5;
		SleepActivity.bIsMandatory = true;
		Schedule.Activities.Add(SleepActivity);
		
		// Morning meal
		FScheduledActivity BreakfastActivity;
		BreakfastActivity.ActivityType = EActivityType::Eat;
		BreakfastActivity.StartHour = 7; // 7 AM
		BreakfastActivity.Duration = 1;
		BreakfastActivity.Priority = 4;
		Schedule.Activities.Add(BreakfastActivity);
		
		// Work
		FScheduledActivity WorkActivity;
		WorkActivity.ActivityType = EActivityType::Work;
		WorkActivity.StartHour = 8; // 8 AM
		WorkActivity.Duration = 4;
		WorkActivity.Priority = 3;
		Schedule.Activities.Add(WorkActivity);
		
		// Midday meal
		FScheduledActivity LunchActivity;
		LunchActivity.ActivityType = EActivityType::Eat;
		LunchActivity.StartHour = 12; // 12 PM
		LunchActivity.Duration = 1;
		LunchActivity.Priority = 4;
		Schedule.Activities.Add(LunchActivity);
		
		// Afternoon work
		FScheduledActivity AfternoonWorkActivity;
		AfternoonWorkActivity.ActivityType = EActivityType::Work;
		AfternoonWorkActivity.StartHour = 13; // 1 PM
		AfternoonWorkActivity.Duration = 5;
		AfternoonWorkActivity.Priority = 3;
		Schedule.Activities.Add(AfternoonWorkActivity);
		
		// Evening meal
		FScheduledActivity DinnerActivity;
		DinnerActivity.ActivityType = EActivityType::Eat;
		DinnerActivity.StartHour = 18; // 6 PM
		DinnerActivity.Duration = 1;
		DinnerActivity.Priority = 4;
		Schedule.Activities.Add(DinnerActivity);
		
		// Evening socialization
		FScheduledActivity EveningActivity;
		EveningActivity.ActivityType = EActivityType::Socialize;
		EveningActivity.StartHour = 19; // 7 PM
		EveningActivity.Duration = 3;
		EveningActivity.Priority = 2;
		Schedule.Activities.Add(EveningActivity);
		
		// Sunday worship
		FScheduledActivity WorshipActivity;
		WorshipActivity.ActivityType = EActivityType::Worship;
		WorshipActivity.StartHour = 10; // 10 AM
		WorshipActivity.Duration = 2;
		WorshipActivity.Priority = 4;
		WorshipActivity.DaysOfWeek = { 0 }; // Sunday only
		Schedule.Activities.Add(WorshipActivity);
		
		// Market day activities
		FScheduledActivity MarketActivity;
		MarketActivity.ActivityType = EActivityType::Shopping;
		MarketActivity.StartHour = 9; // 9 AM
		MarketActivity.Duration = 4;
		MarketActivity.Priority = 3;
		MarketActivity.DaysOfWeek = { 3 }; // Wednesday only
		Schedule.Activities.Add(MarketActivity);
	}
	else if (Occupation == "Guard")
	{
		// Guards have shift work and more rigid schedules
		
		// Day shift guards
		
		// Sleep
		FScheduledActivity SleepActivity;
		SleepActivity.ActivityType = EActivityType::Sleep;
		SleepActivity.StartHour = 22; // 10 PM
		SleepActivity.Duration = 8; // 8 hours
		SleepActivity.Priority = 5;
		SleepActivity.bIsMandatory = true;
		Schedule.Activities.Add(SleepActivity);
		
		// Morning meal
		FScheduledActivity BreakfastActivity;
		BreakfastActivity.ActivityType = EActivityType::Eat;
		BreakfastActivity.StartHour = 6; // 6 AM
		BreakfastActivity.Duration = 1;
		BreakfastActivity.Priority = 4;
		Schedule.Activities.Add(BreakfastActivity);
		
		// Guard duty
		FScheduledActivity WorkActivity;
		WorkActivity.ActivityType = EActivityType::Work;
		WorkActivity.StartHour = 7; // 7 AM
		WorkActivity.Duration = 5;
		WorkActivity.Priority = 5;
		WorkActivity.bIsMandatory = true;
		Schedule.Activities.Add(WorkActivity);
		
		// Midday meal
		FScheduledActivity LunchActivity;
		LunchActivity.ActivityType = EActivityType::Eat;
		LunchActivity.StartHour = 12; // 12 PM
		LunchActivity.Duration = 1;
		LunchActivity.Priority = 4;
		Schedule.Activities.Add(LunchActivity);
		
		// Afternoon duty
		FScheduledActivity AfternoonWorkActivity;
		AfternoonWorkActivity.ActivityType = EActivityType::Work;
		AfternoonWorkActivity.StartHour = 13; // 1 PM
		AfternoonWorkActivity.Duration = 5;
		AfternoonWorkActivity.Priority = 5;
		AfternoonWorkActivity.bIsMandatory = true;
		Schedule.Activities.Add(AfternoonWorkActivity);
		
		// Evening meal
		FScheduledActivity DinnerActivity;
		DinnerActivity.ActivityType = EActivityType::Eat;
		DinnerActivity.StartHour = 18; // 6 PM
		DinnerActivity.Duration = 1;
		DinnerActivity.Priority = 4;
		Schedule.Activities.Add(DinnerActivity);
		
		// Evening recreation
		FScheduledActivity EveningActivity;
		EveningActivity.ActivityType = EActivityType::Recreation;
		EveningActivity.StartHour = 19; // 7 PM
		EveningActivity.Duration = 3;
		EveningActivity.Priority = 2;
		Schedule.Activities.Add(EveningActivity);
	}
	else
	{
		// Default schedule for other occupations
		
		// Sleep
		FScheduledActivity SleepActivity;
		SleepActivity.ActivityType = EActivityType::Sleep;
		SleepActivity.StartHour = 22; // 10 PM
		SleepActivity.Duration = 8; // 8 hours
		SleepActivity.Priority = 5;
		SleepActivity.bIsMandatory = true;
		Schedule.Activities.Add(SleepActivity);
		
		// Morning meal
		FScheduledActivity BreakfastActivity;
		BreakfastActivity.ActivityType = EActivityType::Eat;
		BreakfastActivity.StartHour = 7; // 7 AM
		BreakfastActivity.Duration = 1;
		BreakfastActivity.Priority = 4;
		Schedule.Activities.Add(BreakfastActivity);
		
		// Work
		FScheduledActivity WorkActivity;
		WorkActivity.ActivityType = EActivityType::Work;
		WorkActivity.StartHour = 8; // 8 AM
		WorkActivity.Duration = 4;
		WorkActivity.Priority = 3;
		Schedule.Activities.Add(WorkActivity);
		
		// Midday meal
		FScheduledActivity LunchActivity;
		LunchActivity.ActivityType = EActivityType::Eat;
		LunchActivity.StartHour = 12; // 12 PM
		LunchActivity.Duration = 1;
		LunchActivity.Priority = 4;
		Schedule.Activities.Add(LunchActivity);
		
		// Afternoon work
		FScheduledActivity AfternoonWorkActivity;
		AfternoonWorkActivity.ActivityType = EActivityType::Work;
		AfternoonWorkActivity.StartHour = 13; // 1 PM
		AfternoonWorkActivity.Duration = 5;
		AfternoonWorkActivity.Priority = 3;
		Schedule.Activities.Add(AfternoonWorkActivity);
		
		// Evening meal
		FScheduledActivity DinnerActivity;
		DinnerActivity.ActivityType = EActivityType::Eat;
		DinnerActivity.StartHour = 18; // 6 PM
		DinnerActivity.Duration = 1;
		DinnerActivity.Priority = 4;
		Schedule.Activities.Add(DinnerActivity);
		
		// Evening recreation
		FScheduledActivity EveningActivity;
		EveningActivity.ActivityType = EActivityType::Recreation;
		EveningActivity.StartHour = 19; // 7 PM
		EveningActivity.Duration = 2;
		EveningActivity.Priority = 2;
		Schedule.Activities.Add(EveningActivity);
	}
	
	// Sort activities by start hour for easier lookup
	Schedule.Activities.Sort([](const FScheduledActivity& A, const FScheduledActivity& B) {
		return A.StartHour < B.StartHour;
	});
	
	return Schedule;
}

FScheduledActivity UNPCRoutineSystem::GetActivityForTime(const FDailySchedule& Schedule, int32 CurrentHour, int32 CurrentDay) const
{
	// Check for activities that should be active at the current time
	TArray<FScheduledActivity> ActiveActivities;
	
	for (const FScheduledActivity& Activity : Schedule.Activities)
	{
		if (IsActivityActiveAtTime(Activity, CurrentHour, CurrentDay))
		{
			ActiveActivities.Add(Activity);
		}
	}
	
	// If multiple activities are active, choose the one with highest priority
	if (ActiveActivities.Num() > 0)
	{
		ActiveActivities.Sort([](const FScheduledActivity& A, const FScheduledActivity& B) {
			return A.Priority > B.Priority; // Sort by priority (descending)
		});
		
		return ActiveActivities[0];
	}
	
	// If no activities are active, return idle
	FScheduledActivity IdleActivity;
	IdleActivity.ActivityType = EActivityType::Idle;
	
	// Set location based on time of day
	if (CurrentHour >= 22 || CurrentHour < 6)
	{
		// Late night, likely at home
		IdleActivity.LocationID = Schedule.HomeLocation;
	}
	else if (CurrentHour >= 8 && CurrentHour < 18)
	{
		// Work hours, likely at work
		IdleActivity.LocationID = Schedule.WorkLocation;
	}
	else
	{
		// Other times, could be anywhere
		IdleActivity.LocationID = Schedule.HomeLocation;
	}
	
	return IdleActivity;
}

bool UNPCRoutineSystem::IsActivityActiveAtTime(const FScheduledActivity& Activity, int32 CurrentHour, int32 CurrentDay) const
{
	// Check if activity is scheduled for this day
	if (!Activity.DaysOfWeek.Contains(CurrentDay))
	{
		return false;
	}
	
	// Calculate end hour (wrapping around to next day if needed)
	int32 EndHour = (Activity.StartHour + Activity.Duration) % 24;
	
	// Check if current hour falls within activity time
	if (EndHour > Activity.StartHour)
	{
		// Activity starts and ends on the same day
		return (CurrentHour >= Activity.StartHour && CurrentHour < EndHour);
	}
	else
	{
		// Activity spans midnight
		return (CurrentHour >= Activity.StartHour || CurrentHour < EndHour);
	}
}

void UNPCRoutineSystem::ModifySchedulesForEnvironment()
{
	// This would integrate with weather and other systems to modify schedules
	// For example, NPCs might stay indoors during bad weather
	
	// Get current weather
	// This would integrate with the WeatherSystem
	FName CurrentWeather = "Clear"; // Placeholder - would get from WeatherSystem
	
	// Modify schedules based on weather
	if (CurrentWeather == "Rain" || CurrentWeather == "Snow" || CurrentWeather == "Storm")
	{
		// Modify all NPC schedules to prefer indoor activities
		for (auto& SchedulePair : NPCSchedules)
		{
			// Example: Interrupt outdoor activities with indoor alternatives
			// This is a placeholder for more sophisticated weather response
		}
	}
}