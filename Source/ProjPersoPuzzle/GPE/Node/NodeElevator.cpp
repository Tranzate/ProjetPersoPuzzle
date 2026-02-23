#include "NodeElevator.h"
#include "GPE/Elevator.h"
#include "GPE/Door.h"
#include "GPE/ElevatorButton.h"

ANodeElevator::ANodeElevator()
{
	currentFloorName = TEXT("RDC");
}

void ANodeElevator::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle _timerHandle;
	GetWorld()->GetTimerManager().SetTimer(_timerHandle, [this]()
	{
		if (targetedElevator)
		{
			targetedElevator->nodeElevator = this;
			if (targetedElevator->doorComponent)
			{
				targetedElevator->door = Cast<ADoor>(targetedElevator->doorComponent->GetChildActor());
			}
			if (targetedElevator->door)
			{
				targetedElevator->door->OnDoorFinishMovement.AddDynamic(this, &ANodeElevator::MoveElevator);
			}

			GenerateButtons();
		}
	}, 0.1f, false);
}

void ANodeElevator::GenerateButtons()
{
	int _index = 0;
	for (const TPair<FString, float>& _elem : floorData)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		AElevatorButton* _newButton = GetWorld()->SpawnActor<AElevatorButton>(
			buttonClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (_newButton)
		{
			FAttachmentTransformRules _attachRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				false
			);

			_newButton->AttachToComponent(targetedElevator->buttonPanelAnchor, _attachRules);

			_newButton->SetActorRelativeLocation(FVector(0.0f, _index * 25.0f, 0.0f));

			_newButton->parentNode = this;
			_newButton->targetFloorName = _elem.Key;
			_newButton->isCallButton = false;
			_newButton->UpdateText(_elem.Key);
		}
		else
		{
		}
		_index++;
	}
}

void ANodeElevator::OpenAllDoorsAtFloor(FString _floorName)
{
	if (targetedElevator) targetedElevator->OpenDoor();

	if (floorDoors.Contains(_floorName) && floorDoors[_floorName])
	{
		floorDoors[_floorName]->OnActivate(this);
	}
}

void ANodeElevator::CloseAllDoorsAtFloor(FString _floorName)
{
	if (targetedElevator) targetedElevator->CloseDoor();

	if (floorDoors.Contains(_floorName) && floorDoors[_floorName])
	{
		floorDoors[_floorName]->OnDeActivate(this);
	}
}

void ANodeElevator::ProcessNextFloor()
{
	if (floorQueue.Num() == 0 || !targetedElevator)
	{
		isProcessingQueue = false;
		return;
	}

	isProcessingQueue = true;
	FString _nextFloor = floorQueue[0];

	if (floorData.Contains(_nextFloor))
	{
		float _targetHeight = floorData[_nextFloor];
		float _currentHeight = targetedElevator->GetActorLocation().Z;

		if (FMath::Abs(_currentHeight - _targetHeight) < 5.0f)
		{
			OpenAllDoorsAtFloor(_nextFloor);
			floorQueue.RemoveAt(0);

			FTimerHandle _nextFloorTimer;
			GetWorld()->GetTimerManager().
			            SetTimer(_nextFloorTimer, this, &ANodeElevator::ProcessNextFloor, 3.0f, false);
		}
		else
		{
			pendingHeight = _targetHeight;
			isCallPending = true;
			UE_LOG(LogTemp, Warning, TEXT("Fermeture des portes à l'étage : %s"), *currentFloorName);
			CloseAllDoorsAtFloor(currentFloorName);
		}
	}
}

void ANodeElevator::OnFloorSelected(FString _floorName)
{
	if (floorQueue.Contains(_floorName)) return;

	float _targetHeight = floorData[_floorName];
	float _currentHeight = targetedElevator->GetActorLocation().Z;

	if (FMath::Abs(_currentHeight - _targetHeight) < 5.0f)
	{
		currentFloorName = _floorName;
		OpenAllDoorsAtFloor(_floorName);
		return;
	}

	floorQueue.Add(_floorName);
	if (!isProcessingQueue)
	{
		ProcessNextFloor();
	}
}

void ANodeElevator::CallToHeight(float _height)
{
	pendingHeight = _height;
	isCallPending = true;
	targetedElevator->CloseDoor();
}

void ANodeElevator::MoveElevator(bool _targetOpening)
{
	if (!_targetOpening && isCallPending)
	{
		if (floorQueue.Num() > 0)
		{
			currentFloorName = floorQueue[0];
			floorQueue.RemoveAt(0);
		}

		targetedElevator->MoveToHeight(pendingHeight);
		isCallPending = false;
		UE_LOG(LogTemp, Warning, TEXT("Départ vers l'étage : %s"), *currentFloorName);
	}
	else if (_targetOpening)
	{
		FTimerHandle _queueTimer;
		GetWorld()->GetTimerManager().SetTimer(_queueTimer, this, &ANodeElevator::ProcessNextFloor, 4.0f, false);
	}
}
