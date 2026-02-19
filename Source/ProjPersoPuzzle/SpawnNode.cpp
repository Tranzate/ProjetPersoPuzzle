#include "SpawnNode.h"

#include "Utility.h"

ASpawnNode::ASpawnNode()
{
}


void ASpawnNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnNode::ShowDebug()
{
	Super::ShowDebug();
	if (!showDebug) return;

	for (FSpawnParameters _toSpawn : toSpawn)
	{
		DrawDebugSphere(GetWorld(), _toSpawn.spawnLocation, 60.f, 8, FColor::Yellow, false, -1.f, 0, 2.f);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), _toSpawn.spawnLocation, 
			   150.f, FColor::Cyan, false, -1.f, 0, 5.0f);
	}
}

void ASpawnNode::OnActivate(AActor* _other)
{
	if (spawnedActors.Num() > 0 && !canBeReactivate) return;
	UWorld* _world = GetWorld();
	if (!_world) return;

	if (destroyActorWhenReActivated)
		DestoyAllActor();
	
	for (const FSpawnParameters& _params : toSpawn)
	{
		if (_params.actorToSpawn)
		{
			FTransform _transform(_params.spawnRotation, _params.spawnLocation, _params.spawnScale);
			FActorSpawnParameters _spawnRules;
			_spawnRules.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* _newActor = _world->SpawnActor<AActor>(_params.actorToSpawn, _transform, _spawnRules);
			if (_newActor)
			{
				spawnedActors.Add(_newActor);
				if (_params.lifeTime > 0.0f) _newActor->SetLifeSpan(_params.lifeTime);
			}
		}
	}
	PlayNodeFeedback();
}

void ASpawnNode::OnDeActivate(AActor* _other)
{
	if (!destroyActorWhenDeActivated) return;
	DestoyAllActor();

}

void ASpawnNode::DestoyAllActor()
{
	for (int32 i = spawnedActors.Num() - 1; i >= 0; i--)
	{
		if (spawnedActors[i].IsValid())
		{
			spawnedActors[i]->Destroy();
		}
	}
	spawnedActors.Empty();
}