#pragma once

#include "CoreMinimal.h"
#include "GPE/InteractibleActor.h"
#include "Components/TextRenderComponent.h"
#include "ElevatorButton.generated.h"

class ANodeElevator;

UCLASS()
class PROJPERSOPUZZLE_API AElevatorButton : public AInteractibleActor
{
	GENERATED_BODY()
    
public: 
	AElevatorButton();

	UPROPERTY(EditAnywhere, Category = "Elevator")
	FString targetFloorName;
	
	UPROPERTY(EditAnywhere, Category = "Elevator")
	bool isCallButton = false;

	UPROPERTY(EditAnywhere, Category = "Elevator", meta = (EditCondition = "isCallButton"))
	FString callFloorName;

	UPROPERTY(EditAnywhere, Category = "Elevator")
	ANodeElevator* parentNode;

	void UpdateText(FString _newText);
	virtual void Interact(class APuzzleCharacter* _player) override;
};
