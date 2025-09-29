#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Core/GlobalEventBus.h"

class UDAEditorToolManager;
class UDAEditorToolWidget;

/**
 * Editor Tool Categories for organization
 */
UENUM(BlueprintType)
enum class EDAEditorToolCategory : uint8
{
    Content         UMETA(DisplayName = "Content Creation"),
    AI              UMETA(DisplayName = "AI & Behavior"),
    World           UMETA(DisplayName = "World Building"),
    Gameplay        UMETA(DisplayName = "Gameplay Systems"),
    Debug           UMETA(DisplayName = "Debug & Testing"),
    Custom          UMETA(DisplayName = "Custom Tools")
};

/**
 * Editor Tool Access Levels
 */
UENUM(BlueprintType)
enum class EDAEditorToolAccess : uint8
{
    Public          UMETA(DisplayName = "Public - All Users"),
    Designer        UMETA(DisplayName = "Designer - Content Creators"),
    Developer       UMETA(DisplayName = "Developer - Programmers"),
    Admin           UMETA(DisplayName = "Admin - Project Leads"),
    Debug           UMETA(DisplayName = "Debug - Development Only")
};

/**
 * Editor Tool State
 */
UENUM(BlueprintType)
enum class EDAEditorToolState : uint8
{
    Inactive        UMETA(DisplayName = "Inactive"),
    Active          UMETA(DisplayName = "Active"),
    Minimized       UMETA(DisplayName = "Minimized"),
    Modal           UMETA(DisplayName = "Modal"),
    Error           UMETA(DisplayName = "Error State")
};

/**
 * Base class for all DarkAge editor tools
 *
 * This provides a common framework for in-engine development tools,
 * including lifecycle management, UI integration, and data persistence.
 */
#include "DAEditorToolBase.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class DARKAGE_API UDAEditorToolBase : public UObject, public IGlobalEventListener
{
	GENERATED_BODY()

public:
	UDAEditorToolBase();

	// --- Tool Lifecycle ---

	/**
	 * Initialize the tool
	 * @param InManager The tool manager that owns this tool
	 * @return True if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool")
	virtual bool Initialize(UDAEditorToolManager* InManager);

	/**
	 * Activate the tool (show UI, start processing)
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool")
	virtual void Activate();

	/**
	 * Deactivate the tool (hide UI, stop processing)
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool")
	virtual void Deactivate();

	/**
	 * Shutdown the tool and clean up resources
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool")
	virtual void Shutdown();

	/**
	 * Update the tool (called every frame when active)
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool")
	virtual void Tick(float DeltaTime);

	// --- Tool Properties ---

	/**
	 * Get the tool's display name
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	virtual FString GetToolName() const { return ToolName; }

	/**
	 * Get the tool's description
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	virtual FString GetToolDescription() const { return ToolDescription; }

	/**
	 * Get the tool's category
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	EDAEditorToolCategory GetToolCategory() const { return ToolCategory; }

	/**
	 * Get the tool's access level
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	EDAEditorToolAccess GetAccessLevel() const { return AccessLevel; }

	/**
	 * Get the current tool state
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	EDAEditorToolState GetToolState() const { return ToolState; }

	/**
	 * Check if the tool is currently active
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	bool IsActive() const { return ToolState == EDAEditorToolState::Active; }

	/**
	 * Check if the tool requires exclusive access (modal)
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool")
	bool IsModal() const { return bIsModal; }

	// --- Data Management ---

	/**
	 * Save tool data to persistent storage
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	virtual bool SaveToolData();

	/**
	 * Load tool data from persistent storage
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	virtual bool LoadToolData();

	/**
	 * Get the tool's data as a JSON string
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	virtual FString GetToolDataAsJSON();

	/**
	 * Set the tool's data from a JSON string
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	virtual bool SetToolDataFromJSON(const FString& JSONData);

	/**
	 * Check if the tool has unsaved changes
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool|Data")
	bool HasUnsavedChanges() const { return bHasUnsavedChanges; }

	/**
	 * Mark the tool as having unsaved changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	void MarkAsModified() { bHasUnsavedChanges = true; }

	/**
	 * Clear the unsaved changes flag
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|Data")
	void ClearModifiedFlag() { bHasUnsavedChanges = false; }

	// --- UI Integration ---

	/**
	 * Create the tool's UI widget
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|UI")
	virtual UDAEditorToolWidget* CreateToolWidget();

	/**
	 * Get the tool's UI widget
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool|UI")
	UDAEditorToolWidget* GetToolWidget() const { return ToolWidget; }

	/**
	 * Refresh the tool's UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool|UI")
	virtual void RefreshUI();

	// --- Events ---

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolStateChanged, EDAEditorToolState, NewState);
	UPROPERTY(BlueprintAssignable, Category = "Editor Tool|Events")
	FOnToolStateChanged OnStateChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToolDataChanged);
	UPROPERTY(BlueprintAssignable, Category = "Editor Tool|Events")
	FOnToolDataChanged OnDataChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolError, const FString&, ErrorMessage);
	UPROPERTY(BlueprintAssignable, Category = "Editor Tool|Events")
	FOnToolError OnError;

	// IGlobalEventListener interface
	virtual TArray<EGlobalEventType> GetListenedEventTypes() const override;
	virtual int32 GetListenerPriority() const override { return 30; } // Lower priority than core systems

protected:
	/**
	 * Tool identification
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	FString ToolName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	FString ToolDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	EDAEditorToolCategory ToolCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	EDAEditorToolAccess AccessLevel;

	/**
	 * Tool behavior
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	bool bIsModal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	bool bAutoSave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	float AutoSaveInterval;

	/**
	 * UI Widget class to create for this tool
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	TSubclassOf<UDAEditorToolWidget> ToolWidgetClass;

	/**
	 * Tool runtime state
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool State")
	EDAEditorToolState ToolState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool State")
	bool bHasUnsavedChanges;

	/**
	 * References
	 */
	UPROPERTY()
	UDAEditorToolManager* ToolManager;

	UPROPERTY()
	UDAEditorToolWidget* ToolWidget;

	UPROPERTY()
	UGlobalEventBus* GlobalEventBus;

	// --- Virtual Methods for Subclasses ---

	/**
	 * Called when the tool is first initialized
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	void OnToolInitialized();

	/**
	 * Called when the tool becomes active
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	void OnToolActivated();

	/**
	 * Called when the tool becomes inactive
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	void OnToolDeactivated();

	/**
	 * Called every frame when the tool is active
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	void OnToolTick(float DeltaTime);

	/**
	 * Called when tool data should be saved
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	FString OnSaveToolData();

	/**
	 * Called when tool data should be loaded
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	bool OnLoadToolData(const FString& SavedData);

	/**
	 * Called when a global event is received
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool")
	void OnGlobalEventReceived(const FGlobalEvent& Event);

private:
	// Auto-save timer
	float AutoSaveTimer;

	// Helper methods
	void SetToolState(EDAEditorToolState NewState);
	void RegisterWithEventBus();
	void UnregisterFromEventBus();
	void HandleAutoSave(float DeltaTime);

	// Event handling
	UFUNCTION()
	void OnGlobalEvent(const FGlobalEvent& Event);
};
/**
 * Base class for editor tool UI widgets
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DARKAGE_API UDAEditorToolWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the widget with its associated tool
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool Widget")
	virtual void InitializeWidget(UDAEditorToolBase* InTool);

	/**
	 * Get the associated tool
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Tool Widget")
	UDAEditorToolBase* GetAssociatedTool() const { return AssociatedTool; }

	/**
	 * Refresh the widget's display
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool Widget")
	virtual void RefreshWidget();

	/**
	 * Handle tool state changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Tool Widget")
	virtual void OnToolStateChanged(EDAEditorToolState NewState);

protected:
	/**
	 * The tool this widget represents
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UDAEditorToolBase* AssociatedTool;

	/**
	 * Called when the widget is initialized
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool Widget")
	void OnWidgetInitialized();

	/**
	 * Called when the widget should refresh its display
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool Widget")
	void OnWidgetRefresh();

	/**
	 * Called when the associated tool's state changes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Editor Tool Widget")
	void OnAssociatedToolStateChanged(EDAEditorToolState NewState);
};
/**
 * Tool registration information
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FDAEditorToolInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	FString ToolName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	FString ToolDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	EDAEditorToolCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	EDAEditorToolAccess AccessLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	TSubclassOf<UDAEditorToolBase> ToolClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Info")
	bool bAutoRegister;

	FDAEditorToolInfo()
	{
		ToolName = TEXT("");
		ToolDescription = TEXT("");
		Category = EDAEditorToolCategory::Custom;
		AccessLevel = EDAEditorToolAccess::Public;
		ToolClass = nullptr;
		bAutoRegister = true;
	}
};