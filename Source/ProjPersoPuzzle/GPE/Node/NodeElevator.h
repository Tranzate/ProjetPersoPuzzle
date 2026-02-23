#pragma once

#include "CoreMinimal.h"
#include "GPE/Node/Node.h"
#include "NodeElevator.generated.h"

class AElevator;
class ADoor;

UCLASS()
class PROJPERSOPUZZLE_API ANodeElevator : public ANode
{
	GENERATED_BODY()
private:
	TArray<FString> floorQueue;
	
	void ProcessNextFloor();

	bool isProcessingQueue = false;
public:
	ANodeElevator();

	UPROPERTY(EditAnywhere, Category = "Elevator Configuration")
	TMap<FString, float> floorData;

	UPROPERTY(EditAnywhere, Category = "Elevator Configuration")
	AElevator* targetedElevator;

	UPROPERTY(EditAnywhere, Category = "Elevator Configuration")
	TSubclassOf<AActor> buttonClass;

	UPROPERTY(EditAnywhere, Category = "Elevator Configuration")
	TMap<FString, ADoor*> floorDoors;

	UFUNCTION()
	void MoveElevator(bool _targetOpening);

	void OnFloorSelected(FString _floorName);
	void CallToHeight(float _height);

protected:
	virtual void BeginPlay() override;
	void GenerateButtons();
public:
	void OpenAllDoorsAtFloor(FString _floorName);
	void CloseAllDoorsAtFloor(FString _floorName);
	FString GetCurrentFloorName() const { return currentFloorName; }

private:
	FString pendingFloorName;
	float pendingHeight;
	bool isCallPending = false;
	UPROPERTY(VisibleAnywhere, Category = "Elevator Configuration")
	FString currentFloorName = "";
};