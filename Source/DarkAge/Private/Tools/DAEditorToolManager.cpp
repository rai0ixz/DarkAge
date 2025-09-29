// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/DAEditorToolManager.h"
#include "DarkAge.h"

#if WITH_EDITOR

UDAEditorToolManager::UDAEditorToolManager()
{
	bAutoSaveEnabled = true;
	AutoSaveInterval = 300.0f;
	bDebugModeEnabled = false;
	bShowToolManagerOnStartup = true;
}

void UDAEditorToolManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterWithEventBus();
	AutoRegisterTools();
	if (bShowToolManagerOnStartup)
	{
		ShowToolManagerUI();
	}
}

void UDAEditorToolManager::Deinitialize()
{
	ShutdownAllTools();
	UnregisterFromEventBus();
	Super::Deinitialize();
}

TArray<EGlobalEventType> UDAEditorToolManager::GetListenedEventTypes() const
{
	return TArray<EGlobalEventType>();
}

bool UDAEditorToolManager::RegisterToolClass(TSubclassOf<UDAEditorToolBase> ToolClass, const FDAEditorToolInfo& ToolInfo)
{
	// Implementation
	return false;
}

bool UDAEditorToolManager::RegisterTool(UDAEditorToolBase* Tool)
{
	if (Tool && !RegisteredTools.Contains(Tool))
	{
		RegisteredTools.Add(Tool);
		Tool->Initialize(this);
		OnToolRegistered.Broadcast(Tool);
		return true;
	}
	return false;
}

bool UDAEditorToolManager::UnregisterTool(UDAEditorToolBase* Tool)
{
	if (Tool && RegisteredTools.Contains(Tool))
	{
		Tool->Shutdown();
		RegisteredTools.Remove(Tool);
		OnToolUnregistered.Broadcast(Tool);
		return true;
	}
	return false;
}

UDAEditorToolBase* UDAEditorToolManager::CreateTool(TSubclassOf<UDAEditorToolBase> ToolClass)
{
	if (ToolClass)
	{
		UDAEditorToolBase* NewTool = NewObject<UDAEditorToolBase>(this, ToolClass);
		RegisterTool(NewTool);
		return NewTool;
	}
	return nullptr;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetAllTools() const
{
	return RegisteredTools;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsByCategory(EDAEditorToolCategory Category) const
{
	TArray<UDAEditorToolBase*> Tools;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->GetToolCategory() == Category)
		{
			Tools.Add(Tool);
		}
	}
	return Tools;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsByAccessLevel(EDAEditorToolAccess AccessLevel) const
{
	TArray<UDAEditorToolBase*> Tools;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->GetAccessLevel() == AccessLevel)
		{
			Tools.Add(Tool);
		}
	}
	return Tools;
}

UDAEditorToolBase* UDAEditorToolManager::FindToolByName(const FString& ToolName) const
{
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->GetToolName() == ToolName)
		{
			return Tool;
		}
	}
	return nullptr;
}

UDAEditorToolBase* UDAEditorToolManager::FindToolByClass(TSubclassOf<UDAEditorToolBase> ToolClass) const
{
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->GetClass() == ToolClass)
		{
			return Tool;
		}
	}
	return nullptr;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetActiveTools() const
{
	TArray<UDAEditorToolBase*> ActiveTools;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->IsActive())
		{
			ActiveTools.Add(Tool);
		}
	}
	return ActiveTools;
}

bool UDAEditorToolManager::ActivateTool(UDAEditorToolBase* Tool)
{
	if (CanActivateTool(Tool))
	{
		SetActiveTool(Tool);
		Tool->Activate();
		return true;
	}
	return false;
}

bool UDAEditorToolManager::ActivateToolByName(const FString& ToolName)
{
	UDAEditorToolBase* Tool = FindToolByName(ToolName);
	return ActivateTool(Tool);
}

bool UDAEditorToolManager::DeactivateTool(UDAEditorToolBase* Tool)
{
	if (Tool && Tool->IsActive())
	{
		Tool->Deactivate();
		if (ActiveTool == Tool)
		{
			SetActiveTool(nullptr);
		}
		return true;
	}
	return false;
}

void UDAEditorToolManager::DeactivateAllTools()
{
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		DeactivateTool(Tool);
	}
}

bool UDAEditorToolManager::ShutdownTool(UDAEditorToolBase* Tool)
{
	return UnregisterTool(Tool);
}

void UDAEditorToolManager::ShutdownAllTools()
{
	TArray<UDAEditorToolBase*> ToolsToShutdown = RegisteredTools;
	for (UDAEditorToolBase* Tool : ToolsToShutdown)
	{
		UnregisterTool(Tool);
	}
}

void UDAEditorToolManager::ShowToolManagerUI()
{
	if (ToolManagerWidgetClass && !ToolManagerWidget)
	{
		if (UWorld* World = GetWorld())
		{
			ToolManagerWidget = CreateWidget<UUserWidget>(World, ToolManagerWidgetClass);
		}
	}
	if (ToolManagerWidget && !ToolManagerWidget->IsInViewport())
	{
		ToolManagerWidget->AddToViewport();
	}
}

void UDAEditorToolManager::HideToolManagerUI()
{
	if (ToolManagerWidget && ToolManagerWidget->IsInViewport())
	{
		ToolManagerWidget->RemoveFromParent();
	}
}

void UDAEditorToolManager::ToggleToolManagerUI()
{
	if (ToolManagerWidget && ToolManagerWidget->IsInViewport())
	{
		HideToolManagerUI();
	}
	else
	{
		ShowToolManagerUI();
	}
}

void UDAEditorToolManager::RefreshAllToolUIs()
{
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool)
		{
			Tool->RefreshUI();
		}
	}
}

bool UDAEditorToolManager::SaveAllToolData()
{
	bool bAllSaved = true;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->HasUnsavedChanges())
		{
			if (!Tool->SaveToolData())
			{
				bAllSaved = false;
			}
		}
	}
	return bAllSaved;
}

bool UDAEditorToolManager::LoadAllToolData()
{
	bool bAllLoaded = true;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool)
		{
			if (!Tool->LoadToolData())
			{
				bAllLoaded = false;
			}
		}
	}
	return bAllLoaded;
}

bool UDAEditorToolManager::SaveToolLayout()
{
	return false;
}

bool UDAEditorToolManager::LoadToolLayout()
{
	return false;
}

bool UDAEditorToolManager::HasUnsavedChanges() const
{
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->HasUnsavedChanges())
		{
			return true;
		}
	}
	return false;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsWithUnsavedChanges() const
{
	TArray<UDAEditorToolBase*> Tools;
	for (UDAEditorToolBase* Tool : RegisteredTools)
	{
		if (Tool && Tool->HasUnsavedChanges())
		{
			Tools.Add(Tool);
		}
	}
	return Tools;
}

void UDAEditorToolManager::SetAutoSaveEnabled(bool bEnabled)
{
	bAutoSaveEnabled = bEnabled;
}

void UDAEditorToolManager::SetAutoSaveInterval(float Interval)
{
	AutoSaveInterval = Interval;
}

void UDAEditorToolManager::SetDebugModeEnabled(bool bEnabled)
{
	bDebugModeEnabled = bEnabled;
}

UDAEditorToolManager* UDAEditorToolManager::Get(const UObject* WorldContext)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		return UGameInstance::GetSubsystem<UDAEditorToolManager>(World->GetGameInstance());
	}
	return nullptr;
}
void UDAEditorToolManager::RegisterWithEventBus() {}
void UDAEditorToolManager::UnregisterFromEventBus() {}
void UDAEditorToolManager::HandleAutoSave(float DeltaTime) {}
void UDAEditorToolManager::AutoRegisterTools() {}
void UDAEditorToolManager::SetActiveTool(UDAEditorToolBase* Tool)
{
	if (ActiveTool != Tool)
	{
		ActiveTool = Tool;
		OnActiveToolChanged.Broadcast(ActiveTool);
	}
}
bool UDAEditorToolManager::CanActivateTool(UDAEditorToolBase* Tool) const
{
	if (!Tool) return false;
	if (ActiveTool && ActiveTool->IsModal()) return false;
	return true;
}
void UDAEditorToolManager::OnToolStateChangedInternal(UDAEditorToolBase* Tool, EDAEditorToolState NewState)
{
	OnToolStateChanged.Broadcast(Tool, NewState);
}
void UDAEditorToolManager::OnGlobalEvent(const FGlobalEvent& Event) {}
void UDAEditorToolManager::OnToolStateChangedCallback(EDAEditorToolState NewState) {}
#else // !WITH_EDITOR

UDAEditorToolManager::UDAEditorToolManager()
{
	bAutoSaveEnabled = false;
	AutoSaveInterval = 300.0f;
	bDebugModeEnabled = false;
	bShowToolManagerOnStartup = false;
}

void UDAEditorToolManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDAEditorToolManager::Deinitialize()
{
	Super::Deinitialize();
}

TArray<EGlobalEventType> UDAEditorToolManager::GetListenedEventTypes() const
{
	return {};
}

bool UDAEditorToolManager::RegisterToolClass(TSubclassOf<UDAEditorToolBase> ToolClass, const FDAEditorToolInfo& ToolInfo)
{
	return false;
}

bool UDAEditorToolManager::RegisterTool(UDAEditorToolBase* Tool)
{
	return false;
}

bool UDAEditorToolManager::UnregisterTool(UDAEditorToolBase* Tool)
{
	return false;
}

UDAEditorToolBase* UDAEditorToolManager::CreateTool(TSubclassOf<UDAEditorToolBase> ToolClass)
{
	return nullptr;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetAllTools() const
{
	return {};
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsByCategory(EDAEditorToolCategory Category) const
{
	return {};
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsByAccessLevel(EDAEditorToolAccess AccessLevel) const
{
	return {};
}

UDAEditorToolBase* UDAEditorToolManager::FindToolByName(const FString& ToolName) const
{
	return nullptr;
}

UDAEditorToolBase* UDAEditorToolManager::FindToolByClass(TSubclassOf<UDAEditorToolBase> ToolClass) const
{
	return nullptr;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetActiveTools() const
{
	return {};
}

bool UDAEditorToolManager::ActivateTool(UDAEditorToolBase* Tool)
{
	return false;
}

bool UDAEditorToolManager::ActivateToolByName(const FString& ToolName)
{
	return false;
}

bool UDAEditorToolManager::DeactivateTool(UDAEditorToolBase* Tool)
{
	return false;
}

void UDAEditorToolManager::DeactivateAllTools()
{
}

bool UDAEditorToolManager::ShutdownTool(UDAEditorToolBase* Tool)
{
	return false;
}

void UDAEditorToolManager::ShutdownAllTools()
{
}

void UDAEditorToolManager::ShowToolManagerUI()
{
}

void UDAEditorToolManager::HideToolManagerUI()
{
}

void UDAEditorToolManager::ToggleToolManagerUI()
{
}

void UDAEditorToolManager::RefreshAllToolUIs()
{
}

bool UDAEditorToolManager::SaveAllToolData()
{
	return true;
}

bool UDAEditorToolManager::LoadAllToolData()
{
	return true;
}

bool UDAEditorToolManager::SaveToolLayout()
{
	return true;
}

bool UDAEditorToolManager::LoadToolLayout()
{
	return true;
}

bool UDAEditorToolManager::HasUnsavedChanges() const
{
	return false;
}

TArray<UDAEditorToolBase*> UDAEditorToolManager::GetToolsWithUnsavedChanges() const
{
	return {};
}

void UDAEditorToolManager::SetAutoSaveEnabled(bool bEnabled)
{
}

void UDAEditorToolManager::SetAutoSaveInterval(float Interval)
{
}

void UDAEditorToolManager::SetDebugModeEnabled(bool bEnabled)
{
}

UDAEditorToolManager* UDAEditorToolManager::Get(const UObject* WorldContext)
{
	return nullptr;
}

void UDAEditorToolManager::RegisterWithEventBus()
{
}

void UDAEditorToolManager::UnregisterFromEventBus()
{
}

void UDAEditorToolManager::HandleAutoSave(float DeltaTime)
{
}

void UDAEditorToolManager::AutoRegisterTools()
{
}

void UDAEditorToolManager::SetActiveTool(UDAEditorToolBase* Tool)
{
	if (ActiveTool != Tool)
	{
		ActiveTool = Tool;
		OnActiveToolChanged.Broadcast(ActiveTool);
	}
}

bool UDAEditorToolManager::CanActivateTool(UDAEditorToolBase* Tool) const
{
	return false;
}

void UDAEditorToolManager::OnToolStateChangedInternal(UDAEditorToolBase* Tool, EDAEditorToolState NewState)
{
	OnToolStateChanged.Broadcast(Tool, NewState);
}

void UDAEditorToolManager::OnGlobalEvent(const FGlobalEvent& Event)
{
}

void UDAEditorToolManager::OnToolStateChangedCallback(EDAEditorToolState NewState)
{
}

#endif // WITH_EDITOR