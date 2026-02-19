#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h" 
#include "InteractComponent.generated.h"

class UGrabComponent;
// Forward declarations
class UCameraComponent;
class AInteractibleActor;
class APuzzleCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJPERSOPUZZLE_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(visibleAnywhere) TObjectPtr<APuzzleCharacter> owner = nullptr;
	UPROPERTY(visibleAnywhere) TObjectPtr<UGrabComponent> grabComponent = nullptr;
	
public:    
	UInteractComponent();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void PerformInteractionCheck();

	void SetCanLineTraceTick(bool _value);

	UFUNCTION() void Interact(const FInputActionValue& Value);


private:
	AActor* GetActorInLookDirection() const;

	void UpdateCurrentTarget(AActor* _newTarget);

	void ProcessInteractibleFeedback(AActor* _target, bool _showFeedback);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float lineTraceDist = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool canLineTraceTick = true;

	UPROPERTY()
	TObjectPtr<AActor> currentActorCibling;

	UPROPERTY()
	TObjectPtr<UCameraComponent> cameraPlayer;




};