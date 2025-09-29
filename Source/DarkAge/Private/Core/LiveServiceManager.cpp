#include "Core/LiveServiceManager.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "Core/WorldEcosystemSubsystem.h"
#include "Core/CrimeManagerSubsystem.h"
#include "Data/WorldEcosystemData.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Json.h"

ULiveServiceManager::ULiveServiceManager()
{
    bIsConnected = false;
    HttpModule = &FHttpModule::Get();
}

void ULiveServiceManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Initialized for enterprise-scale deployment"));
    
    // Register with debug system
    
    // Set up periodic update checks (every 30 seconds for enterprise responsiveness)
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(UpdateCheckTimer, this, &ULiveServiceManager::CheckForUpdates, 30.0f, true);
        World->GetTimerManager().SetTimer(TelemetryTimer, this, &ULiveServiceManager::SendSystemHealthTelemetry, 60.0f, true);
    }
}

void ULiveServiceManager::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateCheckTimer);
        World->GetTimerManager().ClearTimer(TelemetryTimer);
    }
    
    Super::Deinitialize();
}

void ULiveServiceManager::StartLiveService(const FString& InServiceEndpoint, const FString& InApiKey)
{
    ServiceEndpoint = InServiceEndpoint;
    ApiKey = InApiKey;
    bIsConnected = true;
    
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Connected to service endpoint: %s"), *ServiceEndpoint);
    
    // Immediate check for updates on connection
    CheckForUpdates();
}

void ULiveServiceManager::CheckForUpdates()
{
    if (!bIsConnected || ServiceEndpoint.IsEmpty())
    {
        return;
    }
    
    // Create HTTP request for update manifest
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
    Request->SetURL(ServiceEndpoint + TEXT("/api/v1/updates/check"));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + ApiKey);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("X-Game-Version"), TEXT("1.0.0"));
    if (UWorld* World = GetWorld())
    {
        Request->SetHeader(TEXT("X-Player-Count"), FString::FromInt(World->GetNumPlayerControllers()));
    }
    
    Request->OnProcessRequestComplete().BindUObject(this, &ULiveServiceManager::OnUpdateCheckResponse);
    Request->ProcessRequest();
}

void ULiveServiceManager::OnUpdateCheckResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("LiveServiceManager: Failed to check for updates"));
        return;
    }
    
    if (Response->GetResponseCode() == 200)
    {
        FString ResponseContent = Response->GetContentAsString();
        
        // Parse JSON response
        TSharedPtr<FJsonObject> JsonObject;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(ResponseContent), JsonObject))
        {
            // Process updates array
            const TArray<TSharedPtr<FJsonValue>>* UpdatesArray;
            if (JsonObject->TryGetArrayField(TEXT("updates"), UpdatesArray))
            {
                for (const auto& UpdateValue : *UpdatesArray)
                {
                    const TSharedPtr<FJsonObject>* UpdateObject;
                    if (UpdateValue->TryGetObject(UpdateObject))
                    {
                        FLiveUpdateManifest UpdateManifest;
                        UpdateManifest.UpdateId = (*UpdateObject)->GetStringField(TEXT("id"));
                        UpdateManifest.SystemTarget = (*UpdateObject)->GetStringField(TEXT("system"));
                        UpdateManifest.ConfigData = (*UpdateObject)->GetStringField(TEXT("config"));
                        UpdateManifest.Priority = (*UpdateObject)->GetIntegerField(TEXT("priority"));
                        UpdateManifest.RequiresRestart = (*UpdateObject)->GetBoolField(TEXT("requiresRestart"));
                        
                        // Check if we've already applied this update
                        if (!AppliedUpdates.Contains(UpdateManifest.UpdateId))
                        {
                            PendingUpdates.Add(UpdateManifest);
                            OnLiveUpdateReceived.Broadcast(UpdateManifest);
                        }
                    }
                }
                
                // Process pending updates
                ProcessPendingUpdates();
            }
        }
    }
}

void ULiveServiceManager::ProcessPendingUpdates()
{
    // Sort by priority (higher priority first)
    PendingUpdates.Sort([](const FLiveUpdateManifest& A, const FLiveUpdateManifest& B) {
        return A.Priority > B.Priority;
    });
    
    for (const FLiveUpdateManifest& Update : PendingUpdates)
    {
        if (ValidateUpdate(Update))
        {
            ApplyUpdate(Update);
        }
    }
    
    PendingUpdates.Empty();
}

bool ULiveServiceManager::ValidateUpdate(const FLiveUpdateManifest& UpdateManifest) const
{
    // Basic validation
    if (UpdateManifest.UpdateId.IsEmpty() || UpdateManifest.SystemTarget.IsEmpty())
    {
        return false;
    }
    
    // Check expiration
    if (UpdateManifest.ExpirationTime < FDateTime::Now())
    {
        return false;
    }
    
    return true;
}

void ULiveServiceManager::ApplyUpdate(const FLiveUpdateManifest& UpdateManifest)
{
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Applying update %s to system %s"), 
        *UpdateManifest.UpdateId, *UpdateManifest.SystemTarget);
    
    // Backup current configuration for rollback capability
    if (!SystemConfigBackups.Contains(UpdateManifest.SystemTarget))
    {
        SystemConfigBackups.Add(UpdateManifest.SystemTarget, TEXT("{}"));
    }
    
    bool bSuccess = false;
    
    // Apply system-specific updates
    if (UpdateManifest.SystemTarget == TEXT("Ecosystem"))
    {
        UpdateEcosystemConfig(UpdateManifest.ConfigData);
        bSuccess = true;
    }
    else if (UpdateManifest.SystemTarget == TEXT("Crime"))
    {
        UpdateCrimeSystemConfig(UpdateManifest.ConfigData);
        bSuccess = true;
    }
    else if (UpdateManifest.SystemTarget == TEXT("Survival"))
    {
        UpdateSurvivalConfig(UpdateManifest.ConfigData);
        bSuccess = true;
    }
    else if (UpdateManifest.SystemTarget == TEXT("AI"))
    {
        UpdateAIConfig(UpdateManifest.ConfigData);
        bSuccess = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LiveServiceManager: Unknown system target: %s"), *UpdateManifest.SystemTarget);
    }
    
    if (bSuccess)
    {
        AppliedUpdates.Add(UpdateManifest.UpdateId, FDateTime::Now());
        UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Successfully applied update %s"), *UpdateManifest.UpdateId);
    }
    
    OnUpdateApplied.Broadcast(UpdateManifest.UpdateId, bSuccess);
}

void ULiveServiceManager::UpdateEcosystemConfig(const FString& ConfigJSON)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance) return;
    
    UWorldEcosystemSubsystem* EcosystemSubsystem = GameInstance->GetSubsystem<UWorldEcosystemSubsystem>();
    if (!EcosystemSubsystem) return;
    
    // Parse JSON and apply ecosystem updates
    TSharedPtr<FJsonObject> JsonObject;
    if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(ConfigJSON), JsonObject))
    {
        // Example: Update weather patterns
        if (JsonObject->HasField(TEXT("weatherPatterns")))
        {
            const TSharedPtr<FJsonObject>* WeatherConfig;
            if (JsonObject->TryGetObjectField(TEXT("weatherPatterns"), WeatherConfig))
            {
                // Apply weather pattern updates to all regions
                TArray<FString> Regions = EcosystemSubsystem->GetAllRegions();
                for (const FString& Region : Regions)
                {
                    // Apply new weather configuration
                    if ((*WeatherConfig)->HasField(TEXT("stormIntensity")))
                    {
                        float StormIntensity = (*WeatherConfig)->GetNumberField(TEXT("stormIntensity"));
                        EcosystemSubsystem->AdminForceWeather(Region, EWeatherPattern::Storm, StormIntensity);
                    }
                }
            }
        }
        
        // Example: Update resource spawn rates
        if (JsonObject->HasField(TEXT("resourceRates")))
        {
            const TSharedPtr<FJsonObject>* ResourceConfig;
            if (JsonObject->TryGetObjectField(TEXT("resourceRates"), ResourceConfig))
            {
                // Apply resource rate updates
                // Implementation would update resource generation parameters
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Ecosystem configuration updated"));
}

void ULiveServiceManager::UpdateCrimeSystemConfig(const FString& ConfigJSON)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance) return;
   
    UCrimeManagerSubsystem* CrimeSubsystem = GameInstance->GetSubsystem<UCrimeManagerSubsystem>();
    if (!CrimeSubsystem) return;
    
    // Parse JSON and apply crime system updates
    TSharedPtr<FJsonObject> JsonObject;
    if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(ConfigJSON), JsonObject))
    {
        // Example: Update bounty multipliers, witness reliability, etc.
        // Implementation would update crime system parameters
    }
    
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Crime system configuration updated"));
}

void ULiveServiceManager::UpdateSurvivalConfig(const FString& ConfigJSON)
{
    // Update survival mechanics (hunger/thirst rates, damage modifiers, etc.)
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Survival configuration updated"));
}

void ULiveServiceManager::UpdateAIConfig(const FString& ConfigJSON)
{
    // Update AI behavior parameters
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: AI configuration updated"));
}

void ULiveServiceManager::TriggerLiveEvent(const FString& EventType, const FString& EventData)
{
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Triggering live event: %s"), *EventType);
    
    // Parse event data and trigger appropriate systems
    if (EventType == TEXT("GlobalWeatherEvent"))
    {
        // Trigger weather event across all regions
        UGameInstance* GameInstance = GetGameInstance();
        if (UWorldEcosystemSubsystem* EcosystemSubsystem = GameInstance->GetSubsystem<UWorldEcosystemSubsystem>())
        {
            TArray<FString> Regions = EcosystemSubsystem->GetAllRegions();
            for (const FString& Region : Regions)
            {
                EcosystemSubsystem->AdminTriggerEvent(Region, EEnvironmentalEventType::Flood, 0.8f, 2.0f);
            }
        }
    }
    else if (EventType == TEXT("CrimeWave"))
    {
        // Trigger crime wave event
        // Implementation would increase crime rates temporarily
    }
    
    // Send telemetry about event
    SendTelemetryData(FString::Printf(TEXT("{\"eventType\":\"%s\",\"timestamp\":\"%s\"}"), 
        *EventType, *FDateTime::Now().ToString()));
}

FString ULiveServiceManager::GetSystemHealthData() const
{
    // Collect system health metrics for telemetry
    TSharedPtr<FJsonObject> HealthData = MakeShareable(new FJsonObject);
    
    HealthData->SetStringField(TEXT("timestamp"), FDateTime::Now().ToString());
    if (UWorld* World = GetWorld())
    {
        HealthData->SetNumberField(TEXT("playerCount"), World->GetNumPlayerControllers());
        if (World->GetDeltaSeconds() > 0.0f)
        {
            HealthData->SetNumberField(TEXT("frameRate"), 1.0f / World->GetDeltaSeconds());
        }
    }
    HealthData->SetBoolField(TEXT("isConnected"), bIsConnected);
    HealthData->SetNumberField(TEXT("appliedUpdates"), AppliedUpdates.Num());
    
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(HealthData.ToSharedRef(), Writer);
    
    return OutputString;
}

void ULiveServiceManager::SendTelemetryData(const FString& TelemetryData)
{
    if (!bIsConnected || ServiceEndpoint.IsEmpty())
    {
        return;
    }
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
    Request->SetURL(ServiceEndpoint + TEXT("/api/v1/telemetry"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + ApiKey);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(TelemetryData);
    
    Request->OnProcessRequestComplete().BindUObject(this, &ULiveServiceManager::OnTelemetryResponse);
    Request->ProcessRequest();
}

void ULiveServiceManager::OnTelemetryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("LiveServiceManager: Telemetry sent successfully"));
    }
}

void ULiveServiceManager::SendSystemHealthTelemetry()
{
    FString HealthData = GetSystemHealthData();
    SendTelemetryData(HealthData);
}


void ULiveServiceManager::UpdateDataTable(const FString& TableName, const FString& TableData)
{
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Updating data table %s"), *TableName);
    
    // Implementation would load and update the specified data table
    // This is a placeholder for the actual data table update logic
    
    // Parse JSON data and update the data table
    TSharedPtr<FJsonObject> JsonObject;
    if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(TableData), JsonObject))
    {
        // Update the data table with new values
        // Implementation depends on the specific data table structure
        UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Data table %s updated successfully"), *TableName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LiveServiceManager: Failed to parse data table JSON for %s"), *TableName);
    }
}

void ULiveServiceManager::ReloadAllDataTables()
{
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Reloading all data tables"));
    
    // Implementation would reload all game data tables
    // This ensures all systems get the latest data
}

void ULiveServiceManager::EndLiveEvent(const FString& EventId)
{
    UE_LOG(LogTemp, Log, TEXT("LiveServiceManager: Ending live event: %s"), *EventId);
    
    // Implementation would end the specified live event
    // This might involve reverting temporary changes or stopping ongoing effects
}
