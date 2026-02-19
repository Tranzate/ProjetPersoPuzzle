// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/CapsuleComponent.h>

#include "InteractComponent.h"
#include "PuzzleCharacter.h"
#include "Utility.h"


UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	physicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}


void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* _owner = GetOwner())
	{
		owner = Cast<APuzzleCharacter>(_owner);
		if (owner)
		{

			camera = owner->GetPlayerCamera();
			interactComponent = owner->GetInteractComponent();
			cameraManager = owner->GetPlayerCameraManager();
		}
	}
}

void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (physicsHandle && physicsHandle->GrabbedComponent && camera)
	{
		UPrimitiveComponent* _heldComp = physicsHandle->GrabbedComponent;

		FVector _cubeLoc = _heldComp->GetComponentLocation();

		FVector _targetLoc = camera->GetComponentLocation() + camera->GetForwardVector() * holdDistance;

		if (grabbedActor)
		{
			if (FVector::Dist(_cubeLoc, _targetLoc) > holdDistance)
				GrabOrRelease();
		}

		physicsHandle->SetTargetLocationAndRotation(_targetLoc, camera->GetComponentRotation());

		_heldComp->SetLinearDamping(5.f);
		_heldComp->SetAngularDamping(5.f);
	}
}

bool UGrabComponent::TraceForPhysicsBody(FHitResult& _hitResult) const
{
	if (!camera) return false;

	FVector _start = camera->GetComponentLocation();
	FVector _end = _start + camera->GetForwardVector() * holdDistance;

	FCollisionQueryParams _params;
	_params.AddIgnoredActor(owner);

	return GetWorld()->LineTraceSingleByChannel(_hitResult, _start, _end, ECC_PhysicsBody, _params);
}

void UGrabComponent::GrabOrRelease()
{
	if (!physicsHandle || !camera || !owner || !cameraManager || !interactComponent) return;
	if (camera->GetRelativeRotation().Pitch <= -50) return;

	if (physicsHandle->GrabbedComponent)
	{
		UPrimitiveComponent* _held = physicsHandle->GrabbedComponent;
		_held->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

		physicsHandle->ReleaseComponent();


		cameraManager->ViewPitchMax = 90.f;
		cameraManager->ViewPitchMin = -90.f;


		grabbedActor = nullptr;


		interactComponent->SetCanLineTraceTick(true);
	}
	else
	{
		FHitResult _hit;
		if (TraceForPhysicsBody(_hit))
		{
			UPrimitiveComponent* _hitComp = _hit.GetComponent();

			if (_hitComp && _hitComp->IsSimulatingPhysics())
			{
				physicsHandle->GrabComponentAtLocation(_hitComp, NAME_None, _hitComp->GetComponentLocation());

				cameraManager->ViewPitchMax = 45.f;
				cameraManager->ViewPitchMin = -45.f;


				grabbedActor = _hitComp->GetOwner();


				interactComponent->SetCanLineTraceTick(false);
			}
		}
	}
}

void UGrabComponent::Throw(float Force)
{
}
