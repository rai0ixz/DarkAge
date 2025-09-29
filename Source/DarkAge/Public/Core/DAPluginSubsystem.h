// Copyright (c) 2025 RaioCore
// Plugin/Extension system for DarkAge - allows modular expansion

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "UObject/Interface.h"
#include "DAPluginSubsystem.generated.h"

class IDAPlugin;

/**
 * Plugin types for categorization and loading order
 */
UENUM(BlueprintType)
enum class EDAPluginType : uint8
{
    Core            UMETA(DisplayName = "Core System"),
    Gameplay        UMETA(DisplayName = "Gameplay"),
    UI              UMETA(DisplayName = "User Interface"),
    Audio           UMETA(DisplayName = "Audio"),
    Graphics        UMETA(DisplayName = "Graphics"),
    Network         UMETA(DisplayName = "Network"),
    AI              UMETA(DisplayName = "AI"),
    Economy         UMETA(DisplayName = "Economy"),
    Content         UMETA(DisplayName = "Content"),
    Debug           UMETA(DisplayName = "Debug/Development")
};

/**
 * Plugin loading priority
 */
UENUM(BlueprintType)
enum class EDAPluginPriority : uint8
{
    Critical    = 0     UMETA(DisplayName = "Critical"),
    High        = 1     UMETA(DisplayName = "High"),
    Normal      = 2     UMETA(DisplayName = "Normal"),
    Low         = 3     UMETA(DisplayName = "Low"),
    Optional    = 4     UMETA(DisplayName = "Optional")
};

/**
 * Plugin state tracking
 */
UENUM(BlueprintType)
enum class EDAPluginState : uint8
{
    Unloaded        UMETA(DisplayName = "Unloaded"),
    Loading         UMETA(DisplayName = "Loading"),
    Loaded          UMETA(DisplayName = "Loaded"),
    Initializing    UMETA(DisplayName = "Initializing"),
    Active          UMETA(DisplayName = "Active"),
    Error           UMETA(DisplayName = "Error"),
    Disabled        UMETA(DisplayName = "Disabled")
};

/**
 * Plugin metadata structure
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAPluginInfo
{
    GENERATED_BODY()

    /** Unique plugin identifier */
    UPROPERTY(BlueprintReadOnly)
    FString PluginID;

    /** Human-readable plugin name */
    UPROPERTY(BlueprintReadOnly)
    FString DisplayName;

    /** Plugin description */
    UPROPERTY(BlueprintReadOnly)
    FString Description;

    /** Plugin version */
    UPROPERTY(BlueprintReadOnly)
    FString Version;

    /** Plugin author */
    UPROPERTY(BlueprintReadOnly)
    FString Author;

    /** Plugin type */
    UPROPERTY(BlueprintReadOnly)
    EDAPluginType Type;

    /** Loading priority */
    UPROPERTY(BlueprintReadOnly)
    EDAPluginPriority Priority;

    /** Current state */
    UPROPERTY(BlueprintReadOnly)
    EDAPluginState State;

    /** Dependencies (other plugin IDs) */
    UPROPERTY(BlueprintReadOnly)
    TArray<FString> Dependencies;

    /** Optional dependencies */
    UPROPERTY(BlueprintReadOnly)
    TArray<FString> OptionalDependencies;

    /** Minimum game version required */
    UPROPERTY(BlueprintReadOnly)
    FString MinGameVersion;

    /** Whether plugin can be disabled at runtime */
    UPROPERTY(BlueprintReadOnly)
    bool bCanDisableAtRuntime;

    /** Whether plugin requires restart to enable/disable */
    UPROPERTY(BlueprintReadOnly)
    bool bRequiresRestart;

    FDAPluginInfo()
        : Type(EDAPluginType::Gameplay)
        , Priority(EDAPluginPriority::Normal)
        , State(EDAPluginState::Unloaded)
        , bCanDisableAtRuntime(true)
        , bRequiresRestart(false)
    {}
};

/**
 * Plugin interface - all plugins must implement this
 */
UINTERFACE(BlueprintType, Blueprintable)
class DARKAGE_API UDAPlugin : public UInterface
{
    GENERATED_BODY()
};

class DARKAGE_API IDAPlugin
{
    GENERATED_BODY()

public:
    /** Get plugin information */
    virtual FDAPluginInfo GetPluginInfo() const = 0;

    /** Initialize the plugin */
    virtual bool Initialize() = 0;

    /** Shutdown the plugin */
    virtual void Shutdown() = 0;

    /** Called every frame (if plugin needs tick) */
    virtual void Tick(float DeltaTime) {}

    /** Called when plugin is enabled */
    virtual void OnEnabled() {}

    /** Called when plugin is disabled */
    virtual void OnDisabled() {}

    /** Check if plugin can be safely disabled */
    virtual bool CanDisable() const { return true; }

    /** Get plugin configuration data */
    virtual TMap<FString, FString> GetConfiguration() const { return TMap<FString, FString>(); }

    /** Set plugin configuration data */
    virtual void SetConfiguration(const TMap<FString, FString>& Config) {}
};

/**
 * Plugin event delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPluginStateChanged, const FString&, PluginID, EDAPluginState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPluginLoaded, const FString&, PluginID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPluginUnloaded, const FString&, PluginID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPluginError, const FString&, PluginID, const FString&, ErrorMessage);

/**
 * Plugin management subsystem
 * Handles loading, unloading, and lifecycle management of plugins
 */
UCLASS(BlueprintType)
class DARKAGE_API UDAPluginSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    /**
     * Register a plugin instance
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool RegisterPlugin(TScriptInterface<IDAPlugin> Plugin);

    /**
     * Unregister a plugin
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool UnregisterPlugin(const FString& PluginID);

    /**
     * Load a plugin by ID
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool LoadPlugin(const FString& PluginID);

    /**
     * Unload a plugin by ID
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool UnloadPlugin(const FString& PluginID);

    /**
     * Enable a plugin
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool EnablePlugin(const FString& PluginID);

    /**
     * Disable a plugin
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool DisablePlugin(const FString& PluginID);

    /**
     * Get all registered plugins
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    TArray<FDAPluginInfo> GetAllPlugins() const;

    /**
     * Get plugins by type
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    TArray<FDAPluginInfo> GetPluginsByType(EDAPluginType Type) const;

    /**
     * Get plugin info by ID
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool GetPluginInfo(const FString& PluginID, FDAPluginInfo& OutInfo) const;

    /**
     * Check if plugin is loaded and active
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool IsPluginActive(const FString& PluginID) const;

    /**
     * Get plugin by ID
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    TScriptInterface<IDAPlugin> GetPlugin(const FString& PluginID) const;

    /**
     * Reload a plugin (unload then load)
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool ReloadPlugin(const FString& PluginID);

    /**
     * Load all plugins of a specific type
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    int32 LoadPluginsByType(EDAPluginType Type);

    /**
     * Get plugin dependencies
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    TArray<FString> GetPluginDependencies(const FString& PluginID) const;

    /**
     * Check if all dependencies are satisfied
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    bool AreDependenciesSatisfied(const FString& PluginID) const;

    /**
     * Get plugins that depend on the specified plugin
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    TArray<FString> GetDependentPlugins(const FString& PluginID) const;

    /**
     * Save plugin configuration
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    void SavePluginConfiguration();

    /**
     * Load plugin configuration
     */
    UFUNCTION(BlueprintCallable, Category = "DarkAge|Plugins")
    void LoadPluginConfiguration();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "DarkAge|Plugins")
    FOnPluginStateChanged OnPluginStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "DarkAge|Plugins")
    FOnPluginLoaded OnPluginLoaded;

    UPROPERTY(BlueprintAssignable, Category = "DarkAge|Plugins")
    FOnPluginUnloaded OnPluginUnloaded;

    UPROPERTY(BlueprintAssignable, Category = "DarkAge|Plugins")
    FOnPluginError OnPluginError;

protected:
    /**
     * Set plugin state and broadcast event
     */
    void SetPluginState(const FString& PluginID, EDAPluginState NewState);

    /**
     * Load plugins in dependency order
     */
    void LoadPluginsInOrder();

    /**
     * Validate plugin dependencies
     */
    bool ValidateDependencies(const FString& PluginID, TArray<FString>& MissingDependencies) const;

    /**
     * Get load order for plugins
     */
    TArray<FString> GetLoadOrder() const;

private:
    /** Registered plugin instances */
    UPROPERTY()
    TMap<FString, TScriptInterface<IDAPlugin>> RegisteredPlugins;

    /** Plugin information cache */
    UPROPERTY()
    TMap<FString, FDAPluginInfo> PluginInfoCache;

    /** Plugins that need ticking */
    UPROPERTY()
    TArray<FString> TickingPlugins;

    /** Plugin configuration file path */
    FString ConfigFilePath;

    /** Whether subsystem is initialized */
    bool bIsInitialized;
};

/**
 * Base class for C++ plugins
 */
UCLASS(Abstract, Blueprintable)
class DARKAGE_API UDAPluginBase : public UObject, public IDAPlugin
{
    GENERATED_BODY()

public:
    UDAPluginBase();

    // IDAPlugin interface
    virtual FDAPluginInfo GetPluginInfo() const override;
    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnEnabled() override;
    virtual void OnDisabled() override;
    virtual bool CanDisable() const override;
    virtual TMap<FString, FString> GetConfiguration() const override;
    virtual void SetConfiguration(const TMap<FString, FString>& Config) override;

protected:
    /** Plugin information */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plugin Info")
    FDAPluginInfo PluginInfo;

    /** Plugin configuration */
    UPROPERTY(BlueprintReadWrite, Category = "Plugin Config")
    TMap<FString, FString> Configuration;

    /** Whether this plugin needs ticking */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plugin Behavior")
    bool bNeedsTick;

    // Blueprint implementable events
    UFUNCTION(BlueprintImplementableEvent, Category = "Plugin Events")
    void OnPluginInitialize();

    UFUNCTION(BlueprintImplementableEvent, Category = "Plugin Events")
    void OnPluginShutdown();

    UFUNCTION(BlueprintImplementableEvent, Category = "Plugin Events")
    void OnPluginTick(float DeltaTime);

    UFUNCTION(BlueprintImplementableEvent, Category = "Plugin Events")
    void OnPluginEnabled();

    UFUNCTION(BlueprintImplementableEvent, Category = "Plugin Events")
    void OnPluginDisabled();
};