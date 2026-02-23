#include "ElevatorButton.h"
#include "Node/NodeElevator.h"

AElevatorButton::AElevatorButton()
{
}

void AElevatorButton::Interact(APuzzleCharacter* _player)
{
	if (parentNode)
	{
		if (isCallButton) 
		{
			parentNode->OnFloorSelected(callFloorName);
		} 
		else 
		{
			parentNode->OnFloorSelected(targetFloorName);
		}
	}
}

void AElevatorButton::UpdateText(FString _newText) {
	textToShow->SetText(FText::FromString(_newText));
}