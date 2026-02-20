#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "LogicNode.generated.h"

class AActivableActor;

UENUM(BlueprintType)
enum class ELogicMode : uint8
{
	And UMETA(DisplayName = "ET (All inputs ON)"),
	Or UMETA(DisplayName = "OU (At least one ON)"),
	Not UMETA(DisplayName = "NON (Invert input)"),
	Xor UMETA(DisplayName = "XOR (Odd inputs ON)")
};

UCLASS()
class PROJPERSOPUZZLE_API ALogicNode : public ANode
{
	GENERATED_BODY()

public:
	ALogicNode();

	UPROPERTY(EditAnywhere, Category = "Logic Settings")
	ELogicMode logicMode = ELogicMode::And;

	UPROPERTY(EditAnywhere, Category = "Logic Settings")
	int32 requiredActivations = 1;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Logic Output")
	TArray<AActivableActor*> targets;

	virtual void OnActivate(AActor* _other) override;
	virtual void OnDeActivate(AActor* _other) override;

#if WITH_EDITOR
	virtual void Tick(float DeltaTime) override;
#endif

	virtual void ShowDebug() override;

private:
	UPROPERTY(EditAnywhere, Category = "Logic Settings")
	int32 currentActivations = 0;
	bool lastOutputStatus = false;

	void EvaluateLogic(AActor* _other);

public:
	virtual void ForceActivate() override;
	virtual void ForceDeActivate() override;
};
