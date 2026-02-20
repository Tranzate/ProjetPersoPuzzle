// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "TextBoxUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UTextBoxUserWidget : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNameValidated, FString, name);
	
	UPROPERTY(EditAnywhere, Category = "Buttons", meta = (BindWidget)) TObjectPtr<UButton> Button_Approve = nullptr;
	UPROPERTY(EditAnywhere, Category = "Buttons", meta = (BindWidget)) TObjectPtr<UButton> Button_Cancel = nullptr;
	UPROPERTY(EditAnywhere, Category = "TextBox", meta = (BindWidget)) TObjectPtr<UEditableTextBox> EditableTextBox = nullptr;
	UPROPERTY(EditAnywhere, Category = "TextBlock", meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock_Error = nullptr;

	UPROPERTY()TArray<FString> nameToCheck = TArray<FString>();
	
protected:
	virtual void NativeConstruct() override;
	void InitBinding();
	UFUNCTION() void OnPlayButton_Approve();
	UFUNCTION() void OnPlayButton_Cancel();
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnNameValidated OnNameValidated;
	void SetNameToCheck(TArray<FString> _nameToCheck) {nameToCheck = _nameToCheck;}

	bool CheckNameToCheck();
};
