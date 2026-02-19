#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyPlayerController.generated.h"

/**
 * PlayerController gérant l'ouverture et la navigation du Debug Menu via Enhanced Input
 */

class UDebugMenuWidget;
class UInputAction;
class UInputMappingContext;
class AInGameHUD;

UCLASS()
class PROJPERSOPUZZLE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	AInGameHUD* ingameHUD = nullptr;
	
private:
#pragma region Input
#pragma region MenuInput
	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputMappingContext* debugMenuContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* toggleMenuAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuUpAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuDownAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuLeftAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuRightAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuConfirmAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|DebugMenu")
	UInputAction* menuBackAction = nullptr;

#pragma endregion MenuInput
#pragma endregion Input

protected:
	virtual void BeginPlay() override;

#pragma region Input
	virtual void SetupInputComponent() override;
#pragma region MenuInput
	void ToggleMenu();
	void MenuUp();
	void MenuDown();
	void MenuLeft();
	void MenuRight();
	void MenuConfirm();
	void MenuBack();
#pragma endregion MenuInput
#pragma endregion Input

private:
	bool bIsDebugMenuOpen = false;

	UDebugMenuWidget* GetDebugMenu();
};
