// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HookPointSubsystem.generated.h"

class AHookPoint;


UCLASS(Blueprintable)
class PROJPERSOPUZZLE_API UHookPointSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TArray<TObjectPtr<AHookPoint>> allHookPoint = TArray<TObjectPtr<AHookPoint>>();

public:
	void RegisterPoint(TObjectPtr<AHookPoint> _point);
	void UnRegisterPoint(TObjectPtr<AHookPoint> _point);

	UFUNCTION(BlueprintCallable, Category = "Hook")
	FORCEINLINE TArray<AHookPoint*> GetAllPoint() { return allHookPoint; }

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

};


