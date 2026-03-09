#include "Elevator.h"
#include "Door.h"
#include "Node/NodeElevator.h"

float AElevator::GetWaitTime() const
{
	return nodeElevator ? nodeElevator->waitTimeAtFloor : 3.f;
}

AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;
	elevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorMesh"));
	RootComponent = elevatorMesh;

	buttonPanelAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ButtonPanelAnchor"));
	buttonPanelAnchor->SetupAttachment(RootComponent);

	doorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Door"));
	doorComponent->SetupAttachment(RootComponent);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	startZ  = GetActorLocation().Z;
	targetZ = startZ;
	door    = Cast<ADoor>(doorComponent->GetChildActor());
}

void AElevator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(waitTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (!isMoving) return;

	elapsedTime += DeltaTime;
	
	const float _t = (moveDuration > 0.f)
		? FMath::Clamp(elapsedTime / moveDuration, 0.f, 1.f)
		: 1.f;
	
	const float _easedT = FEasing::Evaluate(easingFamily, easingDirection, _t);
	
	FVector _loc = GetActorLocation();
	_loc.Z = FMath::Lerp(startZ, targetZ, _easedT);
	SetActorLocation(_loc);
	
	if (_t >= 1.f)
	{
		_loc.Z = targetZ;
		SetActorLocation(_loc);
		isMoving         = false;
		isWaitingAtFloor = true;

		if (nodeElevator)
			nodeElevator->NotifyArrival();
		else
			OpenDoor();
	}
}

void AElevator::MoveToHeight(float _height)
{
	startZ       = GetActorLocation().Z;
	targetZ      = _height;
	elapsedTime  = 0.f;
	
	const float _distance = FMath::Abs(targetZ - startZ);
	moveDuration  = (moveSpeed > 0.f) ? (_distance / moveSpeed) : 0.01f;

	isMoving       = true;
	isWaitingAtFloor = false;
}

void AElevator::NotifyArrival()
{
	isWaitingAtFloor = true;
}

void AElevator::CloseDoor() { if (door) door->OnDeActivate(this); }
void AElevator::OpenDoor()  { if (door) door->OnActivate(this); }
