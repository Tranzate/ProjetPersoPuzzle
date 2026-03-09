#pragma once

#include "CoreMinimal.h"
#include "ActivableActor.h"
#include "Door.generated.h"


UCLASS()
class PROJPERSOPUZZLE_API ADoor : public AActivableActor
{
	GENERATED_BODY()

public:
	ADoor();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorFinishMovement, bool, _targetOpening);

	UPROPERTY(BlueprintAssignable, Category = "Door Events")
	FOnDoorFinishMovement OnDoorFinishMovement;

	virtual void OnActivate(AActor* _other) override;
	virtual void OnDeActivate(AActor* _other) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> leftDoor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> rightDoor;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float maxSlideDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float totalDuration = 1.5f;

	bool targetOpening = false;
	float elapsedTime = 0.0f;
	bool isMoving = false;

	FVector initialLeftPos;
	FVector initialRightPos;
	float currentStartOffset = 0.0f;

	float EaseOutBounce(float x);

public:
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetLeftDoor() { return leftDoor; }
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetRightDoor() { return rightDoor; }
};