// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PauseWidget.h"

#include "Utility.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseWidget::OnResumeClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPauseWidget::OnQuitClicked);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

// void UPauseWidget::ShowMenu()
// {
// 	SetVisibility(ESlateVisibility::Visible);
//
// 	UWorld* _world = GetWorld();
// 	if (!_world) return;
//
// 	APlayerController* _pc = _world->GetFirstPlayerController();
// 	if (_pc)
// 	{
// 		FInputModeGameAndUI InputMode;
// 		InputMode.SetWidgetToFocus(this->TakeWidget());
// 		_pc->SetInputMode(InputMode);
// 		_pc->bShowMouseCursor = true;
// 	}
//
// 	UGameplayStatics::SetGamePaused(_world, true);
// }

void UPauseWidget::ShowMenu()
{
	SetVisibility(ESlateVisibility::Visible);
	UWorld* _world = GetWorld();
	APlayerController* _pc = _world->GetFirstPlayerController();

	if (_pc)
	{
		FInputModeGameAndUI _inputMode;
		_inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
       
		_pc->SetInputMode(_inputMode);
		_pc->bShowMouseCursor = true;
	}

	UGameplayStatics::SetGamePaused(_world, true);
}

void UPauseWidget::HideMenu()
{
   
	APlayerController* _pc = GetWorld()->GetFirstPlayerController();
	if (_pc)
	{
		FInputModeGameOnly _inputMode;
		_pc->SetInputMode(_inputMode);
		_pc->bShowMouseCursor = false;
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseWidget::OnResumeClicked()
{
	HideMenu();
}

void UPauseWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
