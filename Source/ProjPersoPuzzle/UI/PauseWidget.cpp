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

void UPauseWidget::ShowMenu()
{
	SetVisibility(ESlateVisibility::Visible);

	UWorld* _world = GetWorld();
	if (!_world) return;

	APlayerController* _pc = _world->GetFirstPlayerController();
	if (_pc)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		_pc->SetInputMode(InputMode);
		_pc->bShowMouseCursor = true;
	}

	UGameplayStatics::SetGamePaused(_world, true);
}

void UPauseWidget::HideMenu()
{
	LOG("Hide");
	UWorld* _world = GetWorld();
	if (!_world) return;

	APlayerController* _pc = _world->GetFirstPlayerController();
	if (_pc)
	{
		FInputModeGameOnly InputMode;
		_pc->SetInputMode(InputMode);
		_pc->bShowMouseCursor = false;
	}

	UGameplayStatics::SetGamePaused(_world, false);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseWidget::OnResumeClicked()
{
	LOG("RESUME");
	HideMenu();
}

void UPauseWidget::OnQuitClicked()
{
	LOG("QUit");
	UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
