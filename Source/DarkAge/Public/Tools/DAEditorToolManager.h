#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tools/DAEditorToolBase.h"
#include "Core/GlobalEventBus.h"

class UUserWidget;


 /**
  * Tool Manager Events
  * (Declared after generated.h)
  */
/**
 * DarkAge Editor Tool Manager
 *
 * This subsystem manages all in-engine development tools, providing
 * registration, lifecycle management, UI coordination, and data persistence.
 */
#include "DAEditorToolManager.generated.h"

// Note: Dynamic multicast delegates must be declared after including the generated.h
// so that the wrapper declarations are visible to the compiler.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolRegistered, UDAEditorToolBase*, Tool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolUnregistered, UDAEditorToolBase*, Tool);
// Use a unique name to avoid conflict with UDAEditorToolBase::FOnToolStateChanged
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolManagerToolStateChanged, UDAEditorToolBase*, Tool, EDAEditorToolState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveToolChanged, UDAEditorToolBase*, NewActiveTool);

UCLASS(BlueprintType)
class DARKAGE_API UDAEditorToolManager : public UGameInstanceSubsystem, public IGlobalEventListener
{
	GENERATED_BODY()

public:
	UDAEditorToolManager();

	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// IGlobalEventListener interface
	virtual TArray<EGlobalEventType> GetListenedEventTypes() const override;
	virtual int32 GetListenerPriority() const override { return 20; } // High priority for tools

	// --- Tool Registration ---

	/**
	 * Register a tool class for automatic instantiation
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool RegisterToolClass(TSubclassOf<UDAEditorToolBase> ToolClass, const FDAEditorToolInfo& ToolInfo);

	/**
	 * Register an existing tool instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool RegisterTool(UDAEditorToolBase* Tool);

	/**
	 * Unregister a tool
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool UnregisterTool(UDAEditorToolBase* Tool);

	/**
	 * Create and register a tool from its class
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	UDAEditorToolBase* CreateTool(TSubclassOf<UDAEditorToolBase> ToolClass);

	// --- Tool Access ---

	/**
	 * Get all registered tools
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	TArray<UDAEditorToolBase*> GetAllTools() const;

	/**
	 * Get tools by category
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	TArray<UDAEditorToolBase*> GetToolsByCategory(EDAEditorToolCategory Category) const;

	/**
	 * Get tools by access level
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	TArray<UDAEditorToolBase*> GetToolsByAccessLevel(EDAEditorToolAccess AccessLevel) const;

	/**
	 * Find a tool by name
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	UDAEditorToolBase* FindToolByName(const FString& ToolName) const;

	/**
	 * Find a tool by class
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	UDAEditorToolBase* FindToolByClass(TSubclassOf<UDAEditorToolBase> ToolClass) const;

	/**
	 * Get the currently active tool
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	UDAEditorToolBase* GetActiveTool() const { return ActiveTool; }

	/**
	 * Get all currently active tools
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools")
	TArray<UDAEditorToolBase*> GetActiveTools() const;

	// --- Tool Lifecycle Management ---

	/**
	 * Activate a tool
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool ActivateTool(UDAEditorToolBase* Tool);

	/**
	 * Activate a tool by name
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool ActivateToolByName(const FString& ToolName);

	/**
	 * Deactivate a tool
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool DeactivateTool(UDAEditorToolBase* Tool);

	/**
	 * Deactivate all tools
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	void DeactivateAllTools();

	/**
	 * Shutdown a tool
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	bool ShutdownTool(UDAEditorToolBase* Tool);

	/**
	 * Shutdown all tools
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools")
	void ShutdownAllTools();

	// --- UI Management ---

	/**
	 * Show the tool manager UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|UI")
	void ShowToolManagerUI();

	/**
	 * Hide the tool manager UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|UI")
	void HideToolManagerUI();

	/**
	 * Toggle the tool manager UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|UI")
	void ToggleToolManagerUI();

	/**
	 * Get the tool manager UI widget
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools|UI")
	UUserWidget* GetToolManagerWidget() const { return ToolManagerWidget; }

	/**
	 * Refresh all tool UIs
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|UI")
	void RefreshAllToolUIs();

	// --- Data Management ---

	/**
	 * Save all tool data
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Data")
	bool SaveAllToolData();

	/**
	 * Load all tool data
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Data")
	bool LoadAllToolData();

	/**
	 * Save tool layout and preferences
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Data")
	bool SaveToolLayout();

	/**
	 * Load tool layout and preferences
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Data")
	bool LoadToolLayout();

	/**
	 * Check if any tools have unsaved changes
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools|Data")
	bool HasUnsavedChanges() const;

	/**
	 * Get tools with unsaved changes
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools|Data")
	TArray<UDAEditorToolBase*> GetToolsWithUnsavedChanges() const;

	// --- Settings ---

	/**
	 * Enable/disable auto-save for all tools
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Settings")
	void SetAutoSaveEnabled(bool bEnabled);

	/**
	 * Set auto-save interval for all tools
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Settings")
	void SetAutoSaveInterval(float Interval);

	/**
	 * Enable/disable debug mode for all tools
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tools|Settings")
	void SetDebugModeEnabled(bool bEnabled);

	// --- Events ---
UPROPERTY(BlueprintAssignable, Category = "Editor Tools|Events")
FOnToolRegistered OnToolRegistered;

UPROPERTY(BlueprintAssignable, Category = "Editor Tools|Events")
FOnToolUnregistered OnToolUnregistered;

UPROPERTY(BlueprintAssignable, Category = "Editor Tools|Events")
// Use the uniquely named manager state-changed delegate type
FOnToolManagerToolStateChanged OnToolStateChanged;

UPROPERTY(BlueprintAssignable, Category = "Editor Tools|Events")
FOnActiveToolChanged OnActiveToolChanged;

	// --- Static Access ---

	/**
	 * Get the tool manager instance
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tools", meta = (CallInEditor = "true"))
	static UDAEditorToolManager* Get(const UObject* WorldContext);

protected:
	/**
	 * Registered tools
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools")
	TArray<UDAEditorToolBase*> RegisteredTools;

	/**
	 * Tool registration information
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools")
	TMap<UDAEditorToolBase*, FDAEditorToolInfo> ToolInfoMap;

	/**
	 * Currently active tool (for modal tools)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools")
	UDAEditorToolBase* ActiveTool;

	/**
	 * Tool manager UI widget
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ToolManagerWidgetClass;

	UPROPERTY()
	UUserWidget* ToolManagerWidget;

	/**
	 * Settings
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAutoSaveEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float AutoSaveInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bDebugModeEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bShowToolManagerOnStartup;

	/**
	 * Auto-registration of tool classes
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Registration")
	TArray<FDAEditorToolInfo> AutoRegisterToolInfos;

	/**
	 * References
	 */
	UPROPERTY()
	UGlobalEventBus* GlobalEventBus;

private:
	// Auto-save timer
	float AutoSaveTimer;

	// Helper methods
	void RegisterWithEventBus();
	void UnregisterFromEventBus();
	void HandleAutoSave(float DeltaTime);
	void AutoRegisterTools();
	void SetActiveTool(UDAEditorToolBase* Tool);
	bool CanActivateTool(UDAEditorToolBase* Tool) const;
	void OnToolStateChangedInternal(UDAEditorToolBase* Tool, EDAEditorToolState NewState);

	// Event handling
	UFUNCTION()
	void OnGlobalEvent(const FGlobalEvent& Event);

	// Tool event callbacks
	UFUNCTION()
	void OnToolStateChangedCallback(EDAEditorToolState NewState);
};

/**
 * Tool Manager Configuration
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAEditorToolManagerConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bAutoSaveEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	float AutoSaveInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bDebugModeEnabled;

	FDAEditorToolManagerConfig()
		: bAutoSaveEnabled(true)
		, AutoSaveInterval(300.0f)
		, bDebugModeEnabled(false)
	{
	}
};
