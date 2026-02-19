// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Camera/CameraComponent.h>
#include "HookPointSubsystem.h"
#include "HookDataAsset.h"
#include <EnhancedInputSubsystems.h>

#include "HookComponent.generated.h"

//
// UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
// class PROJPERSOPUZZLE_API UHookComponent : public UActorComponent
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere) TObjectPtr<ACharacter> owner = nullptr;
// 	UPROPERTY(EditAnywhere) TObjectPtr<UCameraComponent> camOwner = nullptr;
// 	UPROPERTY(EditAnywhere) TObjectPtr<UHookPointSubsystem> hookPointSubsystem = nullptr;
//
// 	UPROPERTY(EditAnywhere) TObjectPtr<UHookDataAsset> hookDataAsset = nullptr;
//
// 	UPROPERTY(EditAnywhere) TObjectPtr<AHookPoint> bestPoint = nullptr;
// 	UPROPERTY(EditAnywhere) TObjectPtr<AHookPoint> bestPointUsed = nullptr;
// 	UPROPERTY(EditAnywhere) FVector hookPointLocation = FVector::ZeroVector;
//
// 	UPROPERTY(EditAnywhere) bool isSwinging = false;
// 	UPROPERTY(EditAnywhere) bool isHooking = false;
// 	UPROPERTY(EditAnywhere) EHookType currentHookInHand = EHookType::All;
//
// 	UPROPERTY(EditAnywhere) bool canTick = false;
//
// 	UPROPERTY(EditAnywhere) float brakingDecelerationFallingBase = 0.f;
//
// 	UPROPERTY() float swingLenght = 0.f;
//
//
// 	UPROPERTY(EditAnywhere) TObjectPtr<UNiagaraComponent> playerNiagaraSpawned = nullptr;
// 	UPROPERTY(EditAnywhere) TObjectPtr<UNiagaraComponent> ropeNiagaraSpawned = nullptr;
// 	UPROPERTY(EditAnywhere) FTimerHandle timerDespawnNiagara;
//
//
//
// public:
//
// 	UHookComponent();
//
// protected:
// 	virtual void BeginPlay() override;
//
// 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
// 	bool HookPointIsAtGoodDistance(TObjectPtr<AHookPoint> _hookPoint);
//
// 	TArray<TObjectPtr<AHookPoint>> GetAllGoodPoint(TArray<AHookPoint*> _allPoint, const FVector _cameraLocation);
//
// 	bool LineTraceCheckWall(const FVector _hookPointLoc, const FVector _camLoc);
//
// 	void StartSwing();
//
// 	void SwingTick(const float _deltaTime);
//
// 	FVector CalculateForceForSwing();
// 	void DoPropulsion();
//
// 	void SpawnPlayerNiagara();
// 	void SpawnRopeNiagara();
// 	void DeSpawnNiagara();
// public:
//
// 	UFUNCTION(BlueprintCallable) AHookPoint* GetBestHookPoint();
//
// 	UFUNCTION(BlueprintCallable) void StartHookPressed(const FInputActionInstance& _instance);
// 	UFUNCTION(BlueprintCallable) void OnHookRelease();
//
// };

class APuzzleCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EHookState : uint8
{
	Idle,
	Swinging,
	Propelling 
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJPERSOPUZZLE_API UHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookComponent();

	UFUNCTION(BlueprintCallable)
	void StartHookPressed(const FInputActionInstance& _instance);

	UFUNCTION(BlueprintCallable)
	void OnHookRelease();

	UFUNCTION(BlueprintCallable)
	void StartHookPressed_Deprecated(const float _timeTriggeredSeconds);

	void StartMoveOnSwing();
	void EndMoveOnSwing();

	UFUNCTION(BlueprintCallable)
	AHookPoint* GetBestHookPoint() const { return bestPoint; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPropulsionStart);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPropulsionEnd);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwingStart);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwingEnd);

	UPROPERTY(BlueprintAssignable, Category = "Hook|Events")
	FOnPropulsionStart onPropulsionStart;

	UPROPERTY(BlueprintAssignable, Category = "Hook|Events")
	FOnPropulsionEnd onPropulsionEnd;

	UPROPERTY(BlueprintAssignable, Category = "Hook|Events")
	FOnSwingStart onSwingStart;

	UPROPERTY(BlueprintAssignable, Category = "Hook|Events")
	FOnSwingEnd onSwingEnd;



protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE bool UsingHook() { return usingHook; }
	FORCEINLINE EHookState GetHookState() { return currentState; }

protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	EHookType currentHookInHand = EHookType::All;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TObjectPtr<UHookDataAsset> hookDataAsset = nullptr;

	UPROPERTY(EditAnywhere , Category = "Settings")
	float timeToStayInCurrentState = 3.0f;

private:
	void FindBestHookPoint();
	bool IsHookPointValid(TObjectPtr<AHookPoint> _hookPoint, const FVector& _cameraLocation, const FVector& _ownerLocation);
	bool IsPointInRange(const AHookPoint* _hookPoint, const FVector& _ownerLocation, const float _minDist, const float _maxDist) const;
	bool HasLineOfSight(const FVector& _targetLocation, const FVector& _startLocation);
	float CalculateScoreForPoint(const AHookPoint* _hookPoint, const FVector& _cameraLocation, const FVector& _cameraForward) const;

	void StartSwing();
	void SwingTick(const float _deltaTime);
	void StopSwing();
	FVector CalculateForceForSwing() const;

	void DoPropulsion();

	void SpawnAllNiagara(bool _withTimer);
	void SpawnPlayerNiagara();
	void SpawnRopeNiagara();
	void SpawnTrailNiagara();
	UFUNCTION() void DeSpawnNiagara();

	UFUNCTION() void ResetCharMovement();
	void DrawDebug() const;
	bool CheckDataAsset() const;

	void ShowUI(TObjectPtr<AHookPoint> _newBestPoint);

	void EndOtherAction();


	UPROPERTY(VisibleAnywhere, Category = "Hook|State")
	EHookState currentState = EHookState::Idle;

	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	TObjectPtr<AHookPoint> bestPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	TObjectPtr<AHookPoint> bestPointUsed = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	FVector hookPointLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	float swingLenght = 0.f;

	float brakingDecelerationFallingBase = 0.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<APuzzleCharacter> owner = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> ownerMovementComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> camOwner = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UHookPointSubsystem> hookPointSubsystem = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> playerNiagaraSpawned = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> ropeNiagaraSpawned = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> trailNiagaraSpawned = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	bool usingHook = false;

	UPROPERTY(VisibleAnywhere, Category = "Hook|State", Transient)
	float currentFriction = 0.f;


	UPROPERTY() FTimerHandle timerDespawnNiagara;
	UPROPERTY() FTimerHandle timerResetCharMovement;
	UPROPERTY() FTimerHandle timerStateHook;
};