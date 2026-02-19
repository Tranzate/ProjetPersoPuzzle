#include "Door.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* _doorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	RootComponent = _doorRoot;

	leftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	leftDoor->SetupAttachment(_doorRoot);

	rightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	rightDoor->SetupAttachment(_doorRoot);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	initialLeftPos = leftDoor->GetRelativeLocation();
	initialRightPos = rightDoor->GetRelativeLocation();
}

void ADoor::OnActivate(AActor* _other)
{
	if (!targetOpening)
	{
		float _currentPos = FMath::Abs(leftDoor->GetRelativeLocation().Y - initialLeftPos.Y);
		currentStartOffset = _currentPos;
		
		targetOpening = true;
		isMoving = true;
		elapsedTime = 0.0f;
	}
}

void ADoor::OnDeActivate(AActor* _other)
{
	if (targetOpening)
	{
		float _currentPos = FMath::Abs(leftDoor->GetRelativeLocation().Y - initialLeftPos.Y);
		currentStartOffset = _currentPos;

		targetOpening = false;
		isMoving = true;
		elapsedTime = 0.0f;
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isMoving)
	{
		elapsedTime += DeltaTime;
		float _alpha = FMath::Clamp(elapsedTime / totalDuration, 0.0f, 1.0f);
		float _bounceValue = EaseOutBounce(_alpha);
		float _finalTarget = targetOpening ? maxSlideDistance : 0.0f;
		float _currentOffset = FMath::Lerp(currentStartOffset, _finalTarget, _bounceValue);

		leftDoor->SetRelativeLocation(initialLeftPos + FVector(0.0f, -_currentOffset, 0.0f));
		rightDoor->SetRelativeLocation(initialRightPos + FVector(0.0f, _currentOffset, 0.0f));

		if (_alpha >= 1.0f)
		{
			isMoving = false;
		}
	}
}
float ADoor::EaseOutBounce(float x)
{
	const float _n1 = 7.5625f;
	const float _d1 = 2.75f;
	if (x < 1.0f / _d1) return _n1 * x * x;
	else if (x < 2.0f / _d1)
	{
		x -= 1.5f / _d1;
		return _n1 * x * x + 0.75f;
	}
	else if (x < 2.5f / _d1)
	{
		x -= 2.25f / _d1;
		return _n1 * x * x + 0.9375f;
	}
	else
	{
		x -= 2.625f / _d1;
		return _n1 * x * x + 0.984375f;
	}
}