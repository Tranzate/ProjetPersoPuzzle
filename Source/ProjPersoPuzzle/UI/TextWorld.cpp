// Fill out your copyright notice in the Description page of Project Settings.


#include "TextWorld.h"

void UTextWorld::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTextWorld::SetTextInTextBlock(const FString& _text)
{
	TextBlock->SetText(FText::FromString(_text));
}


