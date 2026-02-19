// Fill out your copyright notice in the Description page of Project Settings.


#include "HookPoint.h"
#include "HookPointSubsystem.h"
#include "Utility.h"
#include "HookPointDataAsset.h"
#include "PuzzleCharacter.h"
#include "Kismet/GameplayStatics.h"
//
// AHookPoint::AHookPoint()
// {
// 	PrimaryActorTick.bCanEverTick = true;
// 	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
// 	RootComponent = mesh;
//
// 	hookUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("HookUI"));
// 	hookUI->SetupAttachment(mesh);
// 	hookUI->SetWidgetSpace(EWidgetSpace::Screen);
// 	hookUI->SetDrawSize(FVector2D(64.f, 64.f));
// 	hookUI->SetVisibility(false);
// }
//
// void AHookPoint::BeginPlay()
// {
// 	Super::BeginPlay();
// 	if (TObjectPtr<UWorld> _world = GetWorld())
// 	{
// 		if (TObjectPtr<UHookPointSubsystem> _sub = _world->GetSubsystem<UHookPointSubsystem>())
// 		{
// 			_sub->RegisterPoint(this);
// 		}
// 	}
//
//
// }
//
// void AHookPoint::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }
//
// void AHookPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
// {
// 	if (TObjectPtr<UWorld> _world = GetWorld())
// 	{
// 		if (TObjectPtr<UHookPointSubsystem> _sub = _world->GetSubsystem<UHookPointSubsystem>())
// 		{
// 			_sub->UnRegisterPoint(this);
// 		}
// 	}
//
// 	Super::EndPlay(EndPlayReason);
// }
//
// void AHookPoint::ShowHookUI(bool _visible)
// {
// 	if (hookUI)
// 		hookUI->SetVisibility(_visible);
// }
//
// void AHookPoint::StartTimer() {
// 	canDoPropulsion = false;
// 	FTimerHandle _timer;
// 	TIMER(this, _timer, SetCanDoPropulsion(true), delayTimerForSpam);
// }


AHookPoint::AHookPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	mesh->SetupAttachment(RootComponent);
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	offsetPosition = CreateDefaultSubobject<UBillboardComponent>("OffsetPosition");
	offsetPosition->SetupAttachment(mesh);

	hookUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("HookUI"));
	hookUI->SetupAttachment(offsetPosition);
	hookUI->SetWidgetSpace(EWidgetSpace::Screen);
	hookUI->SetWidgetSpace(EWidgetSpace::Screen);
	hookUI->SetDrawSize(FVector2D(1024.f, 1024.f));
	hookUI->SetVisibility(false);
}

void AHookPoint::HideKeyUI(const bool _value)
{
	//TODO
	// if (currentHookUI)
	// 	currentHookUI->HideKeyImage(_value);
}

void AHookPoint::BeginPlay()
{
	Super::BeginPlay();
	if (TObjectPtr<UWorld> _world = GetWorld())
	{
		if (TObjectPtr<UHookPointSubsystem> _sub = _world->GetSubsystem<UHookPointSubsystem>())
		{
			_sub->RegisterPoint(this);
		}
	}
	//TODO
	// if (hookPointDataAsset && hookPointDataAsset->hookUIClass)
	// {
	// 	hookUI->SetWidgetClass(hookPointDataAsset->hookUIClass);
	// 	//TODO
	// 	// if (UUserWidget* userWidget = hookUI->GetWidget())
	// 	// {
	// 	// 	currentHookUI = Cast<UCurrentHookPointWidget>(userWidget);
	// 	// }
	// 	hookUI->InitWidget();
	// 	hookUI->SetWorldLocation(offsetPosition->GetComponentLocation());
	// }
	// else
	// {
	// 	LOG_ERROR("Missing hookPointDataAsset reference");
	// }

	player = Cast<APuzzleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

}

void AHookPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShowKeyToUse();
}

void AHookPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TObjectPtr<UWorld> _world = GetWorld())
	{
		if (TObjectPtr<UHookPointSubsystem> _sub = _world->GetSubsystem<UHookPointSubsystem>())
		{
			_sub->UnRegisterPoint(this);
		}
	}

	if (hookUI)
		hookUI->SetVisibility(false);

	Super::EndPlay(EndPlayReason);
}

void AHookPoint::ShowKeyToUse()
{
	//TODO
	// if (hookUI->IsVisible())
	// {
	// 	if (currentHookUI)
	// 	{
	// 		if (player)
	// 			currentHookUI->ChangeSizeImageRingByDistance(
	// 				FVector::Dist(player->GetActorLocation(), GetOffsetPosition()), hookDataAsset->distanceMax);
	// 	}
	// }
}

/// <summary>
/// Définit la visibilité du composant d'interface utilisateur (HookUI) affiché au-dessus du point d'accroche.
/// </summary>
/// <param name="_visible">True pour rendre l'UI visible, False pour la masquer.</param>
void AHookPoint::ShowHookUI(bool _visible)
{
	if (hookUI)
	{
		hookUI->SetVisibility(_visible);
		ShowKeyToUse();
	}
}

/// <summary>
///Démarre un minuteur de réutilisation pour la propulsion. Définit temporairement 'canDoPropulsion' à false, puis le rétablit à true après le délai spécifié par le DataAsset, empêchant l'abus (spam).
///</summary>
void AHookPoint::StartTimer()
{
	canDoPropulsion = false;
	if (!hookPointDataAsset)
	{
		LOG_ERROR("Missing hookPointDataAsset reference");
		return;
	}
	FTimerHandle _timer;
	TIMER(this, _timer, SetCanDoPropulsion(true), hookPointDataAsset->delayTimerForSpam);
}

/// <summary>
///Exécuté lorsque ce point est utilisé par le grappin d'un joueur. Déclenche le 'nodeToTrigger' si celui-ci est défini (pour interagir avec d'autres logiques de jeu).
///</summary>
void AHookPoint::OnUse()
{
	//TODO
	// if (nodeToTrigger)
	// nodeToTrigger->Trigger();
}
