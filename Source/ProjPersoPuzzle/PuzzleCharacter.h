// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <InputMappingContext.h>
#include "EnhancedInputComponent.h"
#include <EnhancedInputSubsystems.h>

#include "GrabComponent.h"
#include "HookComponent.h"
#include "PuzzleCharacter.generated.h"

class UInteractComponent;
UCLASS()
class PROJPERSOPUZZLE_API APuzzleCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGrabComponent> grabComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UInteractComponent> interactComponent = nullptr;

	UPROPERTY(VisibleAnywhere) TObjectPtr<APlayerController> playerController = nullptr;
	UPROPERTY(VisibleAnywhere) TObjectPtr<APlayerCameraManager> playerCameraManager = nullptr;

	// UPROPERTY(EditAnywhere) float lineTraceDist = 200.f;



	UPROPERTY(EditAnywhere) TObjectPtr<UHookComponent> hookComponent = nullptr;

#pragma region Camera

	UPROPERTY(EditAnywhere)
	USceneComponent* cameraRoot;

	UPROPERTY(EditAnywhere) TObjectPtr<UCameraComponent> camera = nullptr;
	//UPROPERTY(EditAnywhere) TObjectPtr<USpringArmComponent> springArm = nullptr;

#pragma endregion 

#pragma region Inputs
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> mapping = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> moveAction = nullptr;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> lookAction = nullptr;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> jumpAction = nullptr;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> grabAction = nullptr;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> hookAction = nullptr;

#pragma endregion 

#pragma region Movement

	UPROPERTY(EditAnywhere, Category = "Movement") float moveSpeed = 50.f;
	UPROPERTY(EditAnywhere, Category = "Movement") float lookSpeed = 50.f;

#pragma endregion 

public:

	APuzzleCharacter();

public:
	FORCEINLINE TObjectPtr<APlayerCameraManager> GetPlayerCameraManager() const { return playerCameraManager; };
	FORCEINLINE TObjectPtr<UCameraComponent> GetPlayerCamera() const { return camera; };
	FORCEINLINE TObjectPtr<UInteractComponent> GetInteractComponent() const { return interactComponent; };
	FORCEINLINE TObjectPtr<UGrabComponent> GetGrabComponent() const { return grabComponent; };
	void AddMapping();
	void RemoveMapping();
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


#pragma region Inputs
#pragma region Movement
	UFUNCTION() void Move(const FInputActionValue& _value);

	UFUNCTION() void Look(const FInputActionValue& _value);
#pragma endregion 

#pragma endregion 


};
