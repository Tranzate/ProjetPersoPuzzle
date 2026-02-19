// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"
#include "HookPoint.h"
#include "HookDataAsset.generated.h"

/**
 * 
 */
// UCLASS()
// class PROJPERSOPUZZLE_API UHookDataAsset : public UDataAsset
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(EditAnywhere, Category = "Settings") float distanceMax = 0.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings") float distanceMin = 0.f;
//
// 	UPROPERTY(EditAnywhere, Category = "Settings|Debug") bool debugMaxDistance = false;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Debug") bool debugMinDistance = false;
//
// 	UPROPERTY(EditAnywhere, Category = "Settings") float hookAttachTolerenceDot = 0.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Swing") float maxHoldToSwing = 0.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Swing", meta = (EditCondition = "!keepDistanceForSwing")) float swingLenght = 0.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Propulsion") float propulsionStrenght = 0.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings") bool canSwitchHookInHands = false;
// 	UPROPERTY(EditAnywhere, Category = "Settings") EHookType HookInHandsBase = EHookType::All;
//
// 	UPROPERTY(EditAnywhere, Category = "Settings|Propulsion") float heightAtEndPropulsion = 25.f;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Swing") bool keepDistanceForSwing = false;
//
// 	UPROPERTY(EditAnywhere, Category = "Settings|Niagara") TObjectPtr<UNiagaraSystem> playerNiagaraSystem = nullptr;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Niagara") TObjectPtr<UNiagaraSystem> ropeNiagaraSystem = nullptr;
// 	UPROPERTY(EditAnywhere, Category = "Settings|Niagara") float delayDespawnNiagara = 2.f;
//
//
//
// };
class UNiagaraComponent; // Juste au cas où

UCLASS(BlueprintType)
class UHookDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

#pragma region Distance_Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Distance",
		meta = (ClampMin = "0.0", ClampMax = "10000.0", UIMin = "0.0", UIMax = "10000.0",
			ToolTip = "Maximum distance allowed between the player and a valid hook point."))
	float distanceMax = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Distance",
		meta = (ClampMin = "0.0", ClampMax = "10000.0", UIMin = "0.0", UIMax = "10000.0",
			ToolTip = "Maximum distance allowed between the player and a valid hook point."))
	float distanceMaxDetection = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Distance",
		meta = (ClampMin = "0.0", ClampMax = "10000.0", UIMin = "0.0", UIMax = "10000.0",
			ToolTip = "Minimum distance required for a hook point to be considered valid."))
	float distanceMin = 0.f;

#pragma endregion


#pragma region Scoring_Settings

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Scoring",
			meta = (ClampMin = "0.0", UIMin = "0.0",
				ToolTip = "Multiplier applied to the dot product when calculating the score of a potential hook point. Higher values prioritize facing alignment over distance."))
	float scoreDotMultiplier = 0.000001f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Scoring",
		meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0",
			ToolTip = "Controls how closely the player must aim toward the hook point. 1 = perfectly aligned, 0 = wide angle."))
	float hookAttachTolerenceDot = 0.f;

#pragma endregion


#pragma region General_Settings

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|General",
			meta = (ToolTip = "If true, allows the player to switch hook types while holding the hook (not used in current logic)."))
	bool canSwitchHookInHands = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|General",
		meta = (ToolTip = "Defines which hook type the player starts with."))
	EHookType hookInHandsBase = EHookType::All;



#pragma endregion


#pragma region Swing_Settings

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing",
			meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0.0", UIMax = "5.0",
				ToolTip = "How long the player must hold the hook button before switching from propulsion/pull to swing mode."))
	float maxHoldToSwing = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing",
		meta = (EditCondition = "!keepDistanceForSwing", ClampMin = "0.0", ClampMax = "10000.0", UIMin = "0.0", UIMax = "10000.0",
			ToolTip = "The fixed length of the rope during swing. Only active when 'Keep Distance For Swing' is false."))
	float swingLenght = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing",
		meta = (ToolTip = "If true, keeps the current distance between player and hook when starting the swing."))
	bool keepDistanceForSwing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Correction",
		meta = (ClampMin = "0.0", UIMin = "0.0",
			ToolTip = "Speed used for VInterpConstantTo to snap the player back to the max rope length when pulling away."))
	float swingCorrectionPullForce = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Force",
		meta = (ClampMin = "-100.0", ClampMax = "100.0",
			ToolTip = "Multiplier for the force applied to counteract velocity projected along the rope (pulling inward). Negative values help maintain swinging speed."))
	float swingForceProjectedVelocity = -3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Force",
		meta = (ClampMin = "0.0", UIMin = "0.0",
			ToolTip = "Strength of the horizontal force derived from the camera's forward input vector during the swing."))
	float swingForceCameraInput = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Force",
		meta = (ClampMin = "0.0", UIMin = "0.0",
			ToolTip = "Force applied to correct velocity. This force is nulled when the Roll angle is between Min/Max Roll Correction."))
	float swingForceVelocityCorrection = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Correction",
		meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-90.0", UIMax = "90.0",
			ToolTip = "Minimum Roll angle (in degrees) at which the Velocity Correction Force is disabled."))
	float swingForceCorrectionMinRoll = -10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Correction",
		meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-90.0", UIMax = "90.0",
			ToolTip = "Maximum Roll angle (in degrees) at which the Velocity Correction Force is disabled."))
	float swingForceCorrectionMaxRoll = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Correction",
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0",
			ToolTip = "Friction factor applied while swinging. Higher values increase air resistance and slow the player down faster. When we don't move with ZQSD"
			))
	float frictionFactorWhenNoMoving = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing|Correction",
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0",
			ToolTip = "Friction factor applied while swinging. Higher values increase air resistance and slow the player down faster. When we move with ZQSD"
			))
	float frictionFactorWhenMoving = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing",
		meta = (ToolTip = "If enabled, the player receives a boost at the end of the swing."
			))
	bool canLaunchAtEndSwing = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Swing",
		meta = (ClampMin = "1.0", ClampMax = "1000.0", UIMin = "1.0", UIMax = "1000.0",
			EditCondition = "canLaunchAtEndSwing",
			ToolTip ="Multiplier applied to the launch force when the swing ends. Higher values create a stronger boost."
			))
	float forceMultiplierAtEndSwing = 1.5f;

#pragma endregion


#pragma region Propulsion_Settings

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Propulsion|Force",
			meta = (ClampMin = "0.0", ClampMax = "10000.0", UIMin = "0.0", UIMax = "10000.0",
				ToolTip = "Defines how strong the propulsion force is when launching toward a hook point."))
	float propulsionStrenght = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Propulsion|Force",
		meta = (ClampMin = "-10000.0", ClampMax = "10000.0", UIMin = "-10000.0", UIMax = "10000.0",
			ToolTip = "Adds or removes vertical offset to the launch force. Can be used to make the player jump higher or lower."))
	float heightAtEndPropulsion = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Propulsion|Movement",
		meta = (ClampMin = "0.0", UIMin = "0.0",
			ToolTip = "Braking deceleration set on the Character Movement Component immediately after the propulsion launch. This helps control the landing."))
	float propulsionBrakingDeceleration = 1500.f;

#pragma endregion


#pragma region Niagara_Settings

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Niagara",
			meta = (ToolTip = "Niagara system spawned on the player when the hook is active (often a charging or boosting effect)."))
	TObjectPtr<UNiagaraSystem> playerNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Niagara",
		meta = (ToolTip = "Niagara system used to render or simulate the rope between player and hook point."))
	TObjectPtr<UNiagaraSystem> ropeNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Niagara",
		meta = (ToolTip = "A"))
	TObjectPtr<UNiagaraSystem> trailNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Niagara",
		meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0.0", UIMax = "5.0",
			ToolTip = "Delay before Niagara effects are automatically destroyed after unhooking (used mainly for propulsion)."))
	float delayDespawnNiagara = 2.f;

#pragma endregion


#pragma region Debug_Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|General|Debug",
		meta = (ToolTip = "If true, displays a debug sphere showing the maximum hook range for the player."))
	bool debugMaxDistance = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|General|Debug",
		meta = (ToolTip = "If true, displays a debug sphere showing the minimum hook range for the player."))
	bool debugMinDistance = false;


#pragma endregion
};