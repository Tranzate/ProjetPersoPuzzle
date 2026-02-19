#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "SpawnNode.generated.h"

USTRUCT(BlueprintType)
struct FSpawnParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TSubclassOf<AActor> actorToSpawn = nullptr;
	UPROPERTY(EditAnywhere) FVector spawnLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere) FRotator spawnRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere) FVector spawnScale = FVector::OneVector;
	UPROPERTY(EditAnywhere) float lifeTime = 0.0f;
};

UCLASS()
class PROJPERSOPUZZLE_API ASpawnNode : public ANode
{
	GENERATED_BODY()

public:
	ASpawnNode();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	TArray<FSpawnParameters> toSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	TArray<TWeakObjectPtr<AActor>> spawnedActors;

	UPROPERTY(EditAnywhere, Category = "Spawn Settings") 
	bool destroyActorWhenDeActivated = false;
	UPROPERTY(EditAnywhere, Category = "Spawn Settings") 
	bool destroyActorWhenReActivated = false;

	UPROPERTY(EditAnywhere, Category = "Spawn Settings") 
	bool canBeReactivate = false;

	virtual void ShowDebug() override;
	virtual void OnActivate(AActor* _other) override;
	virtual void OnDeActivate(AActor* _other) override;
	virtual void DestoyAllActor();
};


