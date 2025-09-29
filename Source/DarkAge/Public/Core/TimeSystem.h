// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimeSystem.generated.h"

/**
 * Time structure to represent in-game date and time
 */
USTRUCT(BlueprintType)
struct FGameDateTime
{
	GENERATED_BODY()
	
	// Year since the Sundering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Year;
	
	// Month (1-12)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Month;
	
	// Day (1-30)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Day;
	
	// Hour (0-23)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Hour;
	
	// Minute (0-59)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Minute;
	
	FGameDateTime()
		: Year(50)
		, Month(1)
		, Day(1)
		, Hour(8)
		, Minute(0)
	{
	}
	
	FGameDateTime(int32 InYear, int32 InMonth, int32 InDay, int32 InHour, int32 InMinute)
		: Year(InYear)
		, Month(InMonth)
		, Day(InDay)
		, Hour(InHour)
		, Minute(InMinute)
	{
	}
	
	// Get total days (for comparison)
	int32 GetTotalDays() const
	{
		return (Year * 360) + ((Month - 1) * 30) + Day;
	}
	
	// Get day of year (1-360)
	int32 GetDayOfYear() const
	{
		return ((Month - 1) * 30) + Day;
	}
	
	// Add days to the date
	void AddDays(int32 DaysToAdd)
	{
		int32 TotalDays = GetTotalDays() + DaysToAdd;
		Year = TotalDays / 360;
		int32 RemainingDays = TotalDays % 360;
		Month = (RemainingDays / 30) + 1;
		Day = (RemainingDays % 30) + 1;
	}
	
	// Convert to string
	FString ToString() const
	{
		return FString::Printf(TEXT("Year %d, Month %d, Day %d, %02d:%02d"), Year, Month, Day, Hour, Minute);
	}
	
	// Comparison operators
	bool operator>(const FGameDateTime& Other) const
	{
		return GetTotalDays() > Other.GetTotalDays() || 
			(GetTotalDays() == Other.GetTotalDays() && (Hour > Other.Hour || 
				(Hour == Other.Hour && Minute > Other.Minute)));
	}
	
	bool operator<(const FGameDateTime& Other) const
	{
		return GetTotalDays() < Other.GetTotalDays() || 
			(GetTotalDays() == Other.GetTotalDays() && (Hour < Other.Hour || 
				(Hour == Other.Hour && Minute < Other.Minute)));
	}
	
	bool operator==(const FGameDateTime& Other) const
	{
		return Year == Other.Year && Month == Other.Month && Day == Other.Day && 
			Hour == Other.Hour && Minute == Other.Minute;
	}
	
	// Subtraction operator to get time difference
	FGameDateTime operator-(const FGameDateTime& Other) const
	{
		FGameDateTime Result;
		int32 TotalDays1 = GetTotalDays();
		int32 TotalDays2 = Other.GetTotalDays();
		int32 DayDiff = TotalDays1 - TotalDays2;
		
		Result.Year = DayDiff / 360;
		int32 RemainingDays = DayDiff % 360;
		Result.Month = (RemainingDays / 30) + 1;
		Result.Day = (RemainingDays % 30) + 1;
		
		// Handle time difference
		int32 TotalMinutes1 = Hour * 60 + Minute;
		int32 TotalMinutes2 = Other.Hour * 60 + Other.Minute;
		int32 MinuteDiff = TotalMinutes1 - TotalMinutes2;
		
		if (MinuteDiff < 0)
		{
			// Borrow a day
			Result.AddDays(-1);
			MinuteDiff += 24 * 60;
		}
		
		Result.Hour = MinuteDiff / 60;
		Result.Minute = MinuteDiff % 60;
		
		return Result;
	}
	
	// Get days difference
	int32 GetDays() const
	{
		return GetTotalDays();
	}
};

/**
 * Time System for Dark Age
 * 
 * Manages the passage of time, day/night cycle, seasons, and calendar events.
 * Time affects NPC schedules, weather patterns, and gameplay mechanics.
 */
UCLASS()
class DARKAGE_API UTimeSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UTimeSystem();
	
	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Deinitialize the subsystem
	virtual void Deinitialize() override;
	
	// Update time based on real-time elapsed
	UFUNCTION(BlueprintCallable, Category = "Time")
	void UpdateTime(float DeltaTime);
	
	// Get current game date and time
	UFUNCTION(BlueprintCallable, Category = "Time")
	FGameDateTime GetCurrentDateTime() const;
	
	// Get current season
	UFUNCTION(BlueprintCallable, Category = "Time")
	FName GetCurrentSeason() const;
	
	// Get current time of day (0.0 - 1.0, where 0.0 is midnight and 0.5 is noon)
	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetTimeOfDay() const;
	
	// Check if it's currently daytime
	UFUNCTION(BlueprintCallable, Category = "Time")
	bool IsDaytime() const;
	
	// Get sunrise time for current day
	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetSunriseTime() const;
	
	// Get sunset time for current day
	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetSunsetTime() const;
	
	// Set time scale (1.0 = normal, 2.0 = 2x speed, etc.)
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetTimeScale(float NewTimeScale);

	// Set current game date and time
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetCurrentDateTime(const FGameDateTime& NewDateTime);
	
	// Add a calendar event
	UFUNCTION(BlueprintCallable, Category = "Time")
	void AddCalendarEvent(const FGameDateTime& EventTime, FName EventType, const FString& EventDescription);
	
	// Get upcoming calendar events
	UFUNCTION(BlueprintCallable, Category = "Time")
	TArray<FString> GetUpcomingEvents(int32 DaysAhead) const;
	
	// Delegate for time change events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, const FGameDateTime&, NewTime);
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnTimeChanged OnTimeChanged;
	
	// Delegate for day change events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayChanged);
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnDayChanged OnDayChanged;
	
	// Delegate for season change events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, FName, NewSeason);
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnSeasonChanged OnSeasonChanged;
	
private:
	// Current game date and time
	UPROPERTY()
	FGameDateTime CurrentDateTime;
	
	// Time scale multiplier
	UPROPERTY()
	float TimeScale;
	
	// Accumulated time since last minute increment
	float AccumulatedTime;
	
	// Calendar events - stored as serialized data
	UPROPERTY()
	TMap<FString, FString> CalendarEventData;
	
	// Calendar events map for runtime use
	TMap<FString, TPair<FGameDateTime, FString>> CalendarEvents;
	
	// Update season based on current date
	void UpdateSeason();
	
	// Check for and trigger calendar events
	void CheckCalendarEvents();
	
	// Convert game time to a unique string key for the calendar
	FString DateTimeToString(const FGameDateTime& DateTime) const;
};