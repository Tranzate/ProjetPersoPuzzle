// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivableActor.generated.h"

UCLASS(Abstract)
class PROJPERSOPUZZLE_API AActivableActor : public AActor
{
	GENERATED_BODY()

public:
	AActivableActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, CallInEditor)
	virtual void OnActivate(AActor* _other) {}
	UFUNCTION(BlueprintCallable, CallInEditor)
	virtual void OnDeActivate(AActor* _other){} 
};
