#include "Elevator.h"
#include "Door.h"
#include "Node/NodeElevator.h"

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
	targetZ = GetActorLocation().Z;
	door = Cast<ADoor>(doorComponent->GetChildActor());
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (isMoving)
	{
		FVector _currentLocation = GetActorLocation();
       
		if (FMath::IsNearlyEqual(_currentLocation.Z, targetZ, 1.0f))
		{
			_currentLocation.Z = targetZ;
			SetActorLocation(_currentLocation);
			isMoving = false;
			
			if (nodeElevator)
			{
				nodeElevator->OpenAllDoorsAtFloor(nodeElevator->GetCurrentFloorName());
			}
			else 
			{
				OpenDoor();
			}
		}
		else
		{
			float NewZ = FMath::FInterpTo(_currentLocation.Z, targetZ, DeltaTime, moveSpeed);
			_currentLocation.Z = NewZ;
			SetActorLocation(_currentLocation);
		}
	}
}

void AElevator::MoveToHeight(float _height) { targetZ = _height; isMoving = true; }
void AElevator::CloseDoor() { if(door) door->OnDeActivate(this); }
void AElevator::OpenDoor() { if(door) door->OnActivate(this); }