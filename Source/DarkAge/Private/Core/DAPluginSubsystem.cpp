// Copyright (c) 2025 RaioCore

#include "Core/DAPluginSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

bool UDAPluginSubsystem::IsTickable() const
{
    return true;
}

TStatId UDAPluginSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UDAPluginSubsystem, STATGROUP_Tickables);
}

void UDAPluginSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bIsInitialized = false;
    ConfigFilePath = FPaths::ProjectSavedDir() / TEXT("Config") / TEXT("PluginConfig.json");
    
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Initializing..."));
    
    // Initialize plugin registry
    RegisteredPlugins.Empty();
    PluginInfoCache.Empty();
    TickingPlugins.Empty();
    
    // Check for a reset flag to force-delete corrupted save file
    const FString ResetFlagPath = FPaths::ProjectSavedDir() + TEXT("ResetPluginConfig.flag");
    if (IFileManager::Get().FileExists(*ResetFlagPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("ResetPluginConfig.flag found. Deleting PluginConfig.json and re-initializing."));
        IFileManager::Get().Delete(*ConfigFilePath);
        IFileManager::Get().Delete(*ResetFlagPath);
    }
    
    // Load plugin configuration
    LoadPluginConfiguration();
    
    bIsInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Initialized successfully"));
}

void UDAPluginSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Deinitializing..."));
    
    // Save configuration before shutdown
    SavePluginConfiguration();
    
    // Shutdown all registered plugins
    for (auto& PluginPair : RegisteredPlugins)
    {
        if (PluginPair.Value.GetInterface())
        {
            PluginPair.Value.GetInterface()->Shutdown();
            SetPluginState(PluginPair.Key, EDAPluginState::Unloaded);
        }
    }
    
    RegisteredPlugins.Empty();
    PluginInfoCache.Empty();
    TickingPlugins.Empty();
    bIsInitialized = false;
    
    Super::Deinitialize();
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Deinitialized"));
}

void UDAPluginSubsystem::Tick(float DeltaTime)
{
    if (!bIsInitialized)
    {
        return;
    }
    
    // Tick all plugins that need ticking
    for (const FString& PluginID : TickingPlugins)
    {
        if (RegisteredPlugins.Contains(PluginID))
        {
            TScriptInterface<IDAPlugin> Plugin = RegisteredPlugins[PluginID];
            if (Plugin.GetInterface())
            {
                Plugin.GetInterface()->Tick(DeltaTime);
            }
        }
    }
}

bool UDAPluginSubsystem::RegisterPlugin(TScriptInterface<IDAPlugin> Plugin)
{
    if (!Plugin.GetInterface())
    {
        UE_LOG(LogTemp, Warning, TEXT("DAPluginSubsystem: Cannot register null plugin"));
        return false;
    }
    
    FDAPluginInfo PluginInfo = Plugin.GetInterface()->GetPluginInfo();
    
    if (RegisteredPlugins.Contains(PluginInfo.PluginID))
    {
        UE_LOG(LogTemp, Warning, TEXT("DAPluginSubsystem: Plugin already registered: %s"), *PluginInfo.PluginID);
        return false;
    }
    
    // Initialize the plugin
    SetPluginState(PluginInfo.PluginID, EDAPluginState::Initializing);
    
    if (!Plugin.GetInterface()->Initialize())
    {
        UE_LOG(LogTemp, Error, TEXT("DAPluginSubsystem: Failed to initialize plugin: %s"), *PluginInfo.PluginID);
        SetPluginState(PluginInfo.PluginID, EDAPluginState::Error);
        return false;
    }
    
    RegisteredPlugins.Add(PluginInfo.PluginID, Plugin);
    PluginInfoCache.Add(PluginInfo.PluginID, PluginInfo);
    SetPluginState(PluginInfo.PluginID, EDAPluginState::Active);
    
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Successfully registered plugin: %s"), *PluginInfo.PluginID);
    OnPluginLoaded.Broadcast(PluginInfo.PluginID);
    
    return true;
}

bool UDAPluginSubsystem::UnregisterPlugin(const FString& PluginID)
{
    if (!RegisteredPlugins.Contains(PluginID))
    {
        UE_LOG(LogTemp, Warning, TEXT("DAPluginSubsystem: Plugin not found for unregistration: %s"), *PluginID);
        return false;
    }
    
    TScriptInterface<IDAPlugin> Plugin = RegisteredPlugins[PluginID];
    if (Plugin.GetInterface())
    {
        Plugin.GetInterface()->Shutdown();
    }
    
    RegisteredPlugins.Remove(PluginID);
    PluginInfoCache.Remove(PluginID);
    TickingPlugins.Remove(PluginID);
    
    SetPluginState(PluginID, EDAPluginState::Unloaded);
    OnPluginUnloaded.Broadcast(PluginID);
    
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Successfully unregistered plugin: %s"), *PluginID);
    
    return true;
}

bool UDAPluginSubsystem::LoadPlugin(const FString& PluginID)
{
    // For now, this is a placeholder since dynamic loading would require more complex implementation
    UE_LOG(LogTemp, Warning, TEXT("DAPluginSubsystem: Dynamic plugin loading not yet implemented for: %s"), *PluginID);
    return false;
}

bool UDAPluginSubsystem::UnloadPlugin(const FString& PluginID)
{
    return UnregisterPlugin(PluginID);
}

bool UDAPluginSubsystem::EnablePlugin(const FString& PluginID)
{
    if (!RegisteredPlugins.Contains(PluginID))
    {
        return false;
    }
    
    TScriptInterface<IDAPlugin> Plugin = RegisteredPlugins[PluginID];
    if (Plugin.GetInterface())
    {
        Plugin.GetInterface()->OnEnabled();
        SetPluginState(PluginID, EDAPluginState::Active);
        return true;
    }
    
    return false;
}

bool UDAPluginSubsystem::DisablePlugin(const FString& PluginID)
{
    if (!RegisteredPlugins.Contains(PluginID))
    {
        return false;
    }
    
    TScriptInterface<IDAPlugin> Plugin = RegisteredPlugins[PluginID];
    if (Plugin.GetInterface() && Plugin.GetInterface()->CanDisable())
    {
        Plugin.GetInterface()->OnDisabled();
        SetPluginState(PluginID, EDAPluginState::Disabled);
        return true;
    }
    
    return false;
}

TArray<FDAPluginInfo> UDAPluginSubsystem::GetAllPlugins() const
{
    TArray<FDAPluginInfo> AllPlugins;
    PluginInfoCache.GenerateValueArray(AllPlugins);
    return AllPlugins;
}

TArray<FDAPluginInfo> UDAPluginSubsystem::GetPluginsByType(EDAPluginType Type) const
{
    TArray<FDAPluginInfo> FilteredPlugins;
    
    for (const auto& PluginPair : PluginInfoCache)
    {
        if (PluginPair.Value.Type == Type)
        {
            FilteredPlugins.Add(PluginPair.Value);
        }
    }
    
    return FilteredPlugins;
}

bool UDAPluginSubsystem::GetPluginInfo(const FString& PluginID, FDAPluginInfo& OutInfo) const
{
    if (PluginInfoCache.Contains(PluginID))
    {
        OutInfo = PluginInfoCache[PluginID];
        return true;
    }
    
    return false;
}

bool UDAPluginSubsystem::IsPluginActive(const FString& PluginID) const
{
    if (PluginInfoCache.Contains(PluginID))
    {
        return PluginInfoCache[PluginID].State == EDAPluginState::Active;
    }
    
    return false;
}

TScriptInterface<IDAPlugin> UDAPluginSubsystem::GetPlugin(const FString& PluginID) const
{
    if (RegisteredPlugins.Contains(PluginID))
    {
        return RegisteredPlugins[PluginID];
    }
    
    return TScriptInterface<IDAPlugin>();
}

bool UDAPluginSubsystem::ReloadPlugin(const FString& PluginID)
{
    if (UnloadPlugin(PluginID))
    {
        return LoadPlugin(PluginID);
    }
    
    return false;
}

int32 UDAPluginSubsystem::LoadPluginsByType(EDAPluginType Type)
{
    int32 LoadedCount = 0;
    
    for (const auto& PluginPair : PluginInfoCache)
    {
        if (PluginPair.Value.Type == Type && PluginPair.Value.State != EDAPluginState::Active)
        {
            if (LoadPlugin(PluginPair.Key))
            {
                LoadedCount++;
            }
        }
    }
    
    return LoadedCount;
}

TArray<FString> UDAPluginSubsystem::GetPluginDependencies(const FString& PluginID) const
{
    if (PluginInfoCache.Contains(PluginID))
    {
        return PluginInfoCache[PluginID].Dependencies;
    }
    
    return TArray<FString>();
}

bool UDAPluginSubsystem::AreDependenciesSatisfied(const FString& PluginID) const
{
    TArray<FString> MissingDependencies;
    return ValidateDependencies(PluginID, MissingDependencies);
}

TArray<FString> UDAPluginSubsystem::GetDependentPlugins(const FString& PluginID) const
{
    TArray<FString> DependentPlugins;
    
    for (const auto& PluginPair : PluginInfoCache)
    {
        if (PluginPair.Value.Dependencies.Contains(PluginID))
        {
            DependentPlugins.Add(PluginPair.Key);
        }
    }
    
    return DependentPlugins;
}

void UDAPluginSubsystem::SavePluginConfiguration()
{
    // Placeholder for configuration saving
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Saving plugin configuration..."));
}

void UDAPluginSubsystem::LoadPluginConfiguration()
{
    // Placeholder for configuration loading
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Loading plugin configuration..."));
}

void UDAPluginSubsystem::SetPluginState(const FString& PluginID, EDAPluginState NewState)
{
    if (PluginInfoCache.Contains(PluginID))
    {
        PluginInfoCache[PluginID].State = NewState;
        OnPluginStateChanged.Broadcast(PluginID, NewState);
    }
}

void UDAPluginSubsystem::LoadPluginsInOrder()
{
    // Placeholder for dependency-ordered loading
    UE_LOG(LogTemp, Log, TEXT("DAPluginSubsystem: Loading plugins in dependency order..."));
}

bool UDAPluginSubsystem::ValidateDependencies(const FString& PluginID, TArray<FString>& MissingDependencies) const
{
    MissingDependencies.Empty();
    
    if (!PluginInfoCache.Contains(PluginID))
    {
        return false;
    }
    
    const FDAPluginInfo& PluginInfo = PluginInfoCache[PluginID];
    
    for (const FString& Dependency : PluginInfo.Dependencies)
    {
        if (!IsPluginActive(Dependency))
        {
            MissingDependencies.Add(Dependency);
        }
    }
    
    return MissingDependencies.Num() == 0;
}

TArray<FString> UDAPluginSubsystem::GetLoadOrder() const
{
    // Placeholder for topological sort of dependencies
    TArray<FString> LoadOrder;
    PluginInfoCache.GetKeys(LoadOrder);
    return LoadOrder;
}

// UDAPluginBase implementation
UDAPluginBase::UDAPluginBase()
{
    bNeedsTick = false;
}

FDAPluginInfo UDAPluginBase::GetPluginInfo() const
{
    return PluginInfo;
}

bool UDAPluginBase::Initialize()
{
    OnPluginInitialize();
    return true;
}

void UDAPluginBase::Shutdown()
{
    OnPluginShutdown();
}

void UDAPluginBase::Tick(float DeltaTime)
{
    if (bNeedsTick)
    {
        OnPluginTick(DeltaTime);
    }
}

void UDAPluginBase::OnEnabled()
{
    OnPluginEnabled();
}

void UDAPluginBase::OnDisabled()
{
    OnPluginDisabled();
}

bool UDAPluginBase::CanDisable() const
{
    return PluginInfo.bCanDisableAtRuntime;
}

TMap<FString, FString> UDAPluginBase::GetConfiguration() const
{
    return Configuration;
}

void UDAPluginBase::SetConfiguration(const TMap<FString, FString>& Config)
{
    Configuration = Config;
}