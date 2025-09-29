// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/DAEditorToolBase.h"
#include "DarkAge.h"
#include "Tools/DAEditorToolManager.h"

UDAEditorToolBase::UDAEditorToolBase()
{
	ToolName = TEXT("Base Editor Tool");
	ToolDescription = TEXT("Base class for editor tools.");
	ToolCategory = EDAEditorToolCategory::Custom;
	AccessLevel = EDAEditorToolAccess::Public;
	bIsModal = false;
	bAutoSave = false;
	AutoSaveInterval = 300.0f;
	ToolState = EDAEditorToolState::Inactive;
	bHasUnsavedChanges = false;
}

bool UDAEditorToolBase::Initialize(UDAEditorToolManager* InManager)
{
	ToolManager = InManager;
	OnToolInitialized();
	return true;
}

void UDAEditorToolBase::Activate()
{
	SetToolState(EDAEditorToolState::Active);
	OnToolActivated();
}

void UDAEditorToolBase::Deactivate()
{
	SetToolState(EDAEditorToolState::Inactive);
	OnToolDeactivated();
}

void UDAEditorToolBase::Shutdown()
{
	UnregisterFromEventBus();
}

void UDAEditorToolBase::Tick(float DeltaTime)
{
	if (IsActive())
	{
		OnToolTick(DeltaTime);
		HandleAutoSave(DeltaTime);
	}
}

bool UDAEditorToolBase::SaveToolData()
{
	FString SavedData = OnSaveToolData();
	if (!SavedData.IsEmpty())
	{
		// Placeholder for saving data
		ClearModifiedFlag();
		return true;
	}
	return false;
}

bool UDAEditorToolBase::LoadToolData()
{
	FString SavedData; // Placeholder for loading data
	return OnLoadToolData(SavedData);
}

FString UDAEditorToolBase::GetToolDataAsJSON()
{
	return OnSaveToolData();
}

bool UDAEditorToolBase::SetToolDataFromJSON(const FString& JSONData)
{
	return OnLoadToolData(JSONData);
}

UDAEditorToolWidget* UDAEditorToolBase::CreateToolWidget()
{
	if (ToolWidgetClass)
	{
		if (UWorld* World = GetWorld())
		{
			ToolWidget = CreateWidget<UDAEditorToolWidget>(World, ToolWidgetClass);
			if(ToolWidget)
			{
				ToolWidget->InitializeWidget(this);
			}
		}
	}
	return ToolWidget;
}

void UDAEditorToolBase::RefreshUI()
{
	if (ToolWidget)
	{
		ToolWidget->RefreshWidget();
	}
}

TArray<EGlobalEventType> UDAEditorToolBase::GetListenedEventTypes() const
{
	return TArray<EGlobalEventType>();
}

void UDAEditorToolBase::SetToolState(EDAEditorToolState NewState)
{
	if (ToolState != NewState)
	{
		ToolState = NewState;
		OnStateChanged.Broadcast(NewState);
	}
}

void UDAEditorToolBase::RegisterWithEventBus()
{
	// Implementation to register with GlobalEventBus
}

void UDAEditorToolBase::UnregisterFromEventBus()
{
	// Implementation to unregister from GlobalEventBus
}

void UDAEditorToolBase::HandleAutoSave(float DeltaTime)
{
	if (bAutoSave && IsActive() && HasUnsavedChanges())
	{
		AutoSaveTimer += DeltaTime;
		if (AutoSaveTimer >= AutoSaveInterval)
		{
			SaveToolData();
			AutoSaveTimer = 0.0f;
		}
	}
}

void UDAEditorToolBase::OnGlobalEvent(const FGlobalEvent& Event)
{
	OnGlobalEventReceived(Event);
}