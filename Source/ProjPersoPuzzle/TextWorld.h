// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TextWorld.generated.h"

/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UTextWorld : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Text", meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock = nullptr;

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetTextInTextBlock(const FString& _text);
};
