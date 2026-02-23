#include "UI/GameEditorUtilityWidget.h"
#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "TimerManager.h"

// void UGameEditorUtilityWidget::NativeConstruct()
// {
//     Super::NativeConstruct();
//     
//     // Nettoyage pour éviter les doublons
//     FEditorDelegates::BeginPIE.RemoveAll(this);
//     FEditorDelegates::EndPIE.RemoveAll(this);
//
//     // Liaison aux événements
//     FEditorDelegates::BeginPIE.AddUObject(this, &UGameEditorUtilityWidget::OnPlaySessionStarted);
//     FEditorDelegates::EndPIE.AddUObject(this, &UGameEditorUtilityWidget::OnPlaySessionStopped);
//     
//     UE_LOG(LogTemp, Warning, TEXT("Widget Initialisé"));
// }
//
// void UGameEditorUtilityWidget::NativeDestruct()
// {
//     // Indispensable pour corriger l'erreur LNK2001
//     FEditorDelegates::BeginPIE.RemoveAll(this);
//     FEditorDelegates::EndPIE.RemoveAll(this);
//
//     Super::NativeDestruct();
// }
//
// void UGameEditorUtilityWidget::OnPlaySessionStarted(bool bIsSimulating)
// {
//     UE_LOG(LogTemp, Error, TEXT("!!! PIE STARTED !!!"));
//
//     // Réactiver le widget au niveau de Slate pour éviter qu'il soit grisé
//     TSharedPtr<SWidget> SlateWidget = TakeWidget();
//     if (SlateWidget.IsValid())
//     {
//         SlateWidget->SetEnabled(true);
//     }
//
//     // Timer pour le PlayerController
//     GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
//     {
//         if (UWorld* World = GetWorld())
//         {
//             if (APlayerController* PC = World->GetFirstPlayerController())
//             {
//                 PC->bShowMouseCursor = true;
//                 
//                 FInputModeGameAndUI InputMode;
//                 InputMode.SetWidgetToFocus(TakeWidget());
//                 InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//                 PC->SetInputMode(InputMode);
//
//                 // Focus Slate pour dégriser l'UI
//                 if (FSlateApplication::IsInitialized())
//                 {
//                     FSlateApplication::Get().SetUserFocus(0, TakeWidget());
//                 }
//             }
//         }
//     });
// }
//
// void UGameEditorUtilityWidget::OnPlaySessionStopped(bool bIsSimulating)
// {
//     // Indispensable pour corriger l'erreur LNK2019
//     UE_LOG(LogTemp, Warning, TEXT("PIE STOPPED"));
// }