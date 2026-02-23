#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	float targetZ;
	bool isMoving = false;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	float moveSpeed = 5.0f;

private:
	bool isWaitingAtFloor = false;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	float waitTimeAtFloor = 3.0f;
};