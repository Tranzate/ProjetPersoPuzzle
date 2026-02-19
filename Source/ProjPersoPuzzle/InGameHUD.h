#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;
class UDebugMenuWidget;

UCLASS()
class PROJPERSOPUZZLE_API AInGameHUD : public AHUD
{
    GENERATED_BODY()

public:

    
    void ToggleDebugMenu();

    FORCEINLINE UDebugMenuWidget* GetDebugMenuWidget() const { return debugMenuWidget; }
    FORCEINLINE UInGameWidget* GetInGameWidget() const { return inGameWidget; }

protected:

    UPROPERTY(EditAnywhere, Category = "UI|Classes")
    TSubclassOf<UInGameWidget> inGameWidgetClass = nullptr;
    

    UPROPERTY()
    TObjectPtr<UInGameWidget> inGameWidget = nullptr;

    UPROPERTY()
    TObjectPtr<UDebugMenuWidget> debugMenuWidget = nullptr;


    virtual void BeginPlay() override;
};