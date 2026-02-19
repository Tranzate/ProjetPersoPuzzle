#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
//#include "SR_GameInstance.h"

// ======== Utility ========
#define TOP TObjectPtr
#define WORLD GetWorld()
#define TOP_WORLD(variableName) TOP<UWorld> variableName = WORLD
#define CAST(classType, variableName, variable) TOP<classType> variableName = Cast<classType>(variable)
#define OWNER GetOwner()
#define TOP_OWNER(variableName) TOP<AActor> variableName = GetOwner()
#define PAWN_OWNER Cast<APawn>(OWNER)
#define TOP_PAWN_OWNER(variableName) TOP<APawn> variableName = PAWN_OWNER
#define OWNER_LOCATION OWNER->GetActorLocation()
#define OWNER_ROTATION OWNER->GetActorRotation()
#define DELTATIME WORLD->DeltaTimeSeconds
#define SUBSYSTEM(type) WORLD->GetGameInstance()->GetSubsystem<type>()
#define TOP_SUBSYSTEM(type, variableName) TOP<type> variableName = WORLD->GetGameInstance()->GetSubsystem<type>()
#define UKSL UKismetSystemLibrary
#define UKML UKismetMathLibrary
#define ONE_FRAME_DELAY(worldContext, lambdaBody) if (!worldContext || !worldContext->GetWorld()) return; worldContext->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() { lambdaBody; }))
#define TIMER(worldContext, handleVariable, lambdaBody, delay) if (!worldContext || !worldContext->GetWorld()) return; worldContext->GetWorld()->GetTimerManager().SetTimer(handleVariable, FTimerDelegate::CreateLambda([this]() { lambdaBody; }), delay, false);
#define TIMER_LOOP(worldContext, handleVariable, lambdaBody, initialDelay, loopDelay) if (!worldContext || !worldContext->GetWorld()) return; worldContext->GetWorld()->GetTimerManager().SetTimer(handleVariable, FTimerDelegate::CreateLambda([this]() { lambdaBody; }), loopDelay, true, initialDelay);
#define PAUSE_TIMER(handleVariable) GetWorld()->GetTimerManager().PauseTimer(handleVariable);
#define CLEAR_TIMER(handleVariable) GetWorld()->GetTimerManager().ClearTimer(handleVariable);
#define UNPAUSE_TIMER(handleVariable) GetWorld()->GetTimerManager().UnPauseTimer(handleVariable);

//#define GAMEINSTANCE Cast<USR_GameInstance>(WORLD->GetGameInstance())
// ======== Utility ========

// ======== LOGGING ========
#define LOG_CUSTOM(message, color) UKSL::PrintString(this, FPaths::GetBaseFilename(TEXT(__FILE__)) + "(" + FString::FromInt(__LINE__) + ")=> " + message, true, true, FColor::color, 20.0f)
#define LOG(message) LOG_CUSTOM(message, Green)
#define LOG_WARN(message) LOG_CUSTOM(message, Yellow)
#define LOG_ERROR(message) LOG_CUSTOM(message, Red)
#define FRAME_LOG(message) UKSL::PrintString(this, FPaths::GetBaseFilename(TEXT(__FILE__)) + "(" + FString::FromInt(__LINE__) + ")=> " + message, true, true, FColor::Magenta, DELTATIME)
#define FRAME_LOG_CUSTOM(message, color) UKSL::PrintString(this, FPaths::GetBaseFilename(TEXT(__FILE__)) + "(" + FString::FromInt(__LINE__) + ")=> " + message, true, true, FColor::color, DELTATIME)
// ======== LOGGING ========

// ======== NETWORK ========
#define SERVER HasAuthority()
#define IS_SERVER(actor) actor->HasAuthority()
#define CLIENT !SERVER
#define IS_CLIENT(actor) !IS_SERVER(actor)
#define SELF IsLocallyControlled()
#define IS_SELF(pawn) pawn->IsLocallyControlled()
#define GET_INSTANCE_PAWN WORLD->GetFirstPlayerController()->GetPawn()
#define REPLICATE(methodName, actor) if (IS_SERVER(actor)) Multi_##methodName(); else Server_##methodName()
#define REPLICATE_MOVEMENT(methodName, position, rotation) TOP<AActor> _actor = GetOwner(); if (IS_SERVER(_actor)) Multi_##methodName(_actor, position, rotation); else Server_##methodName(_actor, position, rotation)
// ======== NETWORK ========