// Copyright (C) 2024, Epic Games, Inc. All Rights Reserved.

#include "Core/TemporalManager.h"
#include "Core/TimeSystem.h"
#include "Kismet/GameplayStatics.h"

void UTemporalManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TimeSystem = GetGameInstance()->GetSubsystem<UTimeSystem>();
	if (TimeSystem)
	{
		TimeSystem->OnTimeChanged.AddDynamic(this, &UTemporalManager::HandleTimeUpdated);
	}
}

void UTemporalManager::Deinitialize()
{
	if (TimeSystem)
	{
		TimeSystem->OnTimeChanged.RemoveDynamic(this, &UTemporalManager::HandleTimeUpdated);
	}
	Super::Deinitialize();
}

void UTemporalManager::SetGlobalTimeDilation(float Dilation)
{
	GlobalTimeDilation = FMath::Clamp(Dilation, 0.1f, 10.0f);
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, GlobalTimeDilation);
	}
}

float UTemporalManager::GetGlobalTimeDilation() const
{
	return GlobalTimeDilation;
}

void UTemporalManager::StartGlobalTimeLoop(const FGameDateTime& StartTime, const FGameDateTime& EndTime)
{
	TimeLoopStartPoint = StartTime;
	TimeLoopEndPoint = EndTime;
	bIsTimeLoopActive = true;
}

void UTemporalManager::StopGlobalTimeLoop()
{
	bIsTimeLoopActive = false;
}

bool UTemporalManager::IsTimeLoopActive() const
{
	return bIsTimeLoopActive;
}

void UTemporalManager::HandleTimeUpdated(const FGameDateTime& NewTime)
{
	if (bIsTimeLoopActive && NewTime > TimeLoopEndPoint)
	{
		if (TimeSystem)
		{
			TimeSystem->SetCurrentDateTime(TimeLoopStartPoint);
			OnTimeLoopReset.Broadcast();
		}
	}
}