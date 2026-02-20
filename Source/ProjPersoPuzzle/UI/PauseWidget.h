// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

/**
 * 
 */

class UButton;
class UInGameWidget;
class AInGameHUD;
 
UCLASS()
class PROJPERSOPUZZLE_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY()
	TObjectPtr<UInGameWidget> inGameWidget = nullptr;

	UPROPERTY()
	TObjectPtr<AInGameHUD> inGameHUD = nullptr;

	UPROPERTY()
	bool isOpen = false;
	
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnQuitClicked();

public:
	void ShowMenu();
	void HideMenu();

	void SetInGameWidget(TObjectPtr<UInGameWidget> _inGameWidget) {inGameWidget = _inGameWidget;};
	void SetHudOwner(TObjectPtr<AInGameHUD> _inGameHud) {inGameHUD = _inGameHud;};

	bool GetIsOpen() {return isOpen;};

	
};
