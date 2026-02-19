// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
#include "MenuItem.generated.h"

USTRUCT()
struct FMenuItem
{
    GENERATED_BODY()
	
	FString Name;
	
	TFunction<FString()> getValue = nullptr;
	
	TFunction<void()> action = nullptr;
	
	TArray<FMenuItem> subMenu;
	
	bool isBack = false;
	

	
	FMenuItem() : Name(""), isBack(false) {}

	FMenuItem(const FString& _name, TFunction<FString()> _getValue = nullptr, TFunction<void()> _action = nullptr, bool _backItem = false)
	{
		Name = _name;
		getValue = _getValue;
		action = _action;
		isBack = _backItem;
	}
};
