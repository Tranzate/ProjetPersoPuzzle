#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EasingFunctions.h"
#include "Elevator.generated.h"

class ADoor;
class ANodeElevator;

UCLASS()
class PROJPERSOPUZZLE_API AElevator : public AActor
{
	GENERATED_BODY()
    
public: 
	AElevator();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* buttonPanelAnchor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* elevatorMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UChildActorComponent* doorComponent;
    
	UPROPERTY()
	ADoor* door;
	
	bool IsMoving() const { return isMoving; }
	bool IsReady() const { return !isMoving && !isWaitingAtFloor; }

	UPROPERTY()
	ANodeElevator* nodeElevator;

	void MoveToHeight(float _height);
	void CloseDoor();
	void OpenDoor();

	FORCEINLINE float GetWaitTime() const;
	void NotifyArrival();
	
	void Configure(float _speed, EEasingFamily _family, EEasingDirection _dir)
	{
		moveSpeed       = _speed;
		easingFamily    = _family;
		easingDirection = _dir;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	float startZ  = 0.f;
	float targetZ = 0.f;
	
	float elapsedTime   = 0.f;
	float moveDuration  = 0.f;

	bool isMoving        = false;
	bool isWaitingAtFloor = false;
	
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	float moveSpeed = 300.f;
	
	EEasingFamily    easingFamily    = EEasingFamily::Cubic;
	EEasingDirection easingDirection = EEasingDirection::InOut;

	UPROPERTY()
	FTimerHandle waitTimerHandle;
};
