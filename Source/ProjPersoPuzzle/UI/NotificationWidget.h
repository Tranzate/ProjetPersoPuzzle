#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;
class UNotificationListWidget;

UCLASS()
class PROJPERSOPUZZLE_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Référence vers le pool pour le recyclage
	UPROPERTY()
	UNotificationListWidget* ParentPool = nullptr;

	void SetMessage(const FString& _message);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NotificationText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Show = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Hide = nullptr;
	
	FTimerHandle hideTimerHandle;

	void StartHideTimer();

	UFUNCTION()
	void OnHideFinished();
};