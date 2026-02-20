// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

#include "MenuLineWidget.generated.h"

class UDebugMenuItem;

/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UMenuLineWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelText = nullptr;
	UPROPERTY(EditAnywhere, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText = nullptr;
	UPROPERTY(EditAnywhere, Category = "Border", meta = (BindWidget))
	TObjectPtr<UBorder> Border = nullptr;

	UPROPERTY(EditAnywhere)
	FLinearColor BorderColorSelected = FLinearColor::Red;
	UPROPERTY(EditAnywhere)
	FLinearColor BorderColorNotSelected = FLinearColor::White;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDebugMenuItem> itemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isSelected = false;
	
	virtual void NativeConstruct() override;


	public:
	UFUNCTION(BlueprintCallable)
	void SetValues(UDebugMenuItem* _itemData, const bool _isSelected)
	{
		itemData = _itemData;
		isSelected = _isSelected;
		ChangeValueText();
	}

	UFUNCTION(BlueprintCallable)
	void ChangeValueText();
	UFUNCTION(BlueprintCallable)
	void SetColor();

	void ResetWidget()
	{
		itemData = nullptr;
		isSelected = false;

		if (LabelText) LabelText->SetText(FText::GetEmpty());
		if (ValueText) ValueText->SetText(FText::GetEmpty());
		
		if (Border) Border->SetContentColorAndOpacity(BorderColorNotSelected);
	}
};
