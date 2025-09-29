#include "Core/AdvancedWeatherImpactSubsystem.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "Core/NPCEcosystemSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedWeatherImpactSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize weather impact data
    InitializeWeatherEffects();
    
    // Set up weather monitoring
    WeatherUpdateTimer = 0.0f;
    CurrentWeatherIntensity = 0.5f;
    CurrentWeatherType = EWeatherType::Clear;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedWeatherImpactSubsystem initialized"));
}

void UAdvancedWeatherImpactSubsystem::Tick(float DeltaTime)
{
    
    WeatherUpdateTimer += DeltaTime;
    
    // Update weather effects every 5 seconds
    if (WeatherUpdateTimer >= 5.0f)
    {
        UpdateWeatherEffects();
        WeatherUpdateTimer = 0.0f;
    }
    
    // Apply continuous weather effects
    ApplyContinuousWeatherEffects(DeltaTime);
}

void UAdvancedWeatherImpactSubsystem::InitializeWeatherEffects()
{
    // Clear Weather Effects
    FWeatherImpactData ClearWeather;
    ClearWeather.SurvivalMultipliers.HungerRate = 1.0f;
    ClearWeather.SurvivalMultipliers.ThirstRate = 1.0f;
    ClearWeather.SurvivalMultipliers.WarmthLoss = 1.0f;
    ClearWeather.MovementSpeedMultiplier = 1.0f;
    ClearWeather.VisibilityRange = 1000.0f;
    ClearWeather.CropGrowthMultiplier = 1.2f;
    ClearWeather.TradeEfficiencyMultiplier = 1.0f;
    WeatherEffects.Add(EWeatherType::Clear, ClearWeather);
    
    // Rain Weather Effects
    FWeatherImpactData RainWeather;
    RainWeather.SurvivalMultipliers.HungerRate = 1.1f;
    RainWeather.SurvivalMultipliers.ThirstRate = 0.8f; // Rain provides water
    RainWeather.SurvivalMultipliers.WarmthLoss = 1.5f;
    RainWeather.MovementSpeedMultiplier = 0.8f;
    RainWeather.VisibilityRange = 600.0f;
    RainWeather.CropGrowthMultiplier = 1.8f;
    RainWeather.TradeEfficiencyMultiplier = 0.7f;
    WeatherEffects.Add(EWeatherType::Rain, RainWeather);
    
    // Snow Weather Effects
    FWeatherImpactData SnowWeather;
    SnowWeather.SurvivalMultipliers.HungerRate = 1.3f;
    SnowWeather.SurvivalMultipliers.ThirstRate = 0.9f;
    SnowWeather.SurvivalMultipliers.WarmthLoss = 2.5f;
    SnowWeather.MovementSpeedMultiplier = 0.6f;
    SnowWeather.VisibilityRange = 400.0f;
    SnowWeather.CropGrowthMultiplier = 0.3f;
    SnowWeather.TradeEfficiencyMultiplier = 0.4f;
    WeatherEffects.Add(EWeatherType::Snow, SnowWeather);
    
    // Storm Weather Effects
    FWeatherImpactData StormWeather;
    StormWeather.SurvivalMultipliers.HungerRate = 1.2f;
    StormWeather.SurvivalMultipliers.ThirstRate = 0.7f;
    StormWeather.SurvivalMultipliers.WarmthLoss = 2.0f;
    StormWeather.MovementSpeedMultiplier = 0.5f;
    StormWeather.VisibilityRange = 200.0f;
    StormWeather.CropGrowthMultiplier = 0.8f;
    StormWeather.TradeEfficiencyMultiplier = 0.2f;
    WeatherEffects.Add(EWeatherType::Storm, StormWeather);
    
    // Fog Weather Effects
    FWeatherImpactData FogWeather;
    FogWeather.SurvivalMultipliers.HungerRate = 1.0f;
    FogWeather.SurvivalMultipliers.ThirstRate = 0.95f;
    FogWeather.SurvivalMultipliers.WarmthLoss = 1.2f;
    FogWeather.MovementSpeedMultiplier = 0.7f;
    FogWeather.VisibilityRange = 150.0f;
    FogWeather.CropGrowthMultiplier = 1.1f;
    FogWeather.TradeEfficiencyMultiplier = 0.6f;
    WeatherEffects.Add(EWeatherType::Fog, FogWeather);
}

void UAdvancedWeatherImpactSubsystem::UpdateWeatherEffects()
{
    // Simulate dynamic weather changes
    UpdateWeatherConditions();
    
    // Apply effects to survival system
    ApplyWeatherToSurvival();
    
    // Apply effects to NPC ecosystem
    ApplyWeatherToNPCs();
    
    // Apply effects to economy
    ApplyWeatherToEconomy();
    
    // Apply environmental effects
    ApplyEnvironmentalEffects();
    
    UE_LOG(LogTemp, Log, TEXT("Weather effects updated - Type: %d, Intensity: %f"), 
        (int32)CurrentWeatherType, CurrentWeatherIntensity);
}

void UAdvancedWeatherImpactSubsystem::UpdateWeatherConditions()
{
    // Advanced weather simulation with seasonal patterns
    float SeasonalFactor = GetSeasonalWeatherFactor();
    float RandomFactor = FMath::FRand();
    
    // Determine weather type based on seasonal and random factors
    if (SeasonalFactor > 0.8f && RandomFactor > 0.7f)
    {
        CurrentWeatherType = EWeatherType::Storm;
        CurrentWeatherIntensity = FMath::FRandRange(0.7f, 1.0f);
    }
    else if (SeasonalFactor > 0.6f && RandomFactor > 0.5f)
    {
        CurrentWeatherType = EWeatherType::Rain;
        CurrentWeatherIntensity = FMath::FRandRange(0.4f, 0.8f);
    }
    else if (SeasonalFactor < 0.3f && RandomFactor > 0.6f)
    {
        CurrentWeatherType = EWeatherType::Snow;
        CurrentWeatherIntensity = FMath::FRandRange(0.5f, 0.9f);
    }
    else if (RandomFactor < 0.2f)
    {
        CurrentWeatherType = EWeatherType::Fog;
        CurrentWeatherIntensity = FMath::FRandRange(0.3f, 0.7f);
    }
    else
    {
        CurrentWeatherType = EWeatherType::Clear;
        CurrentWeatherIntensity = FMath::FRandRange(0.2f, 0.6f);
    }
    
    // Broadcast weather change event
    OnWeatherChanged.Broadcast(CurrentWeatherType, CurrentWeatherIntensity);
}

void UAdvancedWeatherImpactSubsystem::ApplyWeatherToSurvival()
{
    UWorld* World = GetWorld();
    if (World && World->GetGameInstance())
    {
        if (UAdvancedSurvivalSubsystem* SurvivalSystem = World->GetGameInstance()->GetSubsystem<UAdvancedSurvivalSubsystem>())
        {
            if (FWeatherImpactData* WeatherData = WeatherEffects.Find(CurrentWeatherType))
            {
                // Apply weather multipliers to survival rates
                float IntensityModifier = 0.5f + (CurrentWeatherIntensity * 0.5f);
                
                FSurvivalMultipliers AdjustedMultipliers;
                AdjustedMultipliers.HungerRate = 1.0f + ((WeatherData->SurvivalMultipliers.HungerRate - 1.0f) * IntensityModifier);
                AdjustedMultipliers.ThirstRate = 1.0f + ((WeatherData->SurvivalMultipliers.ThirstRate - 1.0f) * IntensityModifier);
                AdjustedMultipliers.WarmthLoss = 1.0f + ((WeatherData->SurvivalMultipliers.WarmthLoss - 1.0f) * IntensityModifier);
                
                // Apply to survival system (this would need to be implemented in the survival system)
                UE_LOG(LogTemp, Log, TEXT("Applied weather survival effects - Hunger: %f, Thirst: %f, Warmth: %f"),
                    AdjustedMultipliers.HungerRate, AdjustedMultipliers.ThirstRate, AdjustedMultipliers.WarmthLoss);
            }
        }
    }
}

void UAdvancedWeatherImpactSubsystem::ApplyWeatherToNPCs()
{
    UWorld* World = GetWorld();
    if (World && World->GetGameInstance())
    {
        if (UNPCEcosystemSubsystem* NPCSystem = World->GetGameInstance()->GetSubsystem<UNPCEcosystemSubsystem>())
        {
            if (FWeatherImpactData* WeatherData = WeatherEffects.Find(CurrentWeatherType))
            {
                // Modify NPC behavior based on weather
                switch (CurrentWeatherType)
                {
                case EWeatherType::Storm:
                    UE_LOG(LogTemp, Warning, TEXT("Storm weather - NPCs seeking shelter"));
                    // NPCs should prioritize shelter-seeking behavior
                    break;
                    
                case EWeatherType::Rain:
                    UE_LOG(LogTemp, Log, TEXT("Rain weather - NPCs adjusting outdoor activities"));
                    // NPCs reduce outdoor work efficiency
                    break;
                    
                case EWeatherType::Snow:
                    UE_LOG(LogTemp, Log, TEXT("Snow weather - NPCs conserving energy"));
                    // NPCs move slower and consume more food
                    break;
                    
                case EWeatherType::Fog:
                    UE_LOG(LogTemp, Log, TEXT("Fog weather - NPCs staying close to home"));
                    // NPCs reduce travel distance
                    break;
                    
                default:
                    UE_LOG(LogTemp, Log, TEXT("Clear weather - NPCs operating normally"));
                    break;
                }
            }
        }
    }
}

void UAdvancedWeatherImpactSubsystem::ApplyWeatherToEconomy()
{
    UWorld* World = GetWorld();
    if (World && World->GetGameInstance())
    {
        if (UEconomySubsystem* EconomySystem = World->GetGameInstance()->GetSubsystem<UEconomySubsystem>())
        {
            if (FWeatherImpactData* WeatherData = WeatherEffects.Find(CurrentWeatherType))
            {
                float TradeEfficiency = WeatherData->TradeEfficiencyMultiplier * (0.5f + CurrentWeatherIntensity * 0.5f);
                
                // Apply weather effects to trade routes and market prices
                switch (CurrentWeatherType)
                {
                case EWeatherType::Storm:
                    UE_LOG(LogTemp, Warning, TEXT("Storm disrupting trade routes - efficiency: %f"), TradeEfficiency);
                    // Increase prices of imported goods, decrease trade volume
                    break;
                    
                case EWeatherType::Rain:
                    UE_LOG(LogTemp, Log, TEXT("Rain affecting trade efficiency: %f"), TradeEfficiency);
                    // Moderate impact on trade
                    break;
                    
                case EWeatherType::Snow:
                    UE_LOG(LogTemp, Warning, TEXT("Snow blocking trade routes - efficiency: %f"), TradeEfficiency);
                    // Severe impact on overland trade
                    break;
                    
                case EWeatherType::Clear:
                    UE_LOG(LogTemp, Log, TEXT("Clear weather boosting trade efficiency: %f"), TradeEfficiency);
                    // Optimal trading conditions
                    break;
                }
            }
        }
    }
}

void UAdvancedWeatherImpactSubsystem::ApplyEnvironmentalEffects()
{
    if (FWeatherImpactData* WeatherData = WeatherEffects.Find(CurrentWeatherType))
    {
        // Apply crop growth effects
        float CropGrowthRate = WeatherData->CropGrowthMultiplier * CurrentWeatherIntensity;
        
        // Apply visibility effects
        float VisibilityRange = WeatherData->VisibilityRange * (0.5f + CurrentWeatherIntensity * 0.5f);
        
        // Apply movement effects
        float MovementMultiplier = WeatherData->MovementSpeedMultiplier;
        
        UE_LOG(LogTemp, Log, TEXT("Environmental effects - Crop Growth: %f, Visibility: %f, Movement: %f"),
            CropGrowthRate, VisibilityRange, MovementMultiplier);
        
        // Store current environmental modifiers for other systems to use
        CurrentEnvironmentalModifiers.CropGrowthRate = CropGrowthRate;
        CurrentEnvironmentalModifiers.VisibilityRange = VisibilityRange;
        CurrentEnvironmentalModifiers.MovementSpeedMultiplier = MovementMultiplier;
    }
}

void UAdvancedWeatherImpactSubsystem::ApplyContinuousWeatherEffects(float DeltaTime)
{
    // Apply continuous effects like temperature changes, humidity, etc.
    if (CurrentWeatherType == EWeatherType::Snow)
    {
        // Gradually decrease temperature
        CurrentTemperature = FMath::Max(-20.0f, CurrentTemperature - (DeltaTime * 2.0f * CurrentWeatherIntensity));
    }
    else if (CurrentWeatherType == EWeatherType::Clear)
    {
        // Gradually increase temperature during clear weather
        CurrentTemperature = FMath::Min(30.0f, CurrentTemperature + (DeltaTime * 1.0f));
    }
    else if (CurrentWeatherType == EWeatherType::Rain)
    {
        // Rain moderates temperature
        float TargetTemp = 15.0f;
        CurrentTemperature = FMath::Lerp(CurrentTemperature, TargetTemp, DeltaTime * 0.5f);
    }
    
    // Apply seasonal temperature drift
    float SeasonalTemp = GetSeasonalTemperature();
    CurrentTemperature = FMath::Lerp(CurrentTemperature, SeasonalTemp, DeltaTime * 0.1f);
}

float UAdvancedWeatherImpactSubsystem::GetSeasonalWeatherFactor() const
{
    // Simulate seasonal weather patterns
    if (UWorld* World = GetWorld())
    {
        float GameTime = World->GetTimeSeconds();
        float SeasonCycle = FMath::Fmod(GameTime / 3600.0f, 4.0f); // 4 seasons per hour for testing
        
        // Spring: 0-1, Summer: 1-2, Autumn: 2-3, Winter: 3-4
        if (SeasonCycle < 1.0f) // Spring
        {
            return 0.6f; // Moderate rain chance
        }
        else if (SeasonCycle < 2.0f) // Summer
        {
            return 0.3f; // Low precipitation, more clear weather
        }
        else if (SeasonCycle < 3.0f) // Autumn
        {
            return 0.8f; // High rain chance
        }
        else // Winter
        {
            return 0.9f; // High snow/storm chance
        }
    }
    
    return 0.5f; // Default moderate weather
}

float UAdvancedWeatherImpactSubsystem::GetSeasonalTemperature() const
{
    if (UWorld* World = GetWorld())
    {
        float GameTime = World->GetTimeSeconds();
        float SeasonCycle = FMath::Fmod(GameTime / 3600.0f, 4.0f);
        
        if (SeasonCycle < 1.0f) // Spring
        {
            return FMath::Lerp(5.0f, 20.0f, SeasonCycle);
        }
        else if (SeasonCycle < 2.0f) // Summer
        {
            return FMath::Lerp(20.0f, 30.0f, SeasonCycle - 1.0f);
        }
        else if (SeasonCycle < 3.0f) // Autumn
        {
            return FMath::Lerp(30.0f, 10.0f, SeasonCycle - 2.0f);
        }
        else // Winter
        {
            return FMath::Lerp(10.0f, -10.0f, SeasonCycle - 3.0f);
        }
    }
    
    return 15.0f; // Default temperature
}

EWeatherType UAdvancedWeatherImpactSubsystem::GetCurrentWeatherType() const
{
    return CurrentWeatherType;
}

float UAdvancedWeatherImpactSubsystem::GetCurrentWeatherIntensity() const
{
    return CurrentWeatherIntensity;
}

float UAdvancedWeatherImpactSubsystem::GetCurrentTemperature() const
{
    return CurrentTemperature;
}

FEnvironmentalModifiers UAdvancedWeatherImpactSubsystem::GetCurrentEnvironmentalModifiers() const
{
    return CurrentEnvironmentalModifiers;
}

void UAdvancedWeatherImpactSubsystem::ForceWeatherChange(EWeatherType NewWeatherType, float Intensity)
{
    CurrentWeatherType = NewWeatherType;
    CurrentWeatherIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
    
    UE_LOG(LogTemp, Warning, TEXT("Weather manually changed to type %d with intensity %f"), 
        (int32)NewWeatherType, CurrentWeatherIntensity);
    
    // Immediately apply new weather effects
    UpdateWeatherEffects();
}

bool UAdvancedWeatherImpactSubsystem::IsWeatherSevere() const
{
    return (CurrentWeatherType == EWeatherType::Storm || CurrentWeatherType == EWeatherType::Snow) 
        && CurrentWeatherIntensity > 0.7f;
}

float UAdvancedWeatherImpactSubsystem::GetWeatherSeverityLevel() const
{
    float BaseSeverity = 0.0f;
    
    switch (CurrentWeatherType)
    {
    case EWeatherType::Clear:
        BaseSeverity = 0.1f;
        break;
    case EWeatherType::Rain:
        BaseSeverity = 0.4f;
        break;
    case EWeatherType::Fog:
        BaseSeverity = 0.3f;
        break;
    case EWeatherType::Snow:
        BaseSeverity = 0.7f;
        break;
    case EWeatherType::Storm:
        BaseSeverity = 0.9f;
        break;
    }
    
    return BaseSeverity * CurrentWeatherIntensity;
}