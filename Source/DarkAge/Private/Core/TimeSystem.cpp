// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/TimeSystem.h"

UTimeSystem::UTimeSystem()
	: TimeScale(1.0f)
	, AccumulatedTime(0.0f)
{
}

void UTimeSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize with default date and time
	CurrentDateTime = FGameDateTime();
	
	// Initialize season based on starting date
	UpdateSeason();
	
	// Add some initial calendar events
	AddCalendarEvent(FGameDateTime{ 50, 4, 15, 10, 0 }, "Festival", "Spring Festival in Ironhold");
	AddCalendarEvent(FGameDateTime{ 50, 7, 1, 12, 0 }, "Market", "Grand Market in Crossroads");
	AddCalendarEvent(FGameDateTime{ 50, 10, 31, 18, 0 }, "Ceremony", "Harvest Ceremony in Millhaven");
}

void UTimeSystem::Deinitialize()
{
	Super::Deinitialize();
	
	// Clean up any resources
	CalendarEvents.Empty();
}

void UTimeSystem::UpdateTime(float DeltaTime)
{
	// Apply time scale
	float scaledDelta = DeltaTime * TimeScale;
	
	// Accumulate time until we reach a full minute
	AccumulatedTime += scaledDelta;
	
	// Each real second represents 1 minute in game time at normal time scale
	const float SecondsPerGameMinute = 1.0f;
	
	if (AccumulatedTime >= SecondsPerGameMinute)
	{
		// Calculate how many minutes to advance
		int32 MinutesToAdd = FMath::FloorToInt(AccumulatedTime / SecondsPerGameMinute);
		AccumulatedTime -= MinutesToAdd * SecondsPerGameMinute;
		
		// Store previous day for comparison
		int32 PreviousDay = CurrentDateTime.Day;
		FName PreviousSeason = GetCurrentSeason();
		
		// Update minutes
		CurrentDateTime.Minute += MinutesToAdd;
		
		// Handle minute overflow
		if (CurrentDateTime.Minute >= 60)
		{
			CurrentDateTime.Hour += CurrentDateTime.Minute / 60;
			CurrentDateTime.Minute %= 60;
			
			// Handle hour overflow
			if (CurrentDateTime.Hour >= 24)
			{
				CurrentDateTime.Day += CurrentDateTime.Hour / 24;
				CurrentDateTime.Hour %= 24;
				
				// Handle day overflow (simplified - all months have 30 days)
				if (CurrentDateTime.Day > 30)
				{
					CurrentDateTime.Month += CurrentDateTime.Day / 30;
					CurrentDateTime.Day = (CurrentDateTime.Day % 30 == 0) ? 30 : CurrentDateTime.Day % 30;
					
					// Handle month overflow
					if (CurrentDateTime.Month > 12)
					{
						CurrentDateTime.Year += CurrentDateTime.Month / 12;
						CurrentDateTime.Month = (CurrentDateTime.Month % 12 == 0) ? 12 : CurrentDateTime.Month % 12;
					}
				}
			}
		}
		
		// Broadcast time changed event
		OnTimeChanged.Broadcast(CurrentDateTime);
		
		// Check if day changed
		if (CurrentDateTime.Day != PreviousDay)
		{
			OnDayChanged.Broadcast();
			
			// Update season and check if it changed
			UpdateSeason();
			if (GetCurrentSeason() != PreviousSeason)
			{
				OnSeasonChanged.Broadcast(GetCurrentSeason());
			}
		}
		
		// Check for calendar events
		CheckCalendarEvents();
	}
}

FGameDateTime UTimeSystem::GetCurrentDateTime() const
{
	return CurrentDateTime;
}

FName UTimeSystem::GetCurrentSeason() const
{
	// Determine season based on month
	// Spring: Months 3-5
	// Summer: Months 6-8
	// Fall: Months 9-11
	// Winter: Months 12, 1-2
	
	if (CurrentDateTime.Month >= 3 && CurrentDateTime.Month <= 5)
	{
		return "Spring";
	}
	else if (CurrentDateTime.Month >= 6 && CurrentDateTime.Month <= 8)
	{
		return "Summer";
	}
	else if (CurrentDateTime.Month >= 9 && CurrentDateTime.Month <= 11)
	{
		return "Fall";
	}
	else
	{
		return "Winter";
	}
}

float UTimeSystem::GetTimeOfDay() const
{
	// Convert current hour and minute to a 0.0-1.0 value
	// where 0.0 is midnight and 0.5 is noon
	return (CurrentDateTime.Hour * 60.0f + CurrentDateTime.Minute) / (24.0f * 60.0f);
}

bool UTimeSystem::IsDaytime() const
{
	// Check if current time is between sunrise and sunset
	return (CurrentDateTime.Hour >= 6 && CurrentDateTime.Hour < 18);
}

float UTimeSystem::GetSunriseTime() const
{
	// Sunrise time varies by season
	// This is a simplified implementation
	FName CurrentSeason = GetCurrentSeason();
	
	if (CurrentSeason == "Summer")
	{
		return 5.0f; // 5:00 AM
	}
	else if (CurrentSeason == "Spring" || CurrentSeason == "Fall")
	{
		return 6.0f; // 6:00 AM
	}
	else // Winter
	{
		return 7.0f; // 7:00 AM
	}
}

float UTimeSystem::GetSunsetTime() const
{
	// Sunset time varies by season
	// This is a simplified implementation
	FName CurrentSeason = GetCurrentSeason();
	
	if (CurrentSeason == "Summer")
	{
		return 21.0f; // 9:00 PM
	}
	else if (CurrentSeason == "Spring" || CurrentSeason == "Fall")
	{
		return 19.0f; // 7:00 PM
	}
	else // Winter
	{
		return 17.0f; // 5:00 PM
	}
}

void UTimeSystem::SetTimeScale(float NewTimeScale)
{
	// Clamp time scale to reasonable values
	TimeScale = FMath::Clamp(NewTimeScale, 0.1f, 100.0f);
}

void UTimeSystem::SetCurrentDateTime(const FGameDateTime& NewDateTime)
{
	CurrentDateTime = NewDateTime;
	OnTimeChanged.Broadcast(CurrentDateTime);
}

void UTimeSystem::AddCalendarEvent(const FGameDateTime& EventTime, FName EventType, const FString& EventDescription)
{
	// Create a unique key for this event
	FString EventKey = DateTimeToString(EventTime) + "_" + EventType.ToString();
	
	// Store the event
	CalendarEvents.Add(EventKey, TPair<FGameDateTime, FString>(EventTime, EventDescription));
}

TArray<FString> UTimeSystem::GetUpcomingEvents(int32 DaysAhead) const
{
	TArray<FString> UpcomingEvents;
	
	// Calculate the end date for our search
	FGameDateTime EndDate = CurrentDateTime;
	EndDate.Day += DaysAhead;
	
	// Handle month/year overflow
	if (EndDate.Day > 30)
	{
		EndDate.Month += EndDate.Day / 30;
		EndDate.Day = (EndDate.Day % 30 == 0) ? 30 : EndDate.Day % 30;
		
		if (EndDate.Month > 12)
		{
			EndDate.Year += EndDate.Month / 12;
			EndDate.Month = (EndDate.Month % 12 == 0) ? 12 : EndDate.Month % 12;
		}
	}
	
	// Check all events
	for (const auto& EventPair : CalendarEvents)
	{
		const FGameDateTime& EventTime = EventPair.Value.Key;
		const FString& EventDesc = EventPair.Value.Value;
		
		// Check if event is within our time range
		if (EventTime.Year > CurrentDateTime.Year || 
			(EventTime.Year == CurrentDateTime.Year && EventTime.Month > CurrentDateTime.Month) ||
			(EventTime.Year == CurrentDateTime.Year && EventTime.Month == CurrentDateTime.Month && EventTime.Day >= CurrentDateTime.Day))
		{
			if (EventTime.Year < EndDate.Year ||
				(EventTime.Year == EndDate.Year && EventTime.Month < EndDate.Month) ||
				(EventTime.Year == EndDate.Year && EventTime.Month == EndDate.Month && EventTime.Day <= EndDate.Day))
			{
				// Event is within range, add to list
				FString EventString = FString::Printf(TEXT("Year %d, Month %d, Day %d at %02d:%02d - %s"),
					EventTime.Year, EventTime.Month, EventTime.Day, EventTime.Hour, EventTime.Minute,
					*EventDesc);
				
				UpcomingEvents.Add(EventString);
			}
		}
	}
	
	return UpcomingEvents;
}

void UTimeSystem::UpdateSeason()
{
	// This is a placeholder - the actual season is calculated in GetCurrentSeason()
	// This method would be used for any additional logic needed when seasons change
}

void UTimeSystem::CheckCalendarEvents()
{
	// Check if any events are happening at the current time
	FString CurrentTimeKey = DateTimeToString(CurrentDateTime);
	
	// Look for events that match the current time
	for (const auto& EventPair : CalendarEvents)
	{
		const FGameDateTime& EventTime = EventPair.Value.Key;
		const FString& EventDesc = EventPair.Value.Value;
		
		// Check if event matches current date and time
		if (EventTime.Year == CurrentDateTime.Year &&
			EventTime.Month == CurrentDateTime.Month &&
			EventTime.Day == CurrentDateTime.Day &&
			EventTime.Hour == CurrentDateTime.Hour &&
			EventTime.Minute == CurrentDateTime.Minute)
		{
			// Event is happening now - trigger notification or event
			// This would integrate with a notification system
			UE_LOG(LogTemp, Display, TEXT("Calendar Event: %s"), *EventDesc);
			
			// Additional event handling would go here
		}
	}
}

FString UTimeSystem::DateTimeToString(const FGameDateTime& DateTime) const
{
	return FString::Printf(TEXT("%04d_%02d_%02d_%02d_%02d"),
		DateTime.Year, DateTime.Month, DateTime.Day, DateTime.Hour, DateTime.Minute);
}