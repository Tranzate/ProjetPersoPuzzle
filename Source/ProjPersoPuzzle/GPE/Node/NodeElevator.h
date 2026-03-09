#pragma once

#include "CoreMinimal.h"
#include "GPE/Node/Node.h"
#include "EasingFunctions.h"
#include "NodeElevator.generated.h"

class AElevator;
class ADoor;
class AElevatorButton;

UCLASS()
class PROJPERSOPUZZLE_API ANodeElevator : public ANode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Elevator | Etages")
	TArray<FString> floorQueue;
	void ProcessNextFloor();
	bool isProcessingQueue = false;

public:
	ANodeElevator();
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Etages")
	TMap<FString, float> floorData;
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Setup")
	AElevator* targetedElevator;
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Setup")
	float moveSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Elevator | Setup")
	float waitTimeAtFloor = 3.f;
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Setup")
	EEasingFamily easingFamily = EEasingFamily::Cubic;

	UPROPERTY(EditAnywhere, Category = "Elevator | Setup")
	EEasingDirection easingDirection = EEasingDirection::InOut;
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Interieurs")
	TSubclassOf<AActor> buttonClass;

	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Interieurs")
	int buttonColumns = 1;

	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Interieurs")
	float buttonSpacingH = 25.f;

	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Interieurs")
	float buttonSpacingV = 25.f;
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Portes")
	TSubclassOf<ADoor> floorDoorClass;

	UPROPERTY(EditAnywhere, Category = "Elevator | Portes")
	FVector floorDoorOffset = FVector(200.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Elevator | Portes")
	FRotator floorDoorRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(VisibleAnywhere, Category = "Elevator | Portes")
	TMap<FString, ADoor*> floorDoors;

	UFUNCTION(CallInEditor, Category = "Elevator | Portes")
	void SpawnFloorDoors();

	UFUNCTION(CallInEditor, Category = "Elevator | Portes")
	void DestroyFloorDoors();
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Test")
	FString testFloorName = "";

	UFUNCTION(CallInEditor, Category = "Elevator | Test")
	void MoveToTestFloor();
	
	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Call")
	FVector callButtonOffset = FVector(200.f, 0.f, 50.f);

	UPROPERTY(EditAnywhere, Category = "Elevator | Boutons Call")
	FRotator callButtonRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(VisibleAnywhere, Category = "Elevator | Boutons Call")
	TMap<FString, AElevatorButton*> callButtons;

	UFUNCTION(CallInEditor, Category = "Elevator | Boutons Call")
	void SpawnCallButtons();

	UFUNCTION(CallInEditor, Category = "Elevator | Boutons Call")
	void DestroyCallButtons();

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UFUNCTION()
	void MoveElevator(bool _targetOpening);

	void OnFloorSelected(FString _floorName);
	void CallToHeight(float _height);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GenerateButtons();

	FVector ComputeDoorPosition(float _floorZ) const;
	FRotator ComputeDoorRotation() const;
	FVector ComputeCallButtonPosition(float _floorZ) const;
	FRotator ComputeCallButtonRotation() const;

public:
	void OpenAllDoorsAtFloor(FString _floorName);
	void CloseAllDoorsAtFloor(FString _floorName);
	FString GetCurrentFloorName() const { return currentFloorName; }
	void NotifyArrival();

private:
	FString pendingFloorName;
	float pendingHeight;
	bool isCallPending = false;

	UPROPERTY(VisibleAnywhere, Category = "Elevator | Etages")
	FString currentFloorName = "";

	UPROPERTY()
	FTimerHandle initTimerHandle;
	UPROPERTY()
	FTimerHandle nextFloorTimerHandle;
	UPROPERTY()
	FTimerHandle queueTimerHandle;
};
