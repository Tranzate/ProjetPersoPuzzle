#include "NodeElevator.h"
#include "GPE/Elevator.h"
#include "GPE/Door.h"
#include "GPE/ElevatorButton.h"

#if WITH_EDITOR
#include "Editor.h"
#include "EditorActorFolders.h"
#endif

ANodeElevator::ANodeElevator()
{
	currentFloorName = TEXT("RDC");
}

FVector ANodeElevator::ComputeDoorPosition(float _floorZ) const
{
	if (!targetedElevator) return FVector::ZeroVector;
	const FQuat   _rot    = targetedElevator->GetActorQuat();
	const FVector _pos    = targetedElevator->GetActorLocation();
	const FVector _offset = _rot.RotateVector(FVector(floorDoorOffset.X, floorDoorOffset.Y, 0.f));
	return FVector(_pos.X + _offset.X, _pos.Y + _offset.Y, _floorZ + floorDoorOffset.Z);
}

FRotator ANodeElevator::ComputeDoorRotation() const
{
	if (!targetedElevator) return floorDoorRotation;
	return (targetedElevator->GetActorQuat() * FQuat(floorDoorRotation)).Rotator();
}

FVector ANodeElevator::ComputeCallButtonPosition(float _floorZ) const
{
	if (!targetedElevator) return FVector::ZeroVector;
	const FQuat   _rot    = targetedElevator->GetActorQuat();
	const FVector _pos    = targetedElevator->GetActorLocation();
	const FVector _offset = _rot.RotateVector(FVector(callButtonOffset.X, callButtonOffset.Y, 0.f));
	return FVector(_pos.X + _offset.X, _pos.Y + _offset.Y, _floorZ + callButtonOffset.Z);
}

FRotator ANodeElevator::ComputeCallButtonRotation() const
{
	if (!targetedElevator) return callButtonRotation;
	return (targetedElevator->GetActorQuat() * FQuat(callButtonRotation)).Rotator();
}


void ANodeElevator::BeginPlay()
{
	Super::BeginPlay();
	
	FDetachmentTransformRules _detachRules(EDetachmentRule::KeepWorld, false);
	for (TPair<FString, ADoor*>& _pair : floorDoors)
		if (IsValid(_pair.Value)) _pair.Value->DetachFromActor(_detachRules);

	for (TPair<FString, AElevatorButton*>& _pair : callButtons)
		if (IsValid(_pair.Value)) _pair.Value->DetachFromActor(_detachRules);

	GetWorld()->GetTimerManager().SetTimer(initTimerHandle, [this]()
	{
		if (!targetedElevator) return;

		targetedElevator->nodeElevator = this;
		if (targetedElevator->doorComponent)
			targetedElevator->door = Cast<ADoor>(targetedElevator->doorComponent->GetChildActor());

		if (targetedElevator->door)
			targetedElevator->door->OnDoorFinishMovement.AddDynamic(this, &ANodeElevator::MoveElevator);

		GenerateButtons();

	}, 0.1f, false);
}

void ANodeElevator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(initTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(nextFloorTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(queueTimerHandle);
	Super::EndPlay(EndPlayReason);
}


void ANodeElevator::SpawnFloorDoors()
{
	if (!floorDoorClass)     { UE_LOG(LogTemp, Error,   TEXT("[NodeElevator] floorDoorClass non assignee !")); return; }
	if (!targetedElevator)   { UE_LOG(LogTemp, Error,   TEXT("[NodeElevator] targetedElevator non assigne !")); return; }
	if (floorData.Num() == 0){ UE_LOG(LogTemp, Warning, TEXT("[NodeElevator] floorData est vide.")); return; }

	DestroyFloorDoors();

	const FRotator _rot = ComputeDoorRotation();

#if WITH_EDITOR
	const FString _elevatorName  = targetedElevator->GetActorLabel();
	const FName   _rootFolder    = FName(*FString::Printf(TEXT("Elevator_%s"), *_elevatorName));
	const FName   _doorFolder    = FName(*FString::Printf(TEXT("Elevator_%s/Portes"), *_elevatorName));
	FActorFolders::Get().CreateFolder(*GetWorld(), _rootFolder);
	FActorFolders::Get().CreateFolder(*GetWorld(), _doorFolder);
	targetedElevator->SetFolderPath(_rootFolder);
	this->SetFolderPath(_rootFolder);
#endif

	for (const TPair<FString, float>& _floor : floorData)
	{
		FActorSpawnParameters _params;
		_params.Owner = this;
		_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADoor* _door = GetWorld()->SpawnActor<ADoor>(
			floorDoorClass, ComputeDoorPosition(_floor.Value), _rot, _params);

		if (_door)
		{
#if WITH_EDITOR
			_door->SetFolderPath(_doorFolder);
			_door->SetActorLabel(FString::Printf(TEXT("Porte_%s"), *_floor.Key));
#endif
			floorDoors.Add(_floor.Key, _door);
			UE_LOG(LogTemp, Log, TEXT("[NodeElevator] Porte '%s' spawnee."), *_floor.Key);
		}
	}
}

void ANodeElevator::DestroyFloorDoors()
{
	int _count = 0;
	for (TPair<FString, ADoor*>& _pair : floorDoors)
		if (IsValid(_pair.Value)) { _pair.Value->Destroy(); _count++; }
	floorDoors.Empty();
	UE_LOG(LogTemp, Log, TEXT("[NodeElevator] %d porte(s) supprimee(s)."), _count);
}


void ANodeElevator::SpawnCallButtons()
{
	if (!buttonClass)        { UE_LOG(LogTemp, Error,   TEXT("[NodeElevator] buttonClass non assignee !")); return; }
	if (!targetedElevator)   { UE_LOG(LogTemp, Error,   TEXT("[NodeElevator] targetedElevator non assigne !")); return; }
	if (floorData.Num() == 0){ UE_LOG(LogTemp, Warning, TEXT("[NodeElevator] floorData est vide.")); return; }

	DestroyCallButtons();

	const FRotator _rot = ComputeCallButtonRotation();

#if WITH_EDITOR
	const FString _elevatorName = targetedElevator->GetActorLabel();
	const FName   _rootFolder   = FName(*FString::Printf(TEXT("Elevator_%s"), *_elevatorName));
	const FName   _btnFolder    = FName(*FString::Printf(TEXT("Elevator_%s/BoutonsCall"), *_elevatorName));
	FActorFolders::Get().CreateFolder(*GetWorld(), _rootFolder);
	FActorFolders::Get().CreateFolder(*GetWorld(), _btnFolder);
	targetedElevator->SetFolderPath(_rootFolder);
	this->SetFolderPath(_rootFolder);
#endif

	for (const TPair<FString, float>& _floor : floorData)
	{
		FActorSpawnParameters _params;
		_params.Owner = this;
		_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AElevatorButton* _btn = GetWorld()->SpawnActor<AElevatorButton>(
			TSubclassOf<AElevatorButton>(buttonClass),
			ComputeCallButtonPosition(_floor.Value), _rot, _params);

		if (_btn)
		{
			_btn->parentNode    = this;
			_btn->isCallButton  = true;
			_btn->callFloorName = _floor.Key;
			_btn->UpdateText(_floor.Key);
#if WITH_EDITOR
			_btn->SetFolderPath(_btnFolder);
			_btn->SetActorLabel(FString::Printf(TEXT("BoutonCall_%s"), *_floor.Key));
#endif
			callButtons.Add(_floor.Key, _btn);
			UE_LOG(LogTemp, Log, TEXT("[NodeElevator] Call button '%s' spawne."), *_floor.Key);
		}
	}
}

void ANodeElevator::DestroyCallButtons()
{
	int _count = 0;
	for (TPair<FString, AElevatorButton*>& _pair : callButtons)
		if (IsValid(_pair.Value)) { _pair.Value->Destroy(); _count++; }
	callButtons.Empty();
	UE_LOG(LogTemp, Log, TEXT("[NodeElevator] %d call button(s) supprime(s)."), _count);
}


void ANodeElevator::MoveToTestFloor()
{
	if (!targetedElevator)
	{
		UE_LOG(LogTemp, Error, TEXT("[NodeElevator] MoveToTestFloor: targetedElevator non assigne !"));
		return;
	}
	if (testFloorName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[NodeElevator] MoveToTestFloor: testFloorName est vide. Entre un nom d'etage."));
		return;
	}
	if (!floorData.Contains(testFloorName))
	{
		UE_LOG(LogTemp, Error, TEXT("[NodeElevator] etage '%s' introuvable. Etages disponibles :"), *testFloorName);
		for (const TPair<FString, float>& _p : floorData)
			UE_LOG(LogTemp, Warning, TEXT("  -> '%s' = %.1f"), *_p.Key, _p.Value);
		return;
	}

	const float _targetZ = floorData[testFloorName];
	FVector _newPos      = targetedElevator->GetActorLocation();
	_newPos.Z            = _targetZ;
	targetedElevator->SetActorLocation(_newPos);

	UE_LOG(LogTemp, Log, TEXT("[NodeElevator] Ascenseur -> '%s' (Z=%.1f)"), *testFloorName, _targetZ);
}

#if WITH_EDITOR
void ANodeElevator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!targetedElevator) return;
	
	const FRotator _doorRot = ComputeDoorRotation();
	for (TPair<FString, ADoor*>& _pair : floorDoors)
	{
		if (!IsValid(_pair.Value) || !floorData.Contains(_pair.Key)) continue;
		_pair.Value->SetActorLocation(ComputeDoorPosition(floorData[_pair.Key]));
		_pair.Value->SetActorRotation(_doorRot);
	}
	
	const FRotator _btnRot = ComputeCallButtonRotation();
	for (TPair<FString, AElevatorButton*>& _pair : callButtons)
	{
		if (!IsValid(_pair.Value) || !floorData.Contains(_pair.Key)) continue;
		_pair.Value->SetActorLocation(ComputeCallButtonPosition(floorData[_pair.Key]));
		_pair.Value->SetActorRotation(_btnRot);
	}
}
#endif

void ANodeElevator::GenerateButtons()
{
	if (!targetedElevator || !targetedElevator->buttonPanelAnchor || !buttonClass) return;

	const FVector  _anchorLocation = targetedElevator->buttonPanelAnchor->GetComponentLocation();
	const FRotator _anchorRotation = targetedElevator->buttonPanelAnchor->GetComponentRotation();
	const FVector  _up    = targetedElevator->buttonPanelAnchor->GetUpVector();
	const FVector  _right = targetedElevator->buttonPanelAnchor->GetRightVector();
	const int      _cols  = FMath::Max(1, buttonColumns);

	int _index = 0;
	for (const TPair<FString, float>& _elem : floorData)
	{
		const int _col = _index % _cols;
		const int _row = _index / _cols;

		const FVector _spawnLocation = _anchorLocation
			+ _right * (_col * buttonSpacingH)
			+ _up    * (_row * buttonSpacingV);

		FActorSpawnParameters _spawnParams;
		_spawnParams.Owner = this;

		AElevatorButton* _newButton = GetWorld()->SpawnActor<AElevatorButton>(
			TSubclassOf<AElevatorButton>(buttonClass), _spawnLocation, _anchorRotation, _spawnParams);

		if (_newButton)
		{
			FAttachmentTransformRules _attachRules(EAttachmentRule::KeepWorld, false);
			_newButton->AttachToComponent(targetedElevator->buttonPanelAnchor, _attachRules);
			_newButton->parentNode      = this;
			_newButton->targetFloorName = _elem.Key;
			_newButton->isCallButton    = false;
			_newButton->UpdateText(_elem.Key);
		}
		_index++;
	}
}


void ANodeElevator::OpenAllDoorsAtFloor(FString _floorName)
{
	if (targetedElevator) targetedElevator->OpenDoor();
	if (floorDoors.Contains(_floorName) && floorDoors[_floorName])
		floorDoors[_floorName]->OnActivate(this);
}

void ANodeElevator::CloseAllDoorsAtFloor(FString _floorName)
{
	if (targetedElevator) targetedElevator->CloseDoor();
	if (floorDoors.Contains(_floorName) && floorDoors[_floorName])
		floorDoors[_floorName]->OnDeActivate(this);
}


void ANodeElevator::NotifyArrival()
{
	if (targetedElevator)
	{
		const float _currentZ = targetedElevator->GetActorLocation().Z;
		for (const TPair<FString, float>& _pair : floorData)
		{
			if (FMath::Abs(_pair.Value - _currentZ) < 5.0f)
			{
				currentFloorName = _pair.Key;
				break;
			}
		}
	}

	OpenAllDoorsAtFloor(currentFloorName);

	const float _waitTime = targetedElevator ? targetedElevator->GetWaitTime() : 3.0f;
	GetWorld()->GetTimerManager().SetTimer(
		queueTimerHandle, this, &ANodeElevator::ProcessNextFloor, _waitTime, false);
}

void ANodeElevator::ProcessNextFloor()
{
	if (floorQueue.Num() == 0 || !targetedElevator)
	{
		if (targetedElevator)
		{
			GetWorld()->GetTimerManager().SetTimer(nextFloorTimerHandle, [this]()
			{
				CloseAllDoorsAtFloor(currentFloorName);
			}, targetedElevator->GetWaitTime(), false);
		}
		isProcessingQueue = false;
		return;
	}

	isProcessingQueue = true;
	const FString _nextFloor = floorQueue[0];

	if (!floorData.Contains(_nextFloor))
	{
		floorQueue.RemoveAt(0);
		ProcessNextFloor();
		return;
	}

	const float _targetHeight  = floorData[_nextFloor];
	const float _currentHeight = targetedElevator->GetActorLocation().Z;

	if (FMath::Abs(_currentHeight - _targetHeight) < 5.0f)
	{
		floorQueue.RemoveAt(0);
		OpenAllDoorsAtFloor(_nextFloor);
		GetWorld()->GetTimerManager().SetTimer(
			nextFloorTimerHandle, this, &ANodeElevator::ProcessNextFloor,
			targetedElevator->GetWaitTime(), false);
	}
	else
	{
		pendingHeight = _targetHeight;
		isCallPending = true;
		CloseAllDoorsAtFloor(currentFloorName);
	}
}

void ANodeElevator::OnFloorSelected(FString _floorName)
{
	if (!targetedElevator || !floorData.Contains(_floorName)) return;
	if (floorQueue.Contains(_floorName)) return;

	const float _targetHeight  = floorData[_floorName];
	const float _currentHeight = targetedElevator->GetActorLocation().Z;

	if (FMath::Abs(_currentHeight - _targetHeight) < 5.0f)
	{
		currentFloorName = _floorName;
		OpenAllDoorsAtFloor(_floorName);
		return;
	}

	floorQueue.Add(_floorName);
	if (!isProcessingQueue)
		ProcessNextFloor();
}

void ANodeElevator::CallToHeight(float _height)
{
	pendingHeight = _height;
	isCallPending = true;
	if (targetedElevator) targetedElevator->CloseDoor();
}

void ANodeElevator::MoveElevator(bool _targetOpening)
{
	if (!_targetOpening && isCallPending)
	{
		if (!targetedElevator) return;
		targetedElevator->Configure(moveSpeed, easingFamily, easingDirection);
		targetedElevator->MoveToHeight(pendingHeight);
		isCallPending = false;
	}
}
