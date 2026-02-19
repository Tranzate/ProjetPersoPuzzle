#include "Node.h"
#include "DrawDebugHelpers.h"
#include "NodesWorldSubsystem.h"
#include "InGameHUD.h"
#include "InGameWidget.h"
#include "NotificationListWidget.h"
#include "Utility.h"

ANode::ANode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	textDebug = CreateDefaultSubobject<UTextRenderComponent>("TextDebug");
	textDebug->SetupAttachment(RootComponent);
	textDebug->SetRelativeLocation(FVector(0, 0, 50));
	textDebug->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
}

void ANode::BeginPlay()
{
	Super::BeginPlay();
	if (TObjectPtr<UWorld> _world = GetWorld())
	{
		if (TObjectPtr<UNodesWorldSubsystem> _sub = _world->GetSubsystem<UNodesWorldSubsystem>())
		{
			id = _sub->GetNumNodes() + 1;
			name = GetActorLabel();
			_sub->Register(this);
		}
	}
	textDebug->SetText(FText::FromString(GetNameID()));
	if (APlayerController* _pc = GetWorld()->GetFirstPlayerController())
	{
		player = Cast<APuzzleCharacter>(_pc->GetPawn());
		if (player)
			playerCamera = player->GetPlayerCamera();
	}
	GetNotifUI();
}

void ANode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TObjectPtr<UWorld> _world = GetWorld())
	{
		if (TObjectPtr<UNodesWorldSubsystem> _sub = _world->GetSubsystem<UNodesWorldSubsystem>())
		{
			_sub->DeRegister(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}


void ANode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	ShowDebug();
#endif

	if (playerCamera && textDebug)
	{
		FVector _lookAtDir = playerCamera->GetComponentLocation() - textDebug->GetComponentLocation();
		FRotator _newRot = _lookAtDir.Rotation();
		textDebug->SetWorldRotation(_newRot);
	}
}


void ANode::OnActivate(AActor* _other)
{
	if (isActivated) return;
	isActivated = true;
	PlayNodeFeedback();
	SendSignal(true, _other);
}

void ANode::OnDeActivate(AActor* _other)
{
	if (!isActivated) return;
	isActivated = false;
	SendSignal(false, _other);
}

void ANode::SendSignal(bool _status, AActor* _other)
{
	if (isProcessing) return;
	isProcessing = true;

	for (ANode* _target : nodeToActivate)
	{
		if (_target)
		{
			if (_status) _target->OnActivate(_other);
			else _target->OnDeActivate(_other);
		}
	}
	GetNotifUI();
	if (notifUIRef)
	{
		FString _notif = _other->GetActorLabel() + " -> SendSignal : " + (_status ? "True" : "False");
		notifUIRef->AddNotification(_notif);
	}
	isProcessing = false;
}

void ANode::ShowDebug()
{
	if (!showDebug) return;
	for (ANode* _target : nodeToActivate)
	{
		if (_target)
		{
			DrawDebugSphere(GetWorld(), _target->GetActorLocation(), 60.f, 8, FColor::Yellow, false, -1.f, 0, 2.f);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), _target->GetActorLocation(),
			                          150.f, FColor::Cyan, false, -1.f, 0, 5.0f);
		}
	}

	for (AActor* _target : actorActiveNode)
	{
		if (_target)
		{
			DrawDebugSphere(GetWorld(), _target->GetActorLocation(), 60.f, 8, FColor::Yellow, false, -1.f, 0, 2.f);

			DrawDebugDirectionalArrow(GetWorld(), _target->GetActorLocation(), GetActorLocation(),
			                          150.f, FColor::Cyan, false, -1.f, 0, 5.0f);
		}
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 8, FColor::Red, false, -1.f, 0, 2.f);
}

void ANode::PlayNodeFeedback()
{
	if (activationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, activationSound, GetActorLocation());
	}
}

void ANode::ForceActivate()
{
	OnActivate(this);
}

void ANode::ForceDeActivate()
{
	OnDeActivate(this);
}

void ANode::GetNotifUI()
{
	if (notifUIRef) return;
	if (APlayerController* _pc = GetWorld()->GetFirstPlayerController())
	{
		if (AInGameHUD* _hud = Cast<AInGameHUD>(_pc->GetHUD()))
		{
			UInGameWidget* _mainWidget = _hud->GetInGameWidget();
			if (_mainWidget)
			{
				notifUIRef = _mainWidget->GetNotificationListWidget();
			}
		}
	}
}
