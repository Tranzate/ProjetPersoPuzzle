// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"


class UInteractComponent;
class UPhysicsHandleComponent;
class UCameraComponent;
class APuzzleCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJPERSOPUZZLE_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere) TObjectPtr<UPhysicsHandleComponent> physicsHandle = nullptr;

	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> camera = nullptr;
	UPROPERTY(VisibleAnywhere) TObjectPtr<APlayerCameraManager> cameraManager = nullptr;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UInteractComponent> interactComponent = nullptr;

	UPROPERTY(EditAnywhere) float holdDistance = 200.f;

	UPROPERTY(EditAnywhere) float throwStrength = 1500.f;

	UPROPERTY(VisibleAnywhere) TObjectPtr<AActor> grabbedActor = nullptr;

	UPROPERTY(VisibleAnywhere) TObjectPtr<APuzzleCharacter> owner = nullptr;
public:	

	UGrabComponent();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TraceForPhysicsBody(FHitResult& _hitResult) const;

public:
	UFUNCTION() void GrabOrRelease();
	UFUNCTION() void Throw(float _force = 1000.f);

	FORCEINLINE void SetPlayerCameraManager( TObjectPtr<APlayerCameraManager> _cameraManager)  {  cameraManager = _cameraManager; }


		
};
