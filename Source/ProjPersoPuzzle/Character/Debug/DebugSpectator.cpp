#include "DebugSpectator.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"

ADebugSpectator::ADebugSpectator()
{
	if (UFloatingPawnMovement* FloatingMoveComp = Cast<UFloatingPawnMovement>(GetMovementComponent()))
	{
		FloatingMoveComp->MaxSpeed = 5000.f;
		FloatingMoveComp->Acceleration = 10000.f;
		FloatingMoveComp->Deceleration = 10000.f;
	}
}

void ADebugSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* _e = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (moveAction && lookAction && flyAction) 
		{
			_e->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADebugSpectator::Move);
			_e->BindAction(lookAction, ETriggerEvent::Triggered, this, &ADebugSpectator::Look);
			_e->BindAction(flyAction, ETriggerEvent::Triggered, this, &ADebugSpectator::Fly);
		}

	}
}

void ADebugSpectator::Move(const FInputActionValue& Value)
{
	FVector2D _value = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator _rotation = Controller->GetControlRotation();
        
		const FVector _forwardDirection = FRotationMatrix(_rotation).GetUnitAxis(EAxis::X);
		const FVector _rightDirection = FRotationMatrix(_rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(_forwardDirection, _value.Y);
		AddMovementInput(_rightDirection, _value.X);
	}
}

void ADebugSpectator::Look(const FInputActionValue& Value)
{
	FVector2D _value = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(_value.X);
		AddControllerPitchInput(-_value.Y);
	}
}

void ADebugSpectator::Fly(const FInputActionValue& Value)
{
	float _value = Value.Get<float>();
	AddMovementInput(FVector::UpVector, _value);
}



void ADebugSpectator::NotifyRestarted()
{
	Super::NotifyRestarted();

	if (APlayerController* _pc = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* _lp = _pc->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* _sub = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (spectatorImc)
				{
					_sub->AddMappingContext(spectatorImc, 10);
				}
			}
		}
	}
}

void ADebugSpectator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveMapping();
	Super::EndPlay(EndPlayReason);
}

void ADebugSpectator::RemoveMapping()
{
	if (APlayerController* _pc = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* _lp = _pc->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* _sub = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				_sub->RemoveMappingContext(spectatorImc);
			}
		}
	}
}
