// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

class UDebugMenuWidget;
class UNotificationListWidget;
/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UNotificationListWidget* NotificationList;

	UPROPERTY(meta = (BindWidget))
	UDebugMenuWidget* DebugMenuWidget;

public:
	UFUNCTION(BlueprintCallable) UDebugMenuWidget* GetDebugMenuWidget() {return DebugMenuWidget;}
	UFUNCTION(BlueprintCallable) UNotificationListWidget* GetNotificationListWidget() {return NotificationList;}
	
	
};
