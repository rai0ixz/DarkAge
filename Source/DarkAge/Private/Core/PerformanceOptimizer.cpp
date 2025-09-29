#include "Core/PerformanceOptimizer.h"
#include "Engine/World.h"
#include "HAL/PlatformMemory.h"
#include "Misc/ScopeLock.h"
#include "Engine/Engine.h"

UPerformanceOptimizer::UPerformanceOptimizer()
{
	CurrentLevel = EPerformanceLevel::High;
	TargetFPS = 60.0f;
	MinAcceptableFPS = 30.0f;
	MemoryThreshold = 8.0f * 1024.0f; // 8 GB as example upper bound in MB
	MetricsUpdateTimer = 0.0f;
	FrameTimeHistory.Reset();
}

void UPerformanceOptimizer::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentMetrics = FPerformanceMetrics();
	CurrentSettings = FPerformanceSettings();
	OptimizeForPlatform();
	UpdateQualitySettings();
}

void UPerformanceOptimizer::Deinitialize()
{
	FrameTimeHistory.Reset();
	Super::Deinitialize();
}

void UPerformanceOptimizer::UpdateMetrics(float DeltaTime)
{
	MetricsUpdateTimer += DeltaTime;

	// Simulate basic metric collection (replace with real queries where available)
	CurrentMetrics.FrameTime = DeltaTime;
	CurrentMetrics.FPS = (DeltaTime > KINDA_SMALL_NUMBER) ? (1.0f / DeltaTime) : 0.0f;

	// Memory usage (in MB)
	const FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
	CurrentMetrics.MemoryUsage = MemStats.UsedPhysical / (1024.0f * 1024.0f);

	// Simple smoothing buffer
	FrameTimeHistory.Add(CurrentMetrics.FrameTime);
	if (FrameTimeHistory.Num() > 120)
	{
		FrameTimeHistory.RemoveAt(0);
	}

	CollectPerformanceData();
	AnalyzePerformanceTrends();
	DetectPerformanceIssues();

	if (!IsPerformanceAcceptable())
	{
		OnLowPerformanceDetected.Broadcast(CurrentMetrics);
		AutoAdjustSettings();
	}
}

void UPerformanceOptimizer::LogPerformanceData()
{
	UE_LOG(LogTemp, Log, TEXT("[Performance] FPS=%.1f FT=%.3fms Mem=%.1fMB DrawCalls=%d Tris=%d NPC=%d PFX=%d"),
		CurrentMetrics.FPS,
		CurrentMetrics.FrameTime * 1000.0f,
		CurrentMetrics.MemoryUsage,
		CurrentMetrics.DrawCalls,
		CurrentMetrics.TriangleCount,
		CurrentMetrics.ActiveNPCs,
		CurrentMetrics.ActiveParticles);
}

void UPerformanceOptimizer::SetPerformanceLevel(EPerformanceLevel Level)
{
	CurrentLevel = Level;
	ApplyPerformancePreset(Level);
	OnPerformanceLevelChanged.Broadcast(Level);
	UpdateQualitySettings();
}

void UPerformanceOptimizer::AdjustSettingsBasedOnPerformance()
{
	AutoAdjustSettings();
	UpdateQualitySettings();
}

void UPerformanceOptimizer::ForceGarbageCollection()
{
	// Request GC
	if (GEngine)
	{
		GEngine->ForceGarbageCollection(true);
	}
}

void UPerformanceOptimizer::OptimizeMemoryUsage()
{
	// Placeholder: could unload streaming levels, trim pools, etc.
	ForceGarbageCollection();
}

float UPerformanceOptimizer::GetMemoryUsage() const
{
	return CurrentMetrics.MemoryUsage;
}

void UPerformanceOptimizer::CullDistantNPCs()
{
	// Placeholder: integrate with NPC manager to disable far NPCs
}

void UPerformanceOptimizer::OptimizeNPCUpdates()
{
	// Placeholder: reduce NPC tick frequency under load
}

void UPerformanceOptimizer::SetMaxActiveNPCs(int32 MaxNPCs)
{
	CurrentSettings.MaxNPCs = FMath::Max(0, MaxNPCs);
}

void UPerformanceOptimizer::UpdateLODSettings()
{
	// Placeholder: adjust engine scalability LOD bias if desired
}

void UPerformanceOptimizer::OptimizeDrawDistance()
{
	// Placeholder: adjust view distance
	CurrentSettings.ViewDistance = FMath::Clamp(CurrentSettings.ViewDistance * 0.95f, 2000.0f, 20000.0f);
}

void UPerformanceOptimizer::EnableOcclusionCulling(bool bEnable)
{
	CurrentSettings.bEnableOcclusionCulling = bEnable;
}

void UPerformanceOptimizer::UpdateStreamingSettings()
{
	// Placeholder: tweak streaming settings
}

void UPerformanceOptimizer::PreloadCriticalAssets()
{
	// Placeholder: pre-stream critical assets by path
}

void UPerformanceOptimizer::UnloadUnusedAssets()
{
	// Placeholder: unload non-critical assets
	ForceGarbageCollection();
}

// --------------------- Private helpers ---------------------

void UPerformanceOptimizer::CollectPerformanceData()
{
	// Placeholders for counters; integrate with engine stats as needed
	CurrentMetrics.DrawCalls = FMath::Clamp(CurrentMetrics.DrawCalls + FMath::RandRange(-5, 5), 0, 5000);
	CurrentMetrics.TriangleCount = FMath::Clamp(CurrentMetrics.TriangleCount + FMath::RandRange(-1000, 1500), 0, 5'000'000);
	CurrentMetrics.ActiveNPCs = FMath::Clamp(CurrentMetrics.ActiveNPCs + FMath::RandRange(-2, 3), 0, 1000);
	CurrentMetrics.ActiveParticles = FMath::Clamp(CurrentMetrics.ActiveParticles + FMath::RandRange(-5, 10), 0, 10000);
}

void UPerformanceOptimizer::AnalyzePerformanceTrends()
{
	// Simple trend analysis of last N frames
	if (FrameTimeHistory.Num() >= 30)
	{
		float AvgFT = 0.0f;
		for (float FT : FrameTimeHistory) AvgFT += FT;
		AvgFT /= FrameTimeHistory.Num();

		// Adjust MinAcceptableFPS dynamically if desired
		if (AvgFT > (1.0f / MinAcceptableFPS))
		{
			// trending poorly
		}
	}
}

void UPerformanceOptimizer::ApplyPerformancePreset(EPerformanceLevel Level)
{
	switch (Level)
	{
	case EPerformanceLevel::UltraLow:
		CurrentSettings.MaxNPCs = 30;
		CurrentSettings.ViewDistance = 2500.0f;
		CurrentSettings.ShadowQuality = 0;
		CurrentSettings.TextureQuality = 0;
		CurrentSettings.EffectsQuality = 0;
		CurrentSettings.bEnableLOD = true;
		CurrentSettings.bEnableOcclusionCulling = true;
		CurrentSettings.bEnableDynamicResolution = true;
		break;
	case EPerformanceLevel::Low:
		CurrentSettings.MaxNPCs = 50;
		CurrentSettings.ViewDistance = 3500.0f;
		CurrentSettings.ShadowQuality = 1;
		CurrentSettings.TextureQuality = 1;
		CurrentSettings.EffectsQuality = 1;
		CurrentSettings.bEnableLOD = true;
		CurrentSettings.bEnableOcclusionCulling = true;
		CurrentSettings.bEnableDynamicResolution = true;
		break;
	case EPerformanceLevel::Medium:
		CurrentSettings.MaxNPCs = 80;
		CurrentSettings.ViewDistance = 5000.0f;
		CurrentSettings.ShadowQuality = 2;
		CurrentSettings.TextureQuality = 2;
		CurrentSettings.EffectsQuality = 2;
		CurrentSettings.bEnableLOD = true;
		CurrentSettings.bEnableOcclusionCulling = true;
		CurrentSettings.bEnableDynamicResolution = false;
		break;
	case EPerformanceLevel::High:
		CurrentSettings.MaxNPCs = 120;
		CurrentSettings.ViewDistance = 7000.0f;
		CurrentSettings.ShadowQuality = 3;
		CurrentSettings.TextureQuality = 3;
		CurrentSettings.EffectsQuality = 3;
		CurrentSettings.bEnableLOD = true;
		CurrentSettings.bEnableOcclusionCulling = true;
		CurrentSettings.bEnableDynamicResolution = false;
		break;
	case EPerformanceLevel::Ultra:
		CurrentSettings.MaxNPCs = 200;
		CurrentSettings.ViewDistance = 10000.0f;
		CurrentSettings.ShadowQuality = 4;
		CurrentSettings.TextureQuality = 4;
		CurrentSettings.EffectsQuality = 4;
		CurrentSettings.bEnableLOD = true;
		CurrentSettings.bEnableOcclusionCulling = true;
		CurrentSettings.bEnableDynamicResolution = false;
		break;
	case EPerformanceLevel::Custom:
	default:
		break;
	}
}

void UPerformanceOptimizer::DetectPerformanceIssues()
{
	// If FPS frequently below minimum, warn
	if (CurrentMetrics.FPS < MinAcceptableFPS)
	{
		LogPerformanceWarning(TEXT("FPS below acceptable threshold"));
	}
	if (CurrentMetrics.MemoryUsage > MemoryThreshold)
	{
		LogPerformanceWarning(TEXT("Memory usage above threshold"));
	}
}

void UPerformanceOptimizer::AutoAdjustSettings()
{
	// Simple auto adjustment policy
	if (CurrentMetrics.FPS < MinAcceptableFPS)
	{
		// Reduce view distance slightly
		CurrentSettings.ViewDistance = FMath::Max(2000.0f, CurrentSettings.ViewDistance - 250.0f);
		// Lower effects quality if high
		CurrentSettings.EffectsQuality = FMath::Max(0, CurrentSettings.EffectsQuality - 1);
	}
}

bool UPerformanceOptimizer::IsPerformanceAcceptable() const
{
	return (CurrentMetrics.FPS >= MinAcceptableFPS) && (CurrentMetrics.MemoryUsage <= MemoryThreshold);
}

void UPerformanceOptimizer::LogPerformanceWarning(const FString& Warning)
{
	UE_LOG(LogTemp, Warning, TEXT("[Performance Warning] %s | FPS=%.1f Mem=%.1fMB"),
		*Warning, CurrentMetrics.FPS, CurrentMetrics.MemoryUsage);
}

void UPerformanceOptimizer::OptimizeForPlatform()
{
	// Placeholder: apply platform-specific defaults
}

void UPerformanceOptimizer::UpdateQualitySettings()
{
	// Placeholder: push CurrentSettings to engine scalability if desired
}