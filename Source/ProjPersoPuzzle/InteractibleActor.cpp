// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleActor.h"
#include <Kismet/GameplayStatics.h>

AInteractibleActor::AInteractibleActor()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* _root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = _root;
	textToShow = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextToShow"));
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(_root);
	textToShow->SetupAttachment(mesh);

	textToShow->SetHorizontalAlignment(EHTA_Center);
	textToShow->SetVerticalAlignment(EVRTA_TextCenter);
	textToShow->SetUsingAbsoluteRotation(true);


}

void AInteractibleActor::BeginPlay()
{
	Super::BeginPlay();
	textToShow->SetHiddenInGame(true);
	playerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

void AInteractibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!textToShow) return;
	if (textToShow->bHiddenInGame) return;

	const FVector _widgetLocation = textToShow->GetComponentLocation();
	const FVector _cameraLocation = playerCamera->GetCameraLocation();

	FRotator _lookAtRot = (_cameraLocation - _widgetLocation).Rotation();

	_lookAtRot.Pitch = 0.f;
	_lookAtRot.Roll = 0.f;

	textToShow->SetWorldRotation(_lookAtRot);
}

void AInteractibleActor::ShowInteractionUI()
{
	if (textToShow) {
		const FBoxSphereBounds _bounds = mesh->Bounds;

		FVector _textLocation = _bounds.Origin
			+ FVector(0.f, 0.f, _bounds.BoxExtent.Z + 20.f);

		textToShow->SetWorldLocation(_textLocation);

		textToShow->SetHiddenInGame(false);
	}
	mesh->SetOverlayMaterial(materialOutline);
	
}

void AInteractibleActor::HideInteractionUI()
{

	if (textToShow)
		textToShow->SetHiddenInGame(true);

	mesh->SetOverlayMaterial(nullptr);

}

