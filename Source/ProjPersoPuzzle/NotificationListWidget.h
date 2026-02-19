#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationListWidget.generated.h"

class UNotificationWidget;
class UVerticalBox;

UCLASS()
class PROJPERSOPUZZLE_API UNotificationListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddNotification(const FString& _message);
	
	UVerticalBox* GetHiddenBox() const { return NotificationBox_Hidden; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* NotificationBox_Show;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* NotificationBox_Hidden;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 maxNotificationsCount = 5;
    
	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<UNotificationWidget> notificationWidgetClass;
};