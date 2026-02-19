#include "NotificationWidget.h"
#include "NotificationListWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Components/VerticalBox.h"

void UNotificationWidget::SetMessage(const FString& _message)
{
	if (Show) StopAnimation(Show);
	if (Hide) StopAnimation(Hide);
	GetWorld()->GetTimerManager().ClearTimer(hideTimerHandle);
	UnbindAllFromAnimationFinished(Hide);
	
	SetRenderOpacity(1.0f);
	SetRenderTransform(FWidgetTransform());
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (NotificationText)
	{
		NotificationText->SetText(FText::FromString(_message));
	}
	
	if (Show)
	{
		PlayAnimation(Show);
		float _delayBeforeHide = Show->GetEndTime() + 2.0f;
		GetWorld()->GetTimerManager().SetTimer(hideTimerHandle, this, &UNotificationWidget::StartHideTimer, _delayBeforeHide, false);
	}
}

void UNotificationWidget::StartHideTimer()
{
	if (Hide)
	{
		FWidgetAnimationDynamicEvent _endEvent;
		_endEvent.BindDynamic(this, &UNotificationWidget::OnHideFinished);
		BindToAnimationFinished(Hide, _endEvent);
        
		PlayAnimation(Hide);
	}
	else
	{
		OnHideFinished();
	}
}

void UNotificationWidget::OnHideFinished()
{
	UnbindAllFromAnimationFinished(Hide);

	if (ParentPool)
	{
		if (UVerticalBox* _hiddenBox = ParentPool->GetHiddenBox())
		{
			_hiddenBox->AddChild(this);
			SetRenderOpacity(1.0f); 

		}
	}
	else
	{
		RemoveFromParent();
	}
}