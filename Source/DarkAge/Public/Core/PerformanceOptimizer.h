#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PerformanceOptimizer.generated.h"

UENUM(BlueprintType)
enum class EPerformanceLevel : uint8
{
    UltraLow      UMETA(DisplayName = "Ultra Low"),
    Low           UMETA(DisplayName = "Low"),
    Medium        UMETA(DisplayName = "Medium"),
    High          UMETA(DisplayName = "High"),
    Ultra         UMETA(DisplayName = "Ultra"),
    Custom        UMETA(DisplayName = "Custom")
};

USTRUCT(BlueprintType)
struct FPerformanceMetrics
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    float FrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    float FPS;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 DrawCalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 TriangleCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    float MemoryUsage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 ActiveNPCs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 ActiveParticles;

    FPerformanceMetrics()
    {
        FrameTime = 0.0f;
        FPS = 0.0f;
        DrawCalls = 0;
        TriangleCount = 0;
        MemoryUsage = 0.0f;
        ActiveNPCs = 0;
        ActiveParticles = 0;
    }
};

USTRUCT(BlueprintType)
struct FPerformanceSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 MaxNPCs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float ViewDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 ShadowQuality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 TextureQuality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    int32 EffectsQuality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool bEnableLOD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool bEnableOcclusionCulling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool bEnableDynamicResolution;

    FPerformanceSettings()
    {
        MaxNPCs = 100;
        ViewDistance = 5000.0f;
        ShadowQuality = 2;
        TextureQuality = 2;
        EffectsQuality = 2;
        bEnableLOD = true;
        bEnableOcclusionCulling = true;
        bEnableDynamicResolution = false;
    }
};

UCLASS()
class DARKAGE_API UPerformanceOptimizer : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPerformanceOptimizer();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Performance monitoring
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void UpdateMetrics(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Performance")
    const FPerformanceMetrics& GetCurrentMetrics() const { return CurrentMetrics; }

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void LogPerformanceData();

    // Dynamic performance adjustment
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void SetPerformanceLevel(EPerformanceLevel Level);

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void AdjustSettingsBasedOnPerformance();

    UFUNCTION(BlueprintPure, Category = "Performance")
    const FPerformanceSettings& GetCurrentSettings() const { return CurrentSettings; }

    // Memory management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void ForceGarbageCollection();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void OptimizeMemoryUsage();

    UFUNCTION(BlueprintPure, Category = "Performance")
    float GetMemoryUsage() const;

    // NPC optimization
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void CullDistantNPCs();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void OptimizeNPCUpdates();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void SetMaxActiveNPCs(int32 MaxNPCs);

    // Rendering optimization
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void UpdateLODSettings();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void OptimizeDrawDistance();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void EnableOcclusionCulling(bool bEnable);

    // Asset streaming
    UFUNCTION(BlueprintCallable, Category = "Performance")
    void UpdateStreamingSettings();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void PreloadCriticalAssets();

    UFUNCTION(BlueprintCallable, Category = "Performance")
    void UnloadUnusedAssets();

    // Performance events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceLevelChanged, EPerformanceLevel, NewLevel);
    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceLevelChanged OnPerformanceLevelChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLowPerformanceDetected, const FPerformanceMetrics&, Metrics);
    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnLowPerformanceDetected OnLowPerformanceDetected;

private:
    // Performance data
    UPROPERTY()
    FPerformanceMetrics CurrentMetrics;

    UPROPERTY()
    FPerformanceSettings CurrentSettings;

    UPROPERTY()
    EPerformanceLevel CurrentLevel;

    // Performance thresholds
    UPROPERTY()
    float TargetFPS;

    UPROPERTY()
    float MinAcceptableFPS;

    UPROPERTY()
    float MemoryThreshold;

    // Timing data
    UPROPERTY()
    float MetricsUpdateTimer;

    UPROPERTY()
    TArray<float> FrameTimeHistory;

    // Core functions
    void CollectPerformanceData();
    void AnalyzePerformanceTrends();
    void ApplyPerformancePreset(EPerformanceLevel Level);
    void DetectPerformanceIssues();
    void AutoAdjustSettings();
    bool IsPerformanceAcceptable() const;
    void LogPerformanceWarning(const FString& Warning);
    void OptimizeForPlatform();
    void UpdateQualitySettings();
};