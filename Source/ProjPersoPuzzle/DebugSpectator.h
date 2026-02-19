#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "InputActionValue.h"
#include "DebugSpectator.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJPERSOPUZZLE_API ADebugSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ADebugSpectator();

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> spectatorImc;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> lookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> flyAction; // Pour monter/descendre

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Fonctions de traitement des inputs
	UFUNCTION() void Move(const FInputActionValue& Value);
	UFUNCTION() void Look(const FInputActionValue& Value);
	UFUNCTION() void Fly(const FInputActionValue& Value);
	virtual void NotifyRestarted() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	void RemoveMapping();
};