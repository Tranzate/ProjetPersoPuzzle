// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuLineWidget.h"
#include "DebugMenuItem.h"
#include "Utility.h"

void UMenuLineWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMenuLineWidget::ChangeValueText()
{
	if (!itemData)
	{
		LabelText->SetText(FText::FromString(""));
		ValueText->SetText(FText::FromString(""));
		return;

	}
	LabelText->SetText(FText::FromString(itemData->Label));
	const int _count = itemData->ActionLabels.Num();
	
	if (_count <= 0)
	{
		ValueText->SetText(FText::FromString(itemData->DynamicValue));
	}
	else
	{
		if (_count > 1)
		{
			FString _string = itemData->ActionLabels[itemData->CurrentActionIndex];
			FString _finalString = "<"+_string+">";
			ValueText->SetText(FText::FromString(_finalString));
		}
		else
		{
			FString _string = itemData->ActionLabels[itemData->CurrentActionIndex];
			ValueText->SetText(FText::FromString(_string));
		}
			
	}
	SetColor();
}

void UMenuLineWidget::SetColor()
{
	FLinearColor _color = isSelected ?BorderColorSelected : BorderColorNotSelected;

	Border->SetContentColorAndOpacity(_color);
}
