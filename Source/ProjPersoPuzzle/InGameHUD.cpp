#include "InGameHUD.h"
#include "InGameWidget.h"
#include "DebugMenuWidget.h"
#include "Utility.h"
#include "Blueprint/UserWidget.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (inGameWidgetClass)
	{
		inGameWidget = CreateWidget<UInGameWidget>(GetOwningPlayerController(), inGameWidgetClass);
		if (inGameWidget)
		{
			inGameWidget->AddToViewport(0);
			debugMenuWidget = inGameWidget->GetDebugMenuWidget();
			debugMenuWidget->SetHudOwner(this);
		}
	}
}

void AInGameHUD::ToggleDebugMenu()
{
	if (!debugMenuWidget) return;

	if (debugMenuWidget->GetVisibility() == ESlateVisibility::Collapsed)
		debugMenuWidget->SetVisibility(ESlateVisibility::Visible);
	else if (debugMenuWidget->GetVisibility() == ESlateVisibility::Visible)
		debugMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	


}
