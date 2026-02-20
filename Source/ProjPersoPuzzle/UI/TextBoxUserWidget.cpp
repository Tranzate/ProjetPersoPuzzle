// Fill out your copyright notice in the Description page of Project Settings.


#include "TextBoxUserWidget.h"

#include "Utility.h"

void UTextBoxUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	if (EditableTextBox)
	{
		EditableTextBox->SetKeyboardFocus();
	}
	TObjectPtr<APlayerController> _pc = GetWorld()->GetFirstPlayerController();
	if (_pc)
	{
		_pc->bShowMouseCursor = true;
		FInputModeUIOnly _inputMode;
		_pc->SetInputMode(_inputMode);
	}
	InitBinding();
}

void UTextBoxUserWidget::InitBinding()
{
	if (Button_Approve)
		Button_Approve->OnClicked.AddDynamic(this, &UTextBoxUserWidget::OnPlayButton_Approve);
	if (Button_Cancel)
		Button_Cancel->OnClicked.AddDynamic(this, &UTextBoxUserWidget::OnPlayButton_Cancel);
	if (EditableTextBox)
		EditableTextBox->OnTextCommitted.AddDynamic(this, &UTextBoxUserWidget::OnTextCommitted);
}

void UTextBoxUserWidget::OnPlayButton_Approve()
{
	if (!EditableTextBox) return;

	const FString _enteredText = EditableTextBox->GetText().ToString();

	if (!CheckNameToCheck())
	{
		OnNameValidated.Broadcast(_enteredText);
		TObjectPtr<APlayerController> _pc = GetWorld()->GetFirstPlayerController();
		if (_pc)
		{
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
			FInputModeGameOnly  _inputMode;
			_pc->SetInputMode(_inputMode);
		}
		RemoveFromParent();
	}
	else
	{
		if (EditableTextBox)
		{
			EditableTextBox->SetKeyboardFocus();
		}
	}
}

void UTextBoxUserWidget::OnPlayButton_Cancel()
{
	TObjectPtr<APlayerController> _pc = GetWorld()->GetFirstPlayerController();
	if (_pc)
	{
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		FInputModeGameOnly  _inputMode;
		_pc->SetInputMode(_inputMode);
	}
	RemoveFromParent();
}

void UTextBoxUserWidget::OnTextCommitted(const FText& _text, ETextCommit::Type _commitType)
{
	if (_commitType == ETextCommit::OnEnter)
	{
		if (!CheckNameToCheck())
		{
			OnNameValidated.Broadcast(_text.ToString());
			TObjectPtr<APlayerController> _pc = GetWorld()->GetFirstPlayerController();
			if (_pc)
			{
				GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
				FInputModeGameOnly  _inputMode;
				_pc->SetInputMode(_inputMode);
			}
			RemoveFromParent();
		}
		else
		{
			if (EditableTextBox)
			{
				EditableTextBox->SetKeyboardFocus();
			}
		}
	}
}

bool UTextBoxUserWidget::CheckNameToCheck()
{
	if (!EditableTextBox) return true;

	FString _input = EditableTextBox->GetText().ToString();

	if (_input.IsEmpty())
	{
		TextBlock_Error->SetText(FText::FromString(TEXT("Name cannot be empty")));
		TextBlock_Error->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	for (int32 i = 0; i < _input.Len(); ++i)
	{
		TCHAR _c = _input[i];

		bool _isValid = FChar::IsAlnum(_c) || _c == ' ' || _c == '_';

		if (!_isValid)
		{
			TextBlock_Error->SetText(FText::FromString(TEXT("Special characters not allowed")));
			TextBlock_Error->SetVisibility(ESlateVisibility::Visible);
			return true;
		}
	}

	if (nameToCheck.Contains(_input))
	{
		TextBlock_Error->SetText(FText::FromString(TEXT("Name already exists")));
		TextBlock_Error->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	TextBlock_Error->SetVisibility(ESlateVisibility::Hidden);
	return false;
}
