// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TextRenderComponent.h>
#include "InteractibleActor.generated.h"

class APuzzleCharacter;

UCLASS(Abstract)
class PROJPERSOPUZZLE_API AInteractibleActor : public AActor
{
	GENERATED_BODY()

protected:
	//UPROPERTY(EditAnywhere)
	//TObjectPtr<UWidgetComponent> widgetComp = nullptr;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, APuzzleCharacter*, character);

	UPROPERTY(BlueprintAssignable)
	FOnInteract onInteract;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UTextRenderComponent> textToShow = nullptr;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UMaterialInterface> materialOutline = nullptr;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UStaticMeshComponent> mesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<APlayerCameraManager> playerCamera = nullptr;

public:
	AInteractibleActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void ShowInteractionUI();
	virtual void HideInteractionUI();
	virtual FOnInteract& GetOnInteract() { return onInteract; };
	UFUNCTION(BlueprintCallable)
	virtual void Interact(APuzzleCharacter* _player) { onInteract.Broadcast(_player); };
};
