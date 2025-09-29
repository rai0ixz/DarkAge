#include "UI/DAResponseOptionWidget.h"
#include "Components/Button.h"

void UDAResponseOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResponseButton)
	{
		ResponseButton->OnClicked.AddDynamic(this, &UDAResponseOptionWidget::OnButtonClicked);
	}
}

void UDAResponseOptionWidget::OnButtonClicked()
{
	OnResponseOptionClicked.Broadcast(ResponseIndex);
}