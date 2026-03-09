#include "ElevatorButton.h"
#include "Node/NodeElevator.h"

AElevatorButton::AElevatorButton()
{
}

void AElevatorButton::Interact(APuzzleCharacter* _player)
{
	UE_LOG(LogTemp, Warning, TEXT("[ElevatorButton] Interact appele | isCallButton: %s | targetFloor: %s | callFloor: %s | parentNode: %s"),
		isCallButton ? TEXT("true") : TEXT("false"),
		*targetFloorName,
		*callFloorName,
		parentNode ? TEXT("OK") : TEXT("NULL"));

	if (!parentNode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ElevatorButton] ERREUR: parentNode est NULL !"));
		return;
	}

	const FString _floor = isCallButton ? callFloorName : targetFloorName;
	parentNode->OnFloorSelected(_floor);
}

void AElevatorButton::UpdateText(FString _newText)
{
	textToShow->SetText(FText::FromString(_newText));
}
