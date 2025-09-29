// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/WorldPersistenceSystem.h"
#include "Core/TimeSystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/EconomySystem.h"
#include "Survival/DiseaseSystem.h"
#include "Core/WeatherSystem.h"
#include "AI/NPCRoutineSystem.h"
#include "Core/GlobalEventBus.h"
#include "Core/WorldStateLock.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"

UWorldPersistenceSystem::UWorldPersistenceSystem()
    : WorldStateLock(nullptr)
    , GlobalEventBus(nullptr)
    , TimeSinceLastUpdate(0.0f)
    , TimeSinceLastSave(0.0f)
    , CurrentSaveLockID()
{
    // Initialize default regions
    FRegionState Heartlands;
    Heartlands.RegionID = "Heartlands";
    RegionStates.Add(Heartlands.RegionID, Heartlands);

    FRegionState NorthernWastes;
    NorthernWastes.RegionID = "NorthernWastes";
    RegionStates.Add(NorthernWastes.RegionID, NorthernWastes);

    FRegionState EasternMarshlands;
    EasternMarshlands.RegionID = "EasternMarshlands";
    RegionStates.Add(EasternMarshlands.RegionID, EasternMarshlands);

    FRegionState SouthernDeserts;
    SouthernDeserts.RegionID = "SouthernDeserts";
    RegionStates.Add(SouthernDeserts.RegionID, SouthernDeserts);

    FRegionState WesternCoastal;
    WesternCoastal.RegionID = "WesternCoastal";
    RegionStates.Add(WesternCoastal.RegionID, WesternCoastal);
}

void UWorldPersistenceSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Get references to other subsystems
    TimeSystem = GetGameInstance()->GetSubsystem<UTimeSystem>();
    WorldStateLock = GetGameInstance()->GetSubsystem<UWorldStateLock>();
    GlobalEventBus = GetGameInstance()->GetSubsystem<UGlobalEventBus>();
    
    // Register with Global Event Bus
    if (GlobalEventBus)
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        TArray<EGlobalEventType> ListenedEvents = {
            EGlobalEventType::WorldSaveRequested,
            EGlobalEventType::WorldLoadRequested,
            EGlobalEventType::WorldStateChanged
        };
        GlobalEventBus->RegisterListener(ListenerInterface, ListenedEvents);
        
        UE_LOG(LogTemp, Log, TEXT("WorldPersistenceSystem: Registered with Global Event Bus"));
    }
    
    // Check for a reset flag to force-delete corrupted save file
    const FString ResetFlagPath = FPaths::ProjectSavedDir() + TEXT("ResetWorldState.flag");
    if (IFileManager::Get().FileExists(*ResetFlagPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("ResetWorldState.flag found. Deleting WorldState.json and re-initializing."));
        IFileManager::Get().Delete(*GetWorldStateSavePath());
        IFileManager::Get().Delete(*ResetFlagPath);
    }

    // Load world state from disk
    LoadWorldState();
    
    UE_LOG(LogTemp, Log, TEXT("World Persistence System initialized with %d regions"), RegionStates.Num());
}

void UWorldPersistenceSystem::Deinitialize()
{
    // Release any active locks
    if (WorldStateLock && !CurrentSaveLockID.IsValid() == false)
    {
        WorldStateLock->ReleaseLock(CurrentSaveLockID);
    }
    
    // Unregister from Global Event Bus
    if (GlobalEventBus)
    {
        TScriptInterface<IGlobalEventListener> ListenerInterface;
        ListenerInterface.SetObject(this);
        ListenerInterface.SetInterface(Cast<IGlobalEventListener>(this));
        GlobalEventBus->UnregisterListener(ListenerInterface);
    }
    
    // Save world state before shutting down
    SaveWorldState();
    
    Super::Deinitialize();
}

void UWorldPersistenceSystem::UpdateWorldPersistence(float DeltaTime)
{
    // Update timers
    TimeSinceLastUpdate += DeltaTime;
    TimeSinceLastSave += DeltaTime;
    
    // Process player actions every 5 seconds
    if (TimeSinceLastUpdate >= 5.0f)
    {
        ProcessPlayerActions();
        
        // Update each region
        for (auto& RegionPair : RegionStates)
        {
            UpdateRegion(RegionPair.Key, TimeSinceLastUpdate);
        }
        
        // Simulate NPC actions
        SimulateNPCActions(TimeSinceLastUpdate);
        
        // Simulate faction dynamics
        SimulateFactionDynamics(TimeSinceLastUpdate);
        
        // Update resource levels
        UpdateResourceLevels(TimeSinceLastUpdate);
        
        // Reset timer
        TimeSinceLastUpdate = 0.0f;
    }
    
    // Auto-save every 5 minutes (only if safe to save)
    if (TimeSinceLastSave >= 300.0f)
    {
        if (WorldStateLock && WorldStateLock->IsSafeToSave())
        {
            SaveWorldState();
            TimeSinceLastSave = 0.0f;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("WorldPersistenceSystem: Auto-save delayed due to active locks"));
        }
    }
}

void UWorldPersistenceSystem::RecordPlayerAction(EWorldActionType ActionType, FVector Location, FName RegionID, 
    float Magnitude, FName TargetID, const FString& AdditionalData)
{
    // Create a new action record
    FWorldAction NewAction;
    NewAction.ActionType = ActionType;
    NewAction.Location = Location;
    NewAction.RegionID = RegionID;
    NewAction.Magnitude = FMath::Clamp(Magnitude, 0.0f, 1.0f);
    NewAction.TargetID = TargetID;
    NewAction.AdditionalData = AdditionalData;
    
    // Set timestamp if we have a time system
    if (TimeSystem)
    {
        NewAction.Timestamp = TimeSystem->GetCurrentDateTime();
    }
    
    // Add to action list
    PlayerActions.Add(NewAction);
    
    // Apply immediate effects for high-magnitude actions
    if (Magnitude > 0.7f)
    {
        ApplyActionEffects(NewAction);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Player action recorded: Type=%s, Region=%s, Magnitude=%.2f"), 
        *UEnum::GetValueAsString(ActionType), *RegionID.ToString(), Magnitude);
}

FRegionState UWorldPersistenceSystem::GetRegionState(FName RegionID) const
{
    if (RegionStates.Contains(RegionID))
    {
        return RegionStates[RegionID];
    }
    
    // Return default state if region not found
    return FRegionState();
}

void UWorldPersistenceSystem::SimulateWorldEvolution(int32 GameMinutes)
{
    // Skip if no time system
    if (!TimeSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot simulate world evolution: TimeSystem not found"));
        return;
    }
    
    // Calculate how many update cycles this represents
    // Assuming we update every 5 seconds of real time, which might represent longer in game time
    const float GameTimePerUpdate = 15.0f; // 15 minutes of game time per update
    const int32 UpdateCycles = FMath::CeilToInt(GameMinutes / GameTimePerUpdate);
    
    UE_LOG(LogTemp, Log, TEXT("Simulating world evolution for %d game minutes (%d update cycles)"), 
        GameMinutes, UpdateCycles);
    
    // Run multiple update cycles
    for (int32 i = 0; i < UpdateCycles; ++i)
    {
        // Process player actions
        ProcessPlayerActions();
        
        // Update each region
        for (auto& RegionPair : RegionStates)
        {
            UpdateRegion(RegionPair.Key, 5.0f);
        }
        
        // Simulate NPC actions
        SimulateNPCActions(5.0f);
        
        // Simulate faction dynamics
        SimulateFactionDynamics(5.0f);
        
        // Update resource levels
        UpdateResourceLevels(5.0f);
        
        // Generate random events (more likely during longer simulations)
        for (auto& RegionPair : RegionStates)
        {
            if (FMath::RandBool())
            {
                GenerateRandomEvent(RegionPair.Key);
            }
        }
    }
    
    // Save state after simulation
    SaveWorldState();
}

TArray<FString> UWorldPersistenceSystem::GetRecentRegionalEvents(FName RegionID, int32 Count) const
{
    TArray<FString> Events;
    
    if (RegionStates.Contains(RegionID))
    {
        const FRegionState& Region = RegionStates[RegionID];
        
        // Copy the most recent events up to the requested count
        const int32 NumEvents = FMath::Min(Region.CurrentEvents.Num(), Count);
        for (int32 i = 0; i < NumEvents; ++i)
        {
            Events.Add(Region.CurrentEvents[Region.CurrentEvents.Num() - 1 - i].ToString());
        }
    }
    
    return Events;
}

float UWorldPersistenceSystem::GetPlayerRegionalImpact(FName RegionID) const
{
    float TotalImpact = 0.0f;
    int32 ActionCount = 0;
    
    // Calculate impact based on player actions in this region
    for (const FWorldAction& Action : PlayerActions)
    {
        if (Action.RegionID == RegionID)
        {
            TotalImpact += Action.Magnitude;
            ActionCount++;
        }
    }
    
    // Return average impact, or 0 if no actions
    return (ActionCount > 0) ? (TotalImpact / ActionCount) : 0.0f;
}

bool UWorldPersistenceSystem::SaveWorldState()
{
    // Request a save lock to prevent conflicts
    if (WorldStateLock)
    {
        if (!WorldStateLock->IsSafeToSave())
        {
            UE_LOG(LogTemp, Warning, TEXT("WorldPersistenceSystem: Cannot save - conflicting operations active"));
            return false;
        }
        
        CurrentSaveLockID = WorldStateLock->RequestSaveLock(TEXT("WorldPersistenceSystem"));
        if (!CurrentSaveLockID.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("WorldPersistenceSystem: Failed to acquire save lock"));
            return false;
        }
    }
    
    // Broadcast save started event
    if (GlobalEventBus)
    {
        GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::WorldSaveRequested,
            TEXT("WorldPersistenceSystem"), TEXT(""), TMap<FString, FString>(),
            EEventPriority::High, true);
    }
    // Create a JSON object to store all data
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    
    // Save region states
    TArray<TSharedPtr<FJsonValue>> RegionsArray;
    for (const auto& RegionPair : RegionStates)
    {
        TSharedPtr<FJsonObject> RegionObject = MakeShared<FJsonObject>();
        
        RegionObject->SetStringField(TEXT("RegionID"), RegionPair.Value.RegionID.ToString());
        RegionObject->SetBoolField(TEXT("bIsDiscovered"), RegionPair.Value.bIsDiscovered);
        RegionObject->SetNumberField(TEXT("PlayerReputationLevel"), RegionPair.Value.PlayerReputationLevel);
        RegionObject->SetStringField(TEXT("CurrentWeather"), RegionPair.Value.CurrentWeather);
        RegionObject->SetNumberField(TEXT("LastVisitTime"), RegionPair.Value.LastVisitTime);
        RegionObject->SetNumberField(TEXT("ActiveBountyValue"), RegionPair.Value.ActiveBountyValue);
        RegionObject->SetNumberField(TEXT("CrimeHeatLevel"), RegionPair.Value.CrimeHeatLevel);
        RegionObject->SetNumberField(TEXT("SupplyChainDisruption"), RegionPair.Value.SupplyChainDisruption);

        // Save current events
        TArray<TSharedPtr<FJsonValue>> EventsArray;
        for (const FName& Event : RegionPair.Value.CurrentEvents)
        {
            EventsArray.Add(MakeShared<FJsonValueString>(Event.ToString()));
        }
        RegionObject->SetArrayField(TEXT("CurrentEvents"), EventsArray);
        
        RegionsArray.Add(MakeShared<FJsonValueObject>(RegionObject));
    }
    RootObject->SetArrayField(TEXT("Regions"), RegionsArray);
    
    // Save player actions
    TArray<TSharedPtr<FJsonValue>> ActionsArray;
    for (const FWorldAction& Action : PlayerActions)
    {
        TSharedPtr<FJsonObject> ActionObject = MakeShared<FJsonObject>();
        
        ActionObject->SetNumberField(TEXT("ActionType"), static_cast<int32>(Action.ActionType));
        
        // Save location
        TSharedPtr<FJsonObject> LocationObject = MakeShared<FJsonObject>();
        LocationObject->SetNumberField(TEXT("X"), Action.Location.X);
        LocationObject->SetNumberField(TEXT("Y"), Action.Location.Y);
        LocationObject->SetNumberField(TEXT("Z"), Action.Location.Z);
        ActionObject->SetObjectField(TEXT("Location"), LocationObject);
        
        ActionObject->SetStringField(TEXT("RegionID"), Action.RegionID.ToString());
        
        // Save timestamp
        TSharedPtr<FJsonObject> TimestampObject = MakeShared<FJsonObject>();
        TimestampObject->SetNumberField(TEXT("Year"), Action.Timestamp.Year);
        TimestampObject->SetNumberField(TEXT("Month"), Action.Timestamp.Month);
        TimestampObject->SetNumberField(TEXT("Day"), Action.Timestamp.Day);
        TimestampObject->SetNumberField(TEXT("Hour"), Action.Timestamp.Hour);
        TimestampObject->SetNumberField(TEXT("Minute"), Action.Timestamp.Minute);
        ActionObject->SetObjectField(TEXT("Timestamp"), TimestampObject);
        
        ActionObject->SetNumberField(TEXT("Magnitude"), Action.Magnitude);
        ActionObject->SetStringField(TEXT("TargetID"), Action.TargetID.ToString());
        ActionObject->SetStringField(TEXT("AdditionalData"), Action.AdditionalData);
        ActionObject->SetBoolField(TEXT("Processed"), Action.bProcessed);
        ActionObject->SetStringField(TEXT("ActionID"), Action.ActionID.ToString());
        
        ActionsArray.Add(MakeShared<FJsonValueObject>(ActionObject));
    }
    RootObject->SetArrayField(TEXT("PlayerActions"), ActionsArray);
    
    // Convert JSON object to string
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    // Save to file
    FString SavePath = GetWorldStateSavePath();
    bool bSuccess = FFileHelper::SaveStringToFile(JsonString, *SavePath);
    
    // Release the save lock
    if (WorldStateLock && CurrentSaveLockID.IsValid())
    {
        WorldStateLock->ReleaseLock(CurrentSaveLockID);
        CurrentSaveLockID = FGuid();
    }
    
    // Broadcast save completed event
    if (GlobalEventBus)
    {
        TMap<FString, FString> EventData;
        EventData.Add(TEXT("Success"), bSuccess ? TEXT("true") : TEXT("false"));
        EventData.Add(TEXT("SavePath"), SavePath);
        
        GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::WorldSaveCompleted,
            TEXT("WorldPersistenceSystem"), TEXT(""), EventData,
            EEventPriority::High, true);
    }
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("World state saved successfully to %s"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save world state to %s"), *SavePath);
    }
    
    return bSuccess;
}

bool UWorldPersistenceSystem::LoadWorldState()
{
    // Request a load lock to prevent conflicts
    FGuid LoadLockID;
    if (WorldStateLock)
    {
        LoadLockID = WorldStateLock->RequestLoadLock(TEXT("WorldPersistenceSystem"));
        if (!LoadLockID.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("WorldPersistenceSystem: Failed to acquire load lock"));
            return false;
        }
    }
    
    // Broadcast load started event
    if (GlobalEventBus)
    {
        GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::WorldLoadRequested,
            TEXT("WorldPersistenceSystem"), TEXT(""), TMap<FString, FString>(),
            EEventPriority::Critical, true);
    }
    FString SavePath = GetWorldStateSavePath();
    
    // Check if file exists
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("World state file not found at %s, using defaults"), *SavePath);
        return false;
    }
    
    // Load file content
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SavePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load world state from %s"), *SavePath);
        return false;
    }
    
    // Parse JSON
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse world state JSON"));
        return false;
    }
    
    // Clear existing data
    RegionStates.Empty();
    PlayerActions.Empty();
    
    // Load region states
    const TArray<TSharedPtr<FJsonValue>>* RegionsArray;
    if (RootObject->TryGetArrayField(TEXT("Regions"), RegionsArray))
    {
        for (const TSharedPtr<FJsonValue>& RegionValue : *RegionsArray)
        {
            const TSharedPtr<FJsonObject>* RegionObject;
            if (RegionValue->TryGetObject(RegionObject))
            {
                FRegionState Region;
                
                FString RegionIDStr;
                if ((*RegionObject)->TryGetStringField(TEXT("RegionID"), RegionIDStr))
                {
                    Region.RegionID = FName(*RegionIDStr);
                }

                (*RegionObject)->TryGetBoolField(TEXT("bIsDiscovered"), Region.bIsDiscovered);
                (*RegionObject)->TryGetNumberField(TEXT("PlayerReputationLevel"), Region.PlayerReputationLevel);
                (*RegionObject)->TryGetStringField(TEXT("CurrentWeather"), Region.CurrentWeather);
                (*RegionObject)->TryGetNumberField(TEXT("LastVisitTime"), Region.LastVisitTime);
                (*RegionObject)->TryGetNumberField(TEXT("ActiveBountyValue"), Region.ActiveBountyValue);
                (*RegionObject)->TryGetNumberField(TEXT("CrimeHeatLevel"), Region.CrimeHeatLevel);
                (*RegionObject)->TryGetNumberField(TEXT("SupplyChainDisruption"), Region.SupplyChainDisruption);

                // Load current events
                const TArray<TSharedPtr<FJsonValue>>* EventsArray;
                if ((*RegionObject)->TryGetArrayField(TEXT("CurrentEvents"), EventsArray))
                {
                    for (const TSharedPtr<FJsonValue>& EventValue : *EventsArray)
                    {
                        FString EventStr;
                        if (EventValue->TryGetString(EventStr))
                        {
                            Region.CurrentEvents.Add(FName(*EventStr));
                        }
                    }
                }
                
                RegionStates.Add(Region.RegionID, Region);
            }
        }
    }
    
    // Load player actions
    const TArray<TSharedPtr<FJsonValue>>* ActionsArray;
    if (RootObject->TryGetArrayField(TEXT("PlayerActions"), ActionsArray))
    {
        for (const TSharedPtr<FJsonValue>& ActionValue : *ActionsArray)
        {
            const TSharedPtr<FJsonObject>* ActionObject;
            if (ActionValue->TryGetObject(ActionObject))
            {
                FWorldAction Action;
                
                int32 ActionTypeInt;
                if ((*ActionObject)->TryGetNumberField(TEXT("ActionType"), ActionTypeInt))
                {
                    Action.ActionType = static_cast<EWorldActionType>(ActionTypeInt);
                }
                
                // Load location
                const TSharedPtr<FJsonObject>* LocationObject;
                if ((*ActionObject)->TryGetObjectField(TEXT("Location"), LocationObject))
                {
                    double X, Y, Z;
                    if ((*LocationObject)->TryGetNumberField(TEXT("X"), X) &&
                        (*LocationObject)->TryGetNumberField(TEXT("Y"), Y) &&
                        (*LocationObject)->TryGetNumberField(TEXT("Z"), Z))
                    {
                        Action.Location = FVector(X, Y, Z);
                    }
                }
                
                FString RegionIDStr;
                if ((*ActionObject)->TryGetStringField(TEXT("RegionID"), RegionIDStr))
                {
                    Action.RegionID = FName(*RegionIDStr);
                }
                
                // Load timestamp
                const TSharedPtr<FJsonObject>* TimestampObject;
                if ((*ActionObject)->TryGetObjectField(TEXT("Timestamp"), TimestampObject))
                {
                    int32 Year, Month, Day, Hour, Minute;
                    if ((*TimestampObject)->TryGetNumberField(TEXT("Year"), Year) &&
                        (*TimestampObject)->TryGetNumberField(TEXT("Month"), Month) &&
                        (*TimestampObject)->TryGetNumberField(TEXT("Day"), Day) &&
                        (*TimestampObject)->TryGetNumberField(TEXT("Hour"), Hour) &&
                        (*TimestampObject)->TryGetNumberField(TEXT("Minute"), Minute))
                    {
                        Action.Timestamp = FGameDateTime(Year, Month, Day, Hour, Minute);
                    }
                }
                
                (*ActionObject)->TryGetNumberField(TEXT("Magnitude"), Action.Magnitude);
                
                FString TargetIDStr;
                if ((*ActionObject)->TryGetStringField(TEXT("TargetID"), TargetIDStr))
                {
                    Action.TargetID = FName(*TargetIDStr);
                }
                
                (*ActionObject)->TryGetStringField(TEXT("AdditionalData"), Action.AdditionalData);
                (*ActionObject)->TryGetBoolField(TEXT("Processed"), Action.bProcessed);
                
                FString ActionIDStr;
                if ((*ActionObject)->TryGetStringField(TEXT("ActionID"), ActionIDStr))
                {
                    FGuid::Parse(ActionIDStr, Action.ActionID);
                }
                
                PlayerActions.Add(Action);
            }
        }
    }
    
    // Release the load lock
    if (WorldStateLock && LoadLockID.IsValid())
    {
        WorldStateLock->ReleaseLock(LoadLockID);
    }
    
    // Broadcast load completed event
    if (GlobalEventBus)
    {
        TMap<FString, FString> EventData;
        EventData.Add(TEXT("Success"), TEXT("true"));
        EventData.Add(TEXT("SavePath"), SavePath);
        EventData.Add(TEXT("RegionCount"), FString::FromInt(RegionStates.Num()));
        EventData.Add(TEXT("ActionCount"), FString::FromInt(PlayerActions.Num()));
        
        GlobalEventBus->BroadcastSimpleEvent(EGlobalEventType::WorldLoadCompleted,
            TEXT("WorldPersistenceSystem"), TEXT(""), EventData,
            EEventPriority::Critical, true);
    }
    
    UE_LOG(LogTemp, Log, TEXT("World state loaded successfully from %s: %d regions, %d player actions"),
        *SavePath, RegionStates.Num(), PlayerActions.Num());
    
    return true;
}

void UWorldPersistenceSystem::ProcessPlayerActions()
{
    // Process unprocessed actions
    for (FWorldAction& Action : PlayerActions)
    {
        if (!Action.bProcessed)
        {
            ApplyActionEffects(Action);
            Action.bProcessed = true;
        }
    }
    
    // Limit the number of stored actions to prevent memory bloat
    const int32 MaxStoredActions = 1000;
    if (PlayerActions.Num() > MaxStoredActions)
    {
        // Sort by timestamp (oldest first)
        PlayerActions.Sort([](const FWorldAction& A, const FWorldAction& B) {
            // Compare years
            if (A.Timestamp.Year != B.Timestamp.Year)
                return A.Timestamp.Year < B.Timestamp.Year;
            
            // Compare months
            if (A.Timestamp.Month != B.Timestamp.Month)
                return A.Timestamp.Month < B.Timestamp.Month;
            
            // Compare days
            if (A.Timestamp.Day != B.Timestamp.Day)
                return A.Timestamp.Day < B.Timestamp.Day;
            
            // Compare hours
            if (A.Timestamp.Hour != B.Timestamp.Hour)
                return A.Timestamp.Hour < B.Timestamp.Hour;
            
            // Compare minutes
            return A.Timestamp.Minute < B.Timestamp.Minute;
        });
        
        // Remove oldest actions
        const int32 NumToRemove = PlayerActions.Num() - MaxStoredActions;
        PlayerActions.RemoveAt(0, NumToRemove);
    }
}

void UWorldPersistenceSystem::UpdateRegion(FName RegionID, float DeltaTime)
{
    if (!RegionStates.Contains(RegionID))
    {
        return;
    }
    
    FRegionState& Region = RegionStates[RegionID];
    
    // Calculate player impact in this region
    float PlayerImpact = GetPlayerRegionalImpact(RegionID);
    
    // Update population based on prosperity and disease rate
    // Update crime heat level based on player actions
    Region.CrimeHeatLevel = FMath::FInterpTo(Region.CrimeHeatLevel, Region.CrimeHeatLevel - (PlayerImpact * 0.1f), DeltaTime, 0.01f);
    
    // Occasionally generate random events
    if (FMath::RandRange(0.0f, 100.0f) < DeltaTime * 0.5f)
    {
        GenerateRandomEvent(RegionID);
    }
    
    // Notify listeners of state change
    OnWorldStateChanged.Broadcast(RegionID, Region);
}

void UWorldPersistenceSystem::GenerateRandomEvent(FName RegionID)
{
    if (!RegionStates.Contains(RegionID))
    {
        return;
    }
    
    FRegionState& Region = RegionStates[RegionID];
    
    // Define possible event types based on region state
    TArray<FName> PossibleEvents;
    
    // Economic events
    // Crime events
    if (Region.CrimeHeatLevel > 0.6f)
    {
        PossibleEvents.Add(FName("BanditRaid"));
        PossibleEvents.Add(FName("CriminalTakeover"));
    }
    else if (Region.CrimeHeatLevel < 0.3f)
    {
        PossibleEvents.Add(FName("GuardPatrols"));
        PossibleEvents.Add(FName("CriminalArrest"));
    }
    
    // Always add some generic events
    PossibleEvents.Add(FName("TravelersArrival"));
    PossibleEvents.Add(FName("WeatherEvent"));
    PossibleEvents.Add(FName("LocalCelebration"));
    
    // Select a random event
    if (PossibleEvents.Num() > 0)
    {
        FName EventType = PossibleEvents[FMath::RandRange(0, PossibleEvents.Num() - 1)];
        
        // Generate event parameters
        TMap<FString, FString> Parameters;
        Parameters.Add("RegionName", RegionID.ToString());
        Parameters.Add("CrimeHeat", FString::Printf(TEXT("%.2f"), Region.CrimeHeatLevel));

        // Generate event description
        FString EventDescription = GenerateEventDescription(EventType, RegionID, Parameters);

        // Add to region's current events
        Region.CurrentEvents.Add(FName(*EventDescription));

        // Limit the number of stored events
        const int32 MaxEvents = 20;
        if (Region.CurrentEvents.Num() > MaxEvents)
        {
            Region.CurrentEvents.RemoveAt(0, Region.CurrentEvents.Num() - MaxEvents);
        }

        // Apply event effects
        if (EventType == FName("BanditRaid"))
        {
            Region.CrimeHeatLevel += 0.1f;
        }
        else if (EventType == FName("GuardPatrols"))
        {
            Region.CrimeHeatLevel -= 0.1f;
        }

        // Clamp values
        Region.CrimeHeatLevel = FMath::Clamp(Region.CrimeHeatLevel, 0.05f, 0.95f);
        
        // Notify listeners
        OnWorldEvent.Broadcast(EventType, RegionID, EventDescription);
        
        // Log the event
        FWorldEventLogEntry LogEntry;
        LogEntry.EventType = EventType;
        LogEntry.RegionID = RegionID;
        LogEntry.Description = EventDescription;
        LogEntry.Timestamp = (TimeSystem ? TimeSystem->GetCurrentDateTime() : FGameDateTime());
        GlobalEventLog.Add(LogEntry);

        // Limit log size
        if (GlobalEventLog.Num() > 1000)
        {
            GlobalEventLog.RemoveAt(0, GlobalEventLog.Num() - 1000);
        }

        UE_LOG(LogTemp, Log, TEXT("Generated random event in %s: %s"), *RegionID.ToString(), *EventDescription);
    }
}

void UWorldPersistenceSystem::ApplyActionEffects(const FWorldAction& Action)
{
    if (!RegionStates.Contains(Action.RegionID))
    {
        return;
    }
    
    FRegionState& Region = RegionStates[Action.RegionID];
    
    // Apply effects based on action type
    switch (Action.ActionType)
    {
        case EWorldActionType::Combat:
            // Combat increases crime heat
            Region.CrimeHeatLevel += Action.Magnitude * 0.05f;
            break;

        case EWorldActionType::Crime:
            // Crime increases crime heat
            Region.CrimeHeatLevel += Action.Magnitude * 0.1f;
            break;
    }

    // Clamp values
    Region.CrimeHeatLevel = FMath::Clamp(Region.CrimeHeatLevel, 0.05f, 0.95f);
    
    // Generate an event description for significant actions
    if (Action.Magnitude > 0.5f)
    {
        TMap<FString, FString> Parameters;
        Parameters.Add("ActionType", UEnum::GetValueAsString(Action.ActionType));
        Parameters.Add("Magnitude", FString::Printf(TEXT("%.2f"), Action.Magnitude));
        if (!Action.TargetID.IsNone())
        {
            Parameters.Add("Target", Action.TargetID.ToString());
        }
        
        FString EventDescription = FString::Printf(TEXT("Player %s action in %s"), 
            *UEnum::GetValueAsString(Action.ActionType), *Action.RegionID.ToString());
        
        // Add to region's recent events
        Region.CurrentEvents.Add(FName(*EventDescription));

        // Limit the number of stored events
        const int32 MaxEvents = 20;
        if (Region.CurrentEvents.Num() > MaxEvents)
        {
            Region.CurrentEvents.RemoveAt(0, Region.CurrentEvents.Num() - MaxEvents);
        }

        // Log the event
        FWorldEventLogEntry LogEntry;
        LogEntry.EventType = FName(*UEnum::GetValueAsString(Action.ActionType));
        LogEntry.RegionID = Action.RegionID;
        LogEntry.Description = EventDescription;
        LogEntry.Timestamp = (TimeSystem ? TimeSystem->GetCurrentDateTime() : FGameDateTime());
        GlobalEventLog.Add(LogEntry);

        // Limit log size
        if (GlobalEventLog.Num() > 1000)
        {
            GlobalEventLog.RemoveAt(0, GlobalEventLog.Num() - 1000);
        }
    }
}

void UWorldPersistenceSystem::SimulateNPCActions(float DeltaTime)
{
    // For each region, simulate NPC actions
    for (auto& RegionPair : RegionStates)
    {
        FRegionState& Region = RegionPair.Value;
        
        // Calculate number of NPC actions based on population
        // Simulate some NPC actions that might affect crime heat
        if (FMath::FRand() < Region.CrimeHeatLevel * 0.1f)
        {
            Region.CrimeHeatLevel += 0.01f;
        }
        else
        {
            Region.CrimeHeatLevel -= 0.005f;
        }

        // Clamp values
        Region.CrimeHeatLevel = FMath::Clamp(Region.CrimeHeatLevel, 0.05f, 0.95f);
    }
}

void UWorldPersistenceSystem::SimulateFactionDynamics(float DeltaTime)
{
    // This is a simplified simulation of faction activities
    // In a full implementation, this would interact with the FactionManagerSubsystem
    
    // For each region, simulate faction influence
    for (auto& RegionPair : RegionStates)
    {
        FRegionState& Region = RegionPair.Value;
        
        // Skip regions with no controlling faction
        // This section is now mostly irrelevant due to the change in FRegionState
    }
}

void UWorldPersistenceSystem::UpdateResourceLevels(float DeltaTime)
{
    // This section is now mostly irrelevant due to the change in FRegionState
}

FString UWorldPersistenceSystem::GenerateEventDescription(FName EventType, FName RegionID, const TMap<FString, FString>& Parameters)
{
    // Generate a descriptive string for the event
    FString Description;
    
    if (EventType == FName("EconomicBoom"))
    {
        Description = FString::Printf(TEXT("Economic boom in %s! Prosperity is on the rise."), *RegionID.ToString());
    }
    else if (EventType == FName("EconomicDecline"))
    {
        Description = FString::Printf(TEXT("Economic troubles in %s. Businesses are struggling."), *RegionID.ToString());
    }
    else if (EventType == FName("TradeCaravan"))
    {
        Description = FString::Printf(TEXT("A large trade caravan has arrived in %s, bringing exotic goods."), *RegionID.ToString());
    }
    else if (EventType == FName("FoodShortage"))
    {
        Description = FString::Printf(TEXT("Food shortages reported in %s. Prices are rising."), *RegionID.ToString());
    }
    else if (EventType == FName("BanditRaid"))
    {
        Description = FString::Printf(TEXT("Bandits have raided settlements in %s. Guards are on high alert."), *RegionID.ToString());
    }
    else if (EventType == FName("CriminalTakeover"))
    {
        Description = FString::Printf(TEXT("Criminal elements have gained significant influence in %s."), *RegionID.ToString());
    }
    else if (EventType == FName("GuardPatrols"))
    {
        Description = FString::Printf(TEXT("Increased guard patrols in %s have reduced criminal activity."), *RegionID.ToString());
    }
    else if (EventType == FName("CriminalArrest"))
    {
        Description = FString::Printf(TEXT("A notorious criminal has been arrested in %s."), *RegionID.ToString());
    }
    else if (EventType == FName("Epidemic"))
    {
        Description = FString::Printf(TEXT("Disease outbreak in %s! Residents are advised to take precautions."), *RegionID.ToString());
    }
    else if (EventType == FName("Quarantine"))
    {
        Description = FString::Printf(TEXT("Parts of %s have been quarantined due to disease concerns."), *RegionID.ToString());
    }
    else if (EventType == FName("HealthImprovement"))
    {
        Description = FString::Printf(TEXT("Health conditions in %s have improved thanks to new medical practices."), *RegionID.ToString());
    }
    else if (EventType == FName("MedicalBreakthrough"))
    {
        Description = FString::Printf(TEXT("Healers in %s have discovered a new treatment for common ailments."), *RegionID.ToString());
    }
    else if (EventType == FName("ResourceShortage"))
    {
        Description = FString::Printf(TEXT("Resource shortages reported in %s. Prices are rising."), *RegionID.ToString());
    }
    else if (EventType == FName("TravelersArrival"))
    {
        Description = FString::Printf(TEXT("A group of travelers has arrived in %s, bringing news from distant lands."), *RegionID.ToString());
    }
    else if (EventType == FName("WeatherEvent"))
    {
        Description = FString::Printf(TEXT("Unusual weather patterns have been observed in %s."), *RegionID.ToString());
    }
    else if (EventType == FName("LocalCelebration"))
    {
        Description = FString::Printf(TEXT("Residents of %s are celebrating a local festival."), *RegionID.ToString());
    }
    else
    {
        Description = FString::Printf(TEXT("An event has occurred in %s."), *RegionID.ToString());
    }
    
    // Add timestamp if we have a time system
    if (TimeSystem)
    {
        FGameDateTime CurrentTime = TimeSystem->GetCurrentDateTime();
        Description = FString::Printf(TEXT("[Year %d, Month %d, Day %d] %s"), 
            CurrentTime.Year, CurrentTime.Month, CurrentTime.Day, *Description);
    }
    
    return Description;
}

FString UWorldPersistenceSystem::GetWorldStateSavePath() const
{
    return FPaths::ProjectSavedDir() / TEXT("WorldState.json");
}

TArray<FWorldEventLogEntry> UWorldPersistenceSystem::QueryWorldEvents(FName EventType, FName RegionID, int32 MaxResults) const
{
    TArray<FWorldEventLogEntry> Results;
    for (int32 i = GlobalEventLog.Num() - 1; i >= 0 && Results.Num() < MaxResults; --i)
    {
        const FWorldEventLogEntry& Entry = GlobalEventLog[i];
        if ((EventType.IsNone() || Entry.EventType == EventType) &&
            (RegionID.IsNone() || Entry.RegionID == RegionID))
        {
            Results.Add(Entry);
        }
    }
    return Results;
}

int32 UWorldPersistenceSystem::GetEventCount(FName EventType, FName RegionID) const
{
    int32 Count = 0;
    for (const FWorldEventLogEntry& Entry : GlobalEventLog)
    {
        if ((EventType.IsNone() || Entry.EventType == EventType) &&
            (RegionID.IsNone() || Entry.RegionID == RegionID))
        {
            ++Count;
        }
    }
    return Count;
}

TArray<EGlobalEventType> UWorldPersistenceSystem::GetListenedEventTypes() const
{
    return {
        EGlobalEventType::WorldSaveRequested,
        EGlobalEventType::WorldLoadRequested,
        EGlobalEventType::SeasonChanged,
        EGlobalEventType::WeatherChanged,
        EGlobalEventType::EnvironmentalEventTriggered
    };
}
