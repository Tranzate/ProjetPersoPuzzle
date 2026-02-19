#include "NotificationListWidget.h"
#include "NotificationWidget.h"
#include "Components/VerticalBox.h"

void UNotificationListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!notificationWidgetClass) return;
	
	for (int i = 0; i < maxNotificationsCount; i++)
	{
		UNotificationWidget* _notif = CreateWidget<UNotificationWidget>(this, notificationWidgetClass);
		if (_notif)
		{
			_notif->ParentPool = this;
			NotificationBox_Hidden->AddChild(_notif);
		}
	}
}

void UNotificationListWidget::AddNotification(const FString& _message)
{
	if (!NotificationBox_Hidden || !NotificationBox_Show) return;

	UNotificationWidget* _notifToUse = nullptr;

	if (NotificationBox_Hidden->GetChildrenCount() > 0)
	{
		_notifToUse = Cast<UNotificationWidget>(NotificationBox_Hidden->GetChildAt(0));
	}
	else if (NotificationBox_Show->GetChildrenCount() > 0)
	{
		_notifToUse = Cast<UNotificationWidget>(NotificationBox_Show->GetChildAt(0));
		NotificationBox_Show->RemoveChild(_notifToUse);
	}

	if (_notifToUse)
	{
		NotificationBox_Show->AddChild(_notifToUse);
		
		_notifToUse->SetMessage(_message);
		NotificationBox_Show->InvalidateLayoutAndVolatility();
	}
}