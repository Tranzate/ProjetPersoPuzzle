#include "MyPlayerController.h"

#include "DebugMenuWidget.h"
#include "InGameHUD.h"
#include "Utility.h"


void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ingameHUD = Cast<AInGameHUD>(GetHUD());

	if (ULocalPlayer* _lp = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* _sys = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                _sys->AddMappingContext(debugMenuContext, 0); 
            }
        }
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	ULocalPlayer* _lp = GetLocalPlayer();
	if (!_lp) return;
    
	UEnhancedInputLocalPlayerSubsystem* _inputSystem = _lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!_inputSystem) return;
	
	UEnhancedInputComponent* _eIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!_eIC) return;
	if (toggleMenuAction)
		_eIC->BindAction(toggleMenuAction, ETriggerEvent::Started, this, &AMyPlayerController::ToggleMenu);
	
	if (menuUpAction)
		_eIC->BindAction(menuUpAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuUp);
    
	if (menuDownAction)
		_eIC->BindAction(menuDownAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuDown);

	if (menuConfirmAction)
		_eIC->BindAction(menuConfirmAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuConfirm);

	if (menuBackAction)
		_eIC->BindAction(menuBackAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuBack);
        
	if (menuLeftAction)
		_eIC->BindAction(menuLeftAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuLeft);
        
	if (menuRightAction)
		_eIC->BindAction(menuRightAction, ETriggerEvent::Started, this, &AMyPlayerController::MenuRight);
}

void AMyPlayerController::ToggleMenu()
{
	ingameHUD->ToggleDebugMenu();
}

void AMyPlayerController::MenuUp()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->MoveSelection(-1);
}

void AMyPlayerController::MenuDown()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->MoveSelection(1);
}

void AMyPlayerController::MenuLeft()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->MoveActionSelection(-1);
}

void AMyPlayerController::MenuRight()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->MoveActionSelection(1);
}

void AMyPlayerController::MenuConfirm()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->SelectItem();

}

void AMyPlayerController::MenuBack()
{
	if (ingameHUD->GetDebugMenuWidget()->GetVisibility() == ESlateVisibility::Visible)
	ingameHUD->GetDebugMenuWidget()->GoBack();

}

