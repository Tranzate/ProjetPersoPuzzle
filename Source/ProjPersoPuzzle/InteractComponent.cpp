#include "InteractComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractibleActor.h"
#include "GrabComponent.h"
#include "PuzzleCharacter.h"
#include "Utility.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	owner = Cast<APuzzleCharacter>(GetOwner());
	if (owner)
	{
		cameraPlayer = owner->GetPlayerCamera();
		grabComponent = owner->GetGrabComponent();
	}
}

void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformInteractionCheck();
}

void UInteractComponent::PerformInteractionCheck()
{
	if (!canLineTraceTick) return;

	AActor* _hitActor = GetActorInLookDirection();
	if (currentActorCibling != _hitActor)
	{
		UpdateCurrentTarget(_hitActor);
	}
}

AActor* UInteractComponent::GetActorInLookDirection() const
{
	if (!cameraPlayer) return nullptr;

	FVector _start = cameraPlayer->GetComponentLocation();
	FVector _end = _start + cameraPlayer->GetForwardVector() * lineTraceDist;
	FHitResult _outHit;

	bool _hit = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		_start,
		_end,
		objectTypes,
		false,
		{owner},
		EDrawDebugTrace::None,
		_outHit,
		true
	);

	return _hit ? _outHit.GetActor() : nullptr;
}

void UInteractComponent::UpdateCurrentTarget(AActor* _newTarget)
{
	if (currentActorCibling)
	{
		ProcessInteractibleFeedback(currentActorCibling, false);
	}

	currentActorCibling = _newTarget;

	if (currentActorCibling)
	{
		ProcessInteractibleFeedback(currentActorCibling, true);
	}
}

void UInteractComponent::ProcessInteractibleFeedback(AActor* _target, bool _showFeedback)
{
	if (AInteractibleActor* _interactible = Cast<AInteractibleActor>(_target))
	{
		if (_showFeedback)
		{
			_interactible->ShowInteractionUI();
		}
		else
		{
			_interactible->HideInteractionUI();
		}
	}
}

void UInteractComponent::SetCanLineTraceTick(bool _value)
{
	canLineTraceTick = _value;

	if (!canLineTraceTick && currentActorCibling)
	{
		ProcessInteractibleFeedback(currentActorCibling, false);
		currentActorCibling = nullptr;
	}
}

void UInteractComponent::Interact(const FInputActionValue& Value)
{
	if (grabComponent)
	{
		grabComponent->GrabOrRelease();
	}

	if (AInteractibleActor* _actor = Cast<AInteractibleActor>(currentActorCibling))
		_actor->Interact(owner);
}
