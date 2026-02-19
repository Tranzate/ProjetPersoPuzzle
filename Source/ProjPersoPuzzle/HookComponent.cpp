// Fill out your copyright notice in the Description page of Project Settings.


#include "HookComponent.h"
#include "Utility.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <NiagaraFunctionLibrary.h>
#include "NiagaraComponent.h"
#include "PuzzleCharacter.h"



UHookComponent::UHookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHookComponent::BeginPlay()
{
	Super::BeginPlay();

	owner = Cast<APuzzleCharacter>(GetOwner());
	if (owner)
	{
		camOwner = owner->GetComponentByClass<UCameraComponent>();
		ownerMovementComponent = owner->GetCharacterMovement();
		if (ownerMovementComponent)
		{
			brakingDecelerationFallingBase = ownerMovementComponent->BrakingDecelerationFalling;
		}
	}

	if (UWorld* _world = GetWorld())
	{
		hookPointSubsystem = _world->GetSubsystem<UHookPointSubsystem>();
	}

	if (!CheckDataAsset())
	{
		SetComponentTickEnabled(false);
		return;
	}

	if (!owner || !camOwner || !ownerMovementComponent || !hookPointSubsystem)
	{
		LOG_ERROR("Missing dependencies (Character, Camera, Movement or Subsystem).");
		SetComponentTickEnabled(false);
		return;
	}

	currentHookInHand = hookDataAsset->hookInHandsBase;
}

void UHookComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(timerStateHook);
	CLEAR_TIMER(timerDespawnNiagara);
	CLEAR_TIMER(timerResetCharMovement);
}


void UHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestHookPoint();

	switch (currentState)
	{
	case EHookState::Swinging:
		SwingTick(DeltaTime);
		break;

	case EHookState::Idle:
	case EHookState::Propelling:
		break;
	}

	DrawDebug();
}

/// <summary>
///Recherche le point d'accroche le mieux adapté en scorant tous les HookPoints valides (portée, ...). Met à jour et affiche l'UI du point sélectionné.
/// </summary>
void UHookComponent::FindBestHookPoint()
{
	float _bestScore = -std::numeric_limits<float>::infinity();
	TObjectPtr<AHookPoint> _newBestPoint = nullptr;

	const FVector _cameraLocation = camOwner->GetComponentLocation();
	const FVector _cameraForward = camOwner->GetForwardVector();
	const FVector _ownerLocation = owner->GetActorLocation();

	const TArray<AHookPoint*> _allPointToCheck = hookPointSubsystem->GetAllPoint();

	for (AHookPoint* _hookToCheck : _allPointToCheck)
	{
		if (_hookToCheck == bestPointUsed)
			continue;

		if (IsHookPointValid(_hookToCheck, _cameraLocation, _ownerLocation))
		{
			const float _score = CalculateScoreForPoint(_hookToCheck, _cameraLocation, _cameraForward);


			if (_score > _bestScore)
			{
				_bestScore = _score;
				_newBestPoint = _hookToCheck;
			}
		}
	}


	ShowUI(_newBestPoint);
	bestPoint = _newBestPoint;
}

/// <summary>
/// Vérifie si un point d'accroche respecte toutes les contraintes: portée min/max, type de hook compatible et ligne de visée dégagée depuis la caméra.
/// </summary>
/// <param name="_hookPoint">Le point d'accroche à évaluer.</param>
/// <param name="_cameraLocation">La position de la caméra du joueur.</param>
/// <param name="_ownerLocation">La position du joueur.</param>
/// <returns>True si le point est valide, False sinon.</returns>
bool UHookComponent::IsHookPointValid(TObjectPtr<AHookPoint> _hookPoint, const FVector& _cameraLocation,
                                      const FVector& _ownerLocation)
{
	if (!_hookPoint)
		return false;

	if (!IsPointInRange(_hookPoint, _ownerLocation, hookDataAsset->distanceMin, hookDataAsset->distanceMaxDetection))
		return false;

	const bool _canDoPropulsion = _hookPoint->GetCanDoPropulsion();
	const EHookType _hookType = _hookPoint->GetHookToUse();

	if (_hookType != EHookType::All && !_canDoPropulsion)
		return false;

	if (!HasLineOfSight(_hookPoint->GetOffsetPosition(), _cameraLocation))
		return false;

	return true;
}

/// <summary>
/// Calcule un score basé sur le produit scalaire (alignement avec la caméra) et la distance. Un score plus élevé indique un point plus proche et mieux centré.
/// </summary>
/// <param name="_hookPoint">Le point d'accroche.</param>
/// <param name="_cameraLocation">La position de la caméra.</param>
/// <param name="_cameraForward">Le vecteur avant de la caméra.</param>
/// <returns>Le score de sélection. Retourne -infinity si le point est hors tolérance d'angle.</returns>
float UHookComponent::CalculateScoreForPoint(const AHookPoint* _hookPoint, const FVector& _cameraLocation,
                                             const FVector& _cameraForward) const
{
	const FVector _hookPointLocation = _hookPoint->GetOffsetPosition();
	FVector _dirToPoint = _hookPointLocation - _cameraLocation;

	const float _distance = _dirToPoint.Size();

	const FVector _normDir = _dirToPoint.GetSafeNormal();


	const float _dotProduct = FVector::DotProduct(_cameraForward, _normDir);

	if (_dotProduct <= hookDataAsset->hookAttachTolerenceDot)
		return -std::numeric_limits<float>::infinity();

	const float _score = (_dotProduct * hookDataAsset->scoreDotMultiplier) / FMath::Max(_distance, 1.f);

	return _score;
}

/// <summary>
/// Vérifie si la distance entre le point d'accroche et le joueur est comprise entre distanceMin et distanceMax
/// </summary>
/// <param name="_hookPoint">Le point d'accroche.</param>
/// <param name="_ownerLocation">La position du joueur.</param>
/// <returns>True si le point est dans la plage de distance valide, False sinon.</returns>
bool UHookComponent::IsPointInRange(const AHookPoint* _hookPoint, const FVector& _ownerLocation, const float _minDist,
                                    const float _maxDist) const
{
	if (!_hookPoint)
		return false;

	const FVector _hookPointLocation = _hookPoint->GetOffsetPosition();
	const double _dist = FVector::Dist(_ownerLocation, _hookPointLocation);


	if (_dist >= _maxDist)
	{

		return false;
	}
	else if (_dist <= _minDist)
	{
		return false;
	}

	return true;
}

/// <summary>
/// Effectue un Line Trace depuis la caméra jusqu'à la cible pour s'assurer qu'il n'y a pas d'obstruction entre le joueur et le point d'accroche.
/// </summary>
/// <param name="_targetLocation">La position du point d'accroche.</param>
/// <param name="_startLocation">La position de la caméra.</param>
/// <returns>True si la ligne de visée est dégagée ou si l'objet touché est le HookPoint, False sinon.</returns>
bool UHookComponent::HasLineOfSight(const FVector& _targetLocation, const FVector& _startLocation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes;
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<TObjectPtr<AActor>> _actorsToIgnore;
	_actorsToIgnore.Add(owner);

	FHitResult _hitResult;
	bool _bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		_startLocation,
		_targetLocation,
		_objectTypes,
		true,
		_actorsToIgnore,
		EDrawDebugTrace::None,
		_hitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1
	);

	if (!_bHit)
	{
		return true;
	}

	return Cast<AHookPoint>(_hitResult.GetActor()) != nullptr;
}

// <summary>
// [DÉPRÉCIÉE] Gère l'ancienne méthode de pression du grappin basée sur la durée de maintien, démarrant le swing si le seuil est dépassé.
// </summary>
/// <param name="_timeTriggeredSeconds">La durée de maintien de l'entrée.</param>
void UHookComponent::StartHookPressed_Deprecated(const float _timeTriggeredSeconds)
{
	if (currentState != EHookState::Idle || !bestPoint)
		return;

	bestPointUsed = bestPoint;
	hookPointLocation = bestPointUsed->GetOffsetPosition();

	if (_timeTriggeredSeconds > hookDataAsset->maxHoldToSwing)
	{
		StartSwing();
	}
}

/// <summary>
/// Gère l'entrée de pression du grappin. Si le bouton est maintenu au-delà du seuil 'maxHoldToSwing', déclenche le balancement (Swing).
/// </summary>
/// <param name="_instance">Instance de l'action d'entrée.</param>
void UHookComponent::StartHookPressed(const FInputActionInstance& _instance)
{
	if (currentState != EHookState::Idle || !bestPoint)
		return;

	const FVector _cameraLocation = camOwner->GetComponentLocation();
	const FVector _ownerLocation = owner->GetActorLocation();
	if (!IsHookPointValid(bestPoint, _cameraLocation, _ownerLocation))
	{
		bestPoint->ShowHookUI(false);
		bestPoint = nullptr;
		return;
	}


	// owner->SetStateOfBaihu(HOOK);
	if (_instance.GetElapsedTime() > hookDataAsset->maxHoldToSwing)
	{
		StartSwing();
	}
}

/// <summary>
///Gère le relâchement de l'entrée du grappin. Arrête le balancement si en cours, ou déclenche la propulsion si en état Idle et un point est sélectionné.
///</summary>
void UHookComponent::OnHookRelease()
{
	switch (currentState)
	{
	case EHookState::Swinging:
		StopSwing();
		break;

	case EHookState::Idle:
		bestPointUsed = bestPoint;
		if (bestPointUsed)
		{
			hookPointLocation = bestPointUsed->GetOffsetPosition();

			DoPropulsion();
			onPropulsionEnd.Broadcast();
		}
		break;

	case EHookState::Propelling:
		break;
	}

	if (currentState != EHookState::Swinging)
	{
		bestPointUsed = nullptr;
	}
}

/// <summary>
///Démarre la séquence de balancement (Swinging). Vérifie la compatibilité du HookType, met l'état à Swinging, désactive le freinage en chute, et initialise la longueur de la corde.
///</summary>
void UHookComponent::StartSwing()
{
	if (!bestPoint)
		return;

	if (!IsPointInRange(bestPoint, owner->GetActorLocation(), hookDataAsset->distanceMin, hookDataAsset->distanceMax))
		return;

	bestPointUsed = bestPoint;
	hookPointLocation = bestPointUsed->GetOffsetPosition();

	const EHookType _pointHookType = bestPointUsed->GetHookToUse();
	const bool _canUseSwing = (_pointHookType == EHookType::Swing || _pointHookType == EHookType::All);
	const bool _playerHasSwing = (currentHookInHand == EHookType::Swing || currentHookInHand == EHookType::All);
	if (_canUseSwing && _playerHasSwing)
	{
		currentState = EHookState::Swinging;
		ownerMovementComponent->BrakingDecelerationFalling = 0.f;

		//TODO
		bestPointUsed->OnUse();
		SpawnAllNiagara(false);
		onSwingStart.Broadcast();
		// owner->SetStateOfBaihu(HOOK);
		usingHook = true;
		// bestPointUsed->HideKeyUI(true);

		EndOtherAction();

		if (hookDataAsset->keepDistanceForSwing)
		{
			swingLenght = FVector::Dist(hookPointLocation, owner->GetActorLocation());
		}
		else
		{
			swingLenght = hookDataAsset->swingLenght;
		}
	}
}

/// <summary>
///Arrête l'état de balancement. Réinitialise l'état à Idle, supprime le point utilisé, désactive les effets visuels et restaure les paramètres de mouvement.
///</summary>
void UHookComponent::StopSwing()
{
	currentState = EHookState::Idle;
	if (hookDataAsset->canLaunchAtEndSwing)
		owner->LaunchCharacter(ownerMovementComponent->Velocity * hookDataAsset->forceMultiplierAtEndSwing, true, true);
	//TODO
	// if (bestPoint)
	// 	bestPoint->HideKeyUI(false);
	bestPointUsed = nullptr;
	DeSpawnNiagara();
	ResetCharMovement();
	onSwingEnd.Broadcast();
}

/// <summary>
/// Logique de Tick pendant le balancement. Applique la force calculée (propulsion/correction) et corrige la position du joueur s'il dépasse la longueur de la corde (swingLenght).
/// </summary>
/// <param name="_deltaTime">Le temps écoulé depuis le dernier Tick.</param>
//void UHookComponent::SwingTick(const float _deltaTime)
//{
//
//	const float _currentDistance = FVector::Dist(hookPointLocation, owner->GetActorLocation());
//
//	if (_currentDistance < swingLenght)
//	{
//		//ownerMovementComponent->AirControl = 0.2f;
//		return;
//	}
//
//	ownerMovementComponent->AddForce(CalculateForceForSwing());
//
//	if (FVector::Dist(hookPointLocation, owner->GetActorLocation()) > swingLenght)
//	{
//		const FVector _ownerLoc = owner->GetActorLocation();
//		FVector _dir = _ownerLoc - hookPointLocation;
//		_dir.Normalize();
//
//		const FVector _targetPos = hookPointLocation + (_dir * swingLenght);
//		const FVector _newPos = FMath::VInterpConstantTo(_ownerLoc, _targetPos, _deltaTime, hookDataAsset->swingCorrectionSpeed);
//		owner->SetActorLocation(_newPos);
//	}
//}

/// <summary>
/// Logique de Tick pendant le balancement. Applique la force calculée (propulsion/correction) et corrige la position du joueur s'il dépasse la longueur de la corde (swingLenght).
/// </summary>
/// <param name="_deltaTime">Le temps écoulé depuis le dernier Tick.</param>
//void UHookComponent::SwingTick(const float _deltaTime)
//{
//	const float _currentDistance = FVector::Dist(hookPointLocation, owner->GetActorLocation());
//
//	if (_currentDistance < swingLenght)
//		return;
//
//	ownerMovementComponent->AddForce(CalculateForceForSwing());
//
//	FVector _toHookDir = hookPointLocation - owner->GetActorLocation();
//	_toHookDir.Normalize();
//
//	const FVector _currentVelocity = owner->GetVelocity();
//
//	const float _velocityAway = FVector::DotProduct(_currentVelocity, _toHookDir);
//
//	if (_velocityAway < 0.f)
//	{
//		FVector _dampingVelocity = _toHookDir * _velocityAway;
//		FVector _newVelocity = _currentVelocity - _dampingVelocity;
//
//		ownerMovementComponent->Velocity = _newVelocity;
//	}
//
//	FVector _pullForce = _toHookDir * hookDataAsset->swingCorrectionPullForce;
//	ownerMovementComponent->AddForce(_pullForce);
//}

/// <summary>
/// Logique de Tick pendant le balancement. Applique la force calculée (propulsion/correction), 
/// gère l'amortissement et corrige la position du joueur s'il dépasse la longueur de la corde.
/// </summary>
/// <param name="_deltaTime">Le temps écoulé depuis le dernier Tick.</param>
//void UHookComponent::SwingTick(const float _deltaTime)
//{
//	const float _currentDistance = FVector::Dist(hookPointLocation, owner->GetActorLocation());
//
//	if (_currentDistance < swingLenght)
//		return;
//
//	ownerMovementComponent->AddForce(CalculateForceForSwing());
//
//	FVector _toHookDir = hookPointLocation - owner->GetActorLocation();
//	_toHookDir.Normalize();
//
//	FVector _currentVelocity = owner->GetVelocity();
//
//	const float _velocityAway = FVector::DotProduct(_currentVelocity, _toHookDir);
//
//	if (_velocityAway < 0.f)
//	{
//		FVector _dampingVelocity = _toHookDir * _velocityAway;
//		FVector _newVelocity = _currentVelocity - _dampingVelocity;
//
//		_currentVelocity = _newVelocity;
//	}
//
//	FVector _horizontalVelocity = FVector(_currentVelocity.X, _currentVelocity.Y, 0.0f);
//
//	//_horizontalVelocity *= hookDataAsset->frictionFactor;
//	float _frictionPerSecond = currentFriction;
//	//_horizontalVelocity *= currentFriction;
//	float _frictionDT = FMath::Pow(_frictionPerSecond, _deltaTime);
//	_horizontalVelocity *= _frictionDT;
//
//	_currentVelocity.X = _horizontalVelocity.X;
//	_currentVelocity.Y = _horizontalVelocity.Y;
//
//	ownerMovementComponent->Velocity = _currentVelocity;
//
//	FVector _pullForce = _toHookDir * hookDataAsset->swingCorrectionPullForce;
//	ownerMovementComponent->AddForce(_pullForce);
//}

void UHookComponent::SwingTick(const float _deltaTime)
{
	if (!owner || !ownerMovementComponent)
		return;

	const FVector _ownerLocation = owner->GetActorLocation();
	FVector _toPlayer = _ownerLocation - hookPointLocation;
	float _currentDistance = _toPlayer.Size();

	if (_currentDistance < swingLenght)
		return;

	FVector _ropeDir = _toPlayer / _currentDistance;

	if (_currentDistance > swingLenght)
	{
		FVector _targetPos = hookPointLocation + _ropeDir * swingLenght;
		owner->SetActorLocation(_targetPos, false);
	}

	FVector _velocity = ownerMovementComponent->Velocity;

	FVector _horizontalVelocity(_velocity.X, _velocity.Y, 0.f);
	float _frictionDT = FMath::Pow(currentFriction, _deltaTime);
	_horizontalVelocity *= _frictionDT;

	_velocity.X = _horizontalVelocity.X;
	_velocity.Y = _horizontalVelocity.Y;

	ownerMovementComponent->Velocity = _velocity;

	FVector _swingForce = CalculateForceForSwing();
	ownerMovementComponent->AddForce(_swingForce);
}


/// <summary>
/// Calcule la force à appliquer au joueur pendant le balancement. Combine une force de correction de vitesse le long de la corde, une force d'input de caméra et une correction de roulis.
/// </summary>
/// <returns>Le vecteur de force résultant.</returns>
FVector UHookComponent::CalculateForceForSwing() const
{
	const FVector _ownerVelocity = owner->GetVelocity();
	const FVector _clampedVelocity = _ownerVelocity.GetClampedToSize(0, 1800);

	FVector _directionToHook = owner->GetActorLocation() - hookPointLocation;
	const float _dot = FVector::DotProduct(_clampedVelocity, _directionToHook);
	_directionToHook.Normalize();

	const FVector _velocityA = _directionToHook * _dot * hookDataAsset->swingForceProjectedVelocity;

	_directionToHook = owner->GetActorLocation() - hookPointLocation;
	const FVector _toPlayerNormalized = UKismetMathLibrary::Normal(_directionToHook, 0.0001f);
	const FVector _playerVelocity = ownerMovementComponent->Velocity;
	const FVector _velocityNormalized = UKismetMathLibrary::Normal(_playerVelocity, 0.0001f);
	const FVector _directionToHookNormalized = UKismetMathLibrary::Normal(_directionToHook, 0.0001f);

	const FVector _cross = FVector::CrossProduct(_velocityNormalized, _directionToHookNormalized);
	const FVector _crossFlipped = _cross * -1.0f;
	const FRotator _swingRot = UKismetMathLibrary::MakeRotFromZX(_toPlayerNormalized, _crossFlipped);

	const FVector _cameraFoward = camOwner->GetForwardVector();

	const FVector _velocityB = UKismetMathLibrary::Normal(FVector(_cameraFoward.X, _cameraFoward.Y, 0.f), 0.0001f) *
		hookDataAsset->swingForceCameraInput;

	FVector _velocityC = UKismetMathLibrary::Normal(owner->GetVelocity(), 0.0001f) * hookDataAsset->
		swingForceVelocityCorrection;
	if (_swingRot.Roll > hookDataAsset->swingForceCorrectionMinRoll && _swingRot.Roll < hookDataAsset->
		swingForceCorrectionMaxRoll)
	{
		_velocityC = FVector::ZeroVector;
	}

	return _velocityA + _velocityB + _velocityC;
}


/// <summary>
///Exécute l'action de propulsion. Vérifie la compatibilité, applique une force de lancement vers le point d'accroche (avec un offset verticale), et utilise un freinage en chute temporaire.
/// </summary>
void UHookComponent::DoPropulsion()
{
	if (!bestPointUsed)
		return;

	if (!bestPointUsed->GetCanDoPropulsion())
		return;

	if (!IsPointInRange(bestPointUsed, owner->GetActorLocation(), hookDataAsset->distanceMin,
	                    hookDataAsset->distanceMax))
		return;

	//if (!IsPointInRange(bestPointUsed, owner->GetActorLocation()))
	//	return;

	const EHookType _pointHookType = bestPointUsed->GetHookToUse();
	const bool _bCanUsePropulsion = (_pointHookType == EHookType::Propulsion || _pointHookType == EHookType::All);
	const bool _bPlayerHasPropulsion = (currentHookInHand == EHookType::Propulsion || currentHookInHand ==
		EHookType::All);

	if (_bCanUsePropulsion && _bPlayerHasPropulsion)
	{
		currentState = EHookState::Propelling;

		FVector _dir = bestPointUsed->GetOffsetPosition() - owner->GetActorLocation();
		const FVector _dirNorm = UKismetMathLibrary::Normal(_dir, 0.0001f);

		FVector _force = _dirNorm * hookDataAsset->propulsionStrenght;
		_force.Z += hookDataAsset->heightAtEndPropulsion;

		ownerMovementComponent->BrakingDecelerationFalling = hookDataAsset->propulsionBrakingDeceleration;

		owner->LaunchCharacter(_force, true, true);
		usingHook = true;
		EndOtherAction();

		onPropulsionStart.Broadcast();
		bestPointUsed->OnUse();
		bestPointUsed->StartTimer();

		SpawnAllNiagara(true);
	}
}

/// <summary>
/// Gère l'apparition des effets visuels Niagara pour le joueur et la corde. Si _withTimer est True (pour la propulsion), configure des minuteurs pour la désactivation des effets et la réinitialisation du mouvement.
/// </summary>
/// <param name="_withTimer">Indique si les effets doivent être désactivés automatiquement par minuteur.</param>
void UHookComponent::SpawnAllNiagara(bool _withTimer)
{
	SpawnPlayerNiagara();
	SpawnRopeNiagara();
	SpawnTrailNiagara();

	if (_withTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(
			timerDespawnNiagara,
			this,
			&UHookComponent::DeSpawnNiagara,
			hookDataAsset->delayDespawnNiagara,
			false
		);

		GetWorld()->GetTimerManager().SetTimer(
			timerResetCharMovement,
			this,
			&UHookComponent::ResetCharMovement,
			hookDataAsset->delayDespawnNiagara,
			false
		);
	}
}

/// <summary>
///Fait apparaître le système Niagara du joueur et masque le mesh du personnage. Annule le minuteur de désactivation si un nouveau swing est lancé.
///</summary>
void UHookComponent::SpawnPlayerNiagara()
{
	if (!hookDataAsset->playerNiagaraSystem || !owner) return;

	DeSpawnNiagara();

	USkeletalMeshComponent* _mesh = owner->GetMesh();
	if (!_mesh) return;

	_mesh->SetVisibility(false);

	playerNiagaraSpawned = UNiagaraFunctionLibrary::SpawnSystemAttached(
		hookDataAsset->playerNiagaraSystem,
		_mesh,
		NAME_None,
		FVector(0, 0, 90),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		false
	);

	GetWorld()->GetTimerManager().ClearTimer(timerDespawnNiagara);
}

/// <summary>
///Fait apparaître le système Niagara de la corde/ligne. Définit les paramètres de position et de longueur de la corde.
///</summary>
void UHookComponent::SpawnRopeNiagara()
{
	if (!hookDataAsset->ropeNiagaraSystem || !owner) return;

	USkeletalMeshComponent* _mesh = owner->GetMesh();
	if (!_mesh) return;

	ropeNiagaraSpawned = UNiagaraFunctionLibrary::SpawnSystemAttached(
		hookDataAsset->ropeNiagaraSystem,
		_mesh,
		NAME_None,
		owner->GetActorLocation(),
		FRotator::ZeroRotator,
		EAttachLocation::KeepWorldPosition,
		false
	);

	if (ropeNiagaraSpawned)
	{
		ropeNiagaraSpawned->SetVectorParameter(TEXT("EndingPosition"), hookPointLocation);
		ropeNiagaraSpawned->SetFloatParameter(
			TEXT("LineLength"), FVector::Dist(hookPointLocation, owner->GetActorLocation()));
	}
}

void UHookComponent::SpawnTrailNiagara()
{
	if (!hookDataAsset->trailNiagaraSystem || !owner) return;

	USkeletalMeshComponent* _mesh = owner->GetMesh();
	if (!_mesh) return;

	trailNiagaraSpawned = UNiagaraFunctionLibrary::SpawnSystemAttached(
		hookDataAsset->trailNiagaraSystem,
		_mesh,
		NAME_None,
		FVector(0, 0, 90),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		false
	);
}

/// <summary>
///Désactive et détruit les systèmes Niagara de joueur et de corde. Rend le mesh du personnage à nouveau visible.
/// </summary>
void UHookComponent::DeSpawnNiagara()
{
	if (owner)
	{
		if (USkeletalMeshComponent* _mesh = owner->GetMesh())
		{
			_mesh->SetVisibility(true);
		}
	}

	if (playerNiagaraSpawned && playerNiagaraSpawned->IsValidLowLevel())
	{
		playerNiagaraSpawned->DestroyComponent();
		playerNiagaraSpawned = nullptr;
	}

	if (ropeNiagaraSpawned && ropeNiagaraSpawned->IsValidLowLevel())
	{
		ropeNiagaraSpawned->DestroyComponent();
		ropeNiagaraSpawned = nullptr;
	}
	if (trailNiagaraSpawned && trailNiagaraSpawned->IsValidLowLevel())
	{
		trailNiagaraSpawned->DestroyComponent();
		trailNiagaraSpawned = nullptr;
	}
}

/// <summary>
///Restaure la valeur de base de la décélération en chute du CharacterMovementComponent (BrakingDecelerationFalling).
/// </summary>
void UHookComponent::ResetCharMovement()
{
	if (ownerMovementComponent)
	{
		ownerMovementComponent->BrakingDecelerationFalling = brakingDecelerationFallingBase;
	}
	currentState = EHookState::Idle;
	usingHook = false;
	// owner->SetStateOfBaihu(IDLE);
}

/// <summary>
///Dessine des sphères de débogage pour visualiser les portées maximale et minimale du grappin autour du joueur, si les options de débogage sont activées dans le DataAsset.
/// </summary>
void UHookComponent::DrawDebug() const
{
	if (!hookDataAsset->debugMaxDistance && !hookDataAsset->debugMinDistance)
		return;

	const UWorld* _world = GetWorld();
	if (!_world) return;

	const FVector _ownerLocation = owner->GetActorLocation();

	if (hookDataAsset->debugMaxDistance)
	{
		DrawDebugSphere(_world, _ownerLocation, hookDataAsset->distanceMax, 12, FColor::Green, false, -1.f, 0, 10.f);
	}
	if (hookDataAsset->debugMinDistance)
	{
		DrawDebugSphere(_world, _ownerLocation, hookDataAsset->distanceMin, 12, FColor::Red, false, -1.f, 0, 10.f);
	}
}

/// <summary>
/// Vérifie si le HookDataAsset est assigné et non nul. Log une erreur si ce n'est pas le cas.
/// </summary>
/// <returns>True si le DataAsset est valide, False sinon.</returns>
bool UHookComponent::CheckDataAsset() const
{
	if (!hookDataAsset)
	{
		LOG_ERROR("hookDataAsset is missing (nullptr).");
		return false;
	}
	return true;
}

void UHookComponent::ShowUI(TObjectPtr<AHookPoint> _newBestPoint)
{
	//TODO
	if (bestPoint != _newBestPoint)
	{
		if (bestPoint)
		{
			// bestPoint->HideKeyUI(true);
			bestPoint->ShowHookUI(false);
		}
		if (_newBestPoint)
		{
			// _newBestPoint->HideKeyUI(false);
			_newBestPoint->ShowHookUI(true);
		}
	}
}

void UHookComponent::EndOtherAction()
{
}

void UHookComponent::StartMoveOnSwing()
{
	currentFriction = hookDataAsset->frictionFactorWhenMoving;
}

void UHookComponent::EndMoveOnSwing()
{
	currentFriction = hookDataAsset->frictionFactorWhenNoMoving;
}
