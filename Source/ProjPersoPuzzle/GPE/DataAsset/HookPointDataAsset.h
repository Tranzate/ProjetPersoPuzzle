// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HookPointDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJPERSOPUZZLE_API UHookPointDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Debug",
		meta = (ToolTip = "Enable to show a debug sphere representing the hook point range in the editor and game."))
	bool debugHookPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings",
		meta = (ToolTip = "Widget displayed when the player is near the hook point."))
	TSubclassOf<UUserWidget> hookUIClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings",
		meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0",
			ToolTip = "Delay in seconds before the hook point can be reused, to prevent rapid spamming."))
	float delayTimerForSpam = 2.f;
};
