// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleCharacter.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>

#include "InteractComponent.h"
#include "InteractibleActor.h"

APuzzleCharacter::APuzzleCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	cameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	cameraRoot->SetupAttachment(RootComponent);

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(cameraRoot);
	camera->bUsePawnControlRotation = true;

	grabComponent = CreateDefaultSubobject<UGrabComponent>(TEXT("GrabComponent"));
	interactComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	hookComponent = CreateDefaultSubobject<UHookComponent>(TEXT("HookComponent"));


	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//GetCharacterMovement()->bOrientRotationToMovement = false;
}


void APuzzleCharacter::BeginPlay()
{
	Super::BeginPlay();

	playerController = Cast<APlayerController>(GetController());
	if (playerController)
		playerCameraManager = playerController->PlayerCameraManager;

	if (playerCameraManager && grabComponent)
		grabComponent->SetPlayerCameraManager(playerCameraManager);
}

void APuzzleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma region Inputs

void APuzzleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	TObjectPtr<ULocalPlayer> _localPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (!_localPlayer) return;
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> _inputSystem = _localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!_inputSystem) return;
	_inputSystem->AddMappingContext(mapping, 0);

	TObjectPtr<UEnhancedInputComponent> _input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!_input) return;

	_input->BindAction(moveAction, ETriggerEvent::Triggered, this, &APuzzleCharacter::Move);
	_input->BindAction(lookAction, ETriggerEvent::Triggered, this, &APuzzleCharacter::Look);
	_input->BindAction(jumpAction, ETriggerEvent::Started, this, &APuzzleCharacter::Jump);

	_input->BindAction(grabAction, ETriggerEvent::Started, interactComponent.Get(), &UInteractComponent::Interact);
	_input->BindAction(hookAction, ETriggerEvent::Triggered, hookComponent.Get(), &UHookComponent::StartHookPressed);
	_input->BindAction(hookAction, ETriggerEvent::Completed , hookComponent.Get(), &UHookComponent::OnHookRelease);


}



#pragma region Movement

void APuzzleCharacter::Move(const FInputActionValue& _value)
{
	const FVector2D _movementValue = _value.Get<FVector2D>();

	if (Controller)
	{
		FRotator _controlRot = Controller->GetControlRotation();
		_controlRot.Pitch = 0.f;

		const FVector _forwardDir = UKismetMathLibrary::GetForwardVector(_controlRot);
		const FVector _rightDir = UKismetMathLibrary::GetRightVector(_controlRot);

		const FVector _fwdMovement = _forwardDir * _movementValue.Y * moveSpeed * GetWorld()->DeltaTimeSeconds;
		const FVector _rgtMovement = _rightDir * _movementValue.X * moveSpeed * GetWorld()->DeltaTimeSeconds;

		AddMovementInput(_fwdMovement + _rgtMovement);
	}
}

void APuzzleCharacter::Look(const FInputActionValue& _value)
{
	const FVector2D& _rotateValue = _value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("Your message"));
	AddControllerYawInput(_rotateValue.X * lookSpeed * GetWorld()->DeltaTimeSeconds);
	AddControllerPitchInput(-_rotateValue.Y * lookSpeed * GetWorld()->DeltaTimeSeconds);


}

#pragma endregion


void APuzzleCharacter::AddMapping()
{
	
	if (APlayerController* _pc = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* _lp = _pc->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* _sub = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (mapping)
				{
					_sub->AddMappingContext(mapping, 10);
				}
			}
		}
	}
	
}

void APuzzleCharacter::RemoveMapping()
{
	if (APlayerController* _pc = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* _lp = _pc->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* _sub = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				_sub->RemoveMappingContext(mapping);
			}
		}
	}
}


#pragma endregion
