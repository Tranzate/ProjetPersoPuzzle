// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ConsoleButtonEditorUtilityWidget.generated.h"

class USizeBox;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UConsoleButtonEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	// --- Widgets Bindings (Doivent avoir le même nom dans l'UMG) ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText;

	// --- Paramètres du Bouton ---
	UPROPERTY(EditAnywhere, Category = "Console Button|Appearance")
	FText buttonName = FText::FromString("Execute Command");

	UPROPERTY(EditAnywhere, Category = "Console Button|Appearance")
	float buttonHeight = 25.f;

	UPROPERTY(EditAnywhere, Category = "Console Button|Appearance")
	float buttonWidth = 150.f;

	UPROPERTY(EditAnywhere, Category = "Console Button|Appearance")
	bool invertColor = false;

	UPROPERTY(EditAnywhere, Category = "Console Button|Logic")
	FString consoleCommand = FString("");

	UPROPERTY(EditAnywhere, Category = "Console Button|Logic")
	bool commandHasIntState = false;

	UPROPERTY(EditAnywhere, Category = "Console Button|Logic", meta = (EditCondition = "commandHasIntState"))
	int intValue = 1;

protected:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ButtonClicked();
};