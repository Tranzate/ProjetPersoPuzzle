// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActivableActor.h"

#include "Door.generated.h"

UCLASS()
class PROJPERSOPUZZLE_API ADoor : public AActivableActor
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnActivate(AActor* _other) override;
	virtual void OnDeActivate(AActor* _other) override;
	

	float EaseOutBounce(float _x);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> leftDoor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> rightDoor;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float maxSlideDistance = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float totalDuration = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
	bool targetOpening = false;
	UPROPERTY(VisibleAnywhere, Category = "Door Settings")
	float elapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	bool isMoving = false;

	FVector initialLeftPos = FVector::ZeroVector;
	FVector initialRightPos = FVector::ZeroVector;

	float currentStartOffset = 0.0f;
};
