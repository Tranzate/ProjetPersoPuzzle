// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DebugMenuSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FWaypointData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString mapName;
};

UCLASS()
class PROJPERSOPUZZLE_API UDebugMenuSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY() TArray<FWaypointData> savedWaypoints;

	TArray<FString> GetAllName();
};
