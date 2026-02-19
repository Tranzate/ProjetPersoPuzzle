// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/WidgetComponent.h>

#include "Components/BillboardComponent.h"
#include "HookPoint.generated.h"

class UHookDataAsset;
class APuzzleCharacter;
class UHookPointDataAsset;

// UENUM(BlueprintType)
// enum class EHookType : uint8
// {
// 	Swing     UMETA(DisplayName = "Swing"),
// 	Propulsion     UMETA(DisplayName = "Propulsion"),
// 	All     UMETA(DisplayName = "All")
// };

//
// UCLASS()
// class PROJPERSOPUZZLE_API AHookPoint : public AActor
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) TObjectPtr<UStaticMeshComponent> mesh = nullptr;
//
// 	UPROPERTY(EditAnywhere) EHookType hookToUse = EHookType::All;
// 	UPROPERTY(EditAnywhere) TObjectPtr<UWidgetComponent> hookUI = nullptr;
// 	UPROPERTY(EditAnywhere) float delayTimerForSpam = 2.f;
// 	UPROPERTY(EditAnywhere) bool canDoPropulsion = true;
//
// public:
//
// 	AHookPoint();
//
// 	UFUNCTION(BlueprintPure, Category = "Settings")
// 	EHookType GetHookToUse() const { return hookToUse; }
// 	void ShowHookUI(bool _visible);
// 	void StartTimer();
// 	FORCEINLINE void SetCanDoPropulsion(const bool _canDoPropulsion) { canDoPropulsion = _canDoPropulsion; }
// 	FORCEINLINE bool GetCanDoPropulsion() { return canDoPropulsion; }
// protected:
// 	virtual void BeginPlay() override;
// 	virtual void Tick(float DeltaTime) override;
// 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//
//
// };

UENUM(BlueprintType)
enum class EHookType : uint8
{
	Swing UMETA(DisplayName = "Swing"),
	Propulsion UMETA(DisplayName = "Propulsion"),
	All UMETA(DisplayName = "All")
};

UCLASS()
class PROJPERSOPUZZLE_API AHookPoint : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Settings", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APuzzleCharacter> player = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> mesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBillboardComponent> offsetPosition = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings")
	EHookType hookToUse = EHookType::All;

	UPROPERTY(VisibleAnywhere, Category = "Settings|Debug")
	bool canDoPropulsion = true;

	UPROPERTY(EditAnywhere, Category = "Settings", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHookDataAsset> hookDataAsset;

	UPROPERTY(EditAnywhere, Category = "Settings", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHookPointDataAsset> hookPointDataAsset;


	UPROPERTY(VisibleAnywhere, Category = "Settings")
	TObjectPtr<UWidgetComponent> hookUI = nullptr;

	//TODO
	// UPROPERTY(VisibleAnywhere, Category = "Settings")
	// TObjectPtr<UCurrentHookPointWidget> currentHookUI = nullptr;

	// UPROPERTY(EditAnywhere, Category = "Settings",
	// 	meta = (ToolTip =
	// 		"Node that is optional, you can add one if you want to add a sound (for example) when colliding with the artifact"
	// 	))
	// TObjectPtr<ABaseNode> nodeToTrigger = nullptr;

public:
	AHookPoint();

	UFUNCTION(BlueprintPure, Category = "Settings")
	EHookType GetHookToUse() const { return hookToUse; }

	UFUNCTION(BlueprintCallable)
	void ShowHookUI(bool _visible);
	void StartTimer();
	FORCEINLINE void SetCanDoPropulsion(const bool _canDoPropulsion) { canDoPropulsion = _canDoPropulsion; }
	FORCEINLINE bool GetCanDoPropulsion() { return canDoPropulsion; }
	void OnUse();

	FORCEINLINE FVector GetOffsetPosition() const { return offsetPosition->GetComponentLocation(); }


	void HideKeyUI(const bool _value);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ShowKeyToUse();
};
