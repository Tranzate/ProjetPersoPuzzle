#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;
class UDebugMenuWidget;
class UPauseWidget;

UCLASS()
class PROJPERSOPUZZLE_API AInGameHUD : public AHUD
{
    GENERATED_BODY()

public:

    
    void ToggleDebugMenu();
    void TogglePauseMenu();

    FORCEINLINE UDebugMenuWidget* GetDebugMenuWidget() const { return debugMenuWidget; }
    FORCEINLINE UInGameWidget* GetInGameWidget() const { return inGameWidget; }
    FORCEINLINE UPauseWidget* GetPauseWidget() const { return pauseWidget; }

protected:

    UPROPERTY(EditAnywhere, Category = "UI|Classes")
    TSubclassOf<UInGameWidget> inGameWidgetClass = nullptr;
    

    UPROPERTY()
    TObjectPtr<UInGameWidget> inGameWidget = nullptr;

    UPROPERTY()
    TObjectPtr<UDebugMenuWidget> debugMenuWidget = nullptr;

    UPROPERTY()
    TObjectPtr<UPauseWidget> pauseWidget = nullptr;


    virtual void BeginPlay() override;
};