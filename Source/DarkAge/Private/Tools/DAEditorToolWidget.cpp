// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/DAEditorToolBase.h"
#include "DarkAge.h"

void UDAEditorToolWidget::InitializeWidget(UDAEditorToolBase* InTool)
{
	AssociatedTool = InTool;
	OnWidgetInitialized();
}

void UDAEditorToolWidget::RefreshWidget()
{
	OnWidgetRefresh();
}

void UDAEditorToolWidget::OnToolStateChanged(EDAEditorToolState NewState)
{
	OnAssociatedToolStateChanged(NewState);
}