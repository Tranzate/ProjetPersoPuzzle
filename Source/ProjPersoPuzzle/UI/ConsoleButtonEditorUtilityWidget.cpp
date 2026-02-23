// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ConsoleButtonEditorUtilityWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Editor.h"
#include "Kismet/KismetSystemLibrary.h"



void UConsoleButtonEditorUtilityWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (SizeBox)
	{
		SizeBox->SetWidthOverride(buttonWidth);
		SizeBox->SetHeightOverride(buttonHeight);
	}
	
	if (ButtonText)
	{
		ButtonText->SetText(buttonName);
	}
	
	if (Button)
	{
		FLinearColor _color = invertColor ? FLinearColor::Black : FLinearColor::White;
		Button->SetBackgroundColor(_color);
        
		if (ButtonText)
		{
			ButtonText->SetColorAndOpacity(invertColor ? FLinearColor::White : FLinearColor::Black);
		}
	}
}

void UConsoleButtonEditorUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button)
	{
		Button->OnClicked.RemoveDynamic(this, &UConsoleButtonEditorUtilityWidget::ButtonClicked);
		Button->OnClicked.AddDynamic(this, &UConsoleButtonEditorUtilityWidget::ButtonClicked);
	}
}

void UConsoleButtonEditorUtilityWidget::ButtonClicked()
{
	if (consoleCommand.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ConsoleButton: Aucune commande définie !"));
		return;
	}
	
	FString _finalCommand = consoleCommand;
	if (consoleCommand.IsEmpty()) return;
	

	if (commandHasIntState)
	{
		intValue = (intValue == 0) ? 1 : 0;
        
		_finalCommand += FString::Printf(TEXT(" %d"), intValue);
		UE_LOG(LogTemp, Log, TEXT("Toggle : %s est maintenant à %d"), *consoleCommand, intValue);
	}
	UWorld* _targetWorld = nullptr;


	if (GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				_targetWorld = Context.World();
				break;
			}
		}
	}

	if (!_targetWorld)
	{
		_targetWorld = GetWorld();
	}

	if (_targetWorld)
	{
		if (APlayerController* _pc = _targetWorld->GetFirstPlayerController())
		{
			_pc->ConsoleCommand(_finalCommand);
			UE_LOG(LogTemp, Log, TEXT("Commande exécutée sur le PlayerController : %s"), *_finalCommand);
		}
		else
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(_targetWorld, _finalCommand);
			UE_LOG(LogTemp, Log, TEXT("Commande exécutée via SystemLibrary : %s"), *_finalCommand);
		}
	}
}