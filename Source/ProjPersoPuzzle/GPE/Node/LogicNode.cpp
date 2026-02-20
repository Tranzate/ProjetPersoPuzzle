#include "LogicNode.h"
#include "GPE/ActivableActor.h"
#include "Utility.h"

ALogicNode::ALogicNode()
{
	logicMode = ELogicMode::And;
	requiredActivations = 1;
	currentActivations = 0;
	lastOutputStatus = false;
}

void ALogicNode::OnActivate(AActor* _other)
{
	Super::OnActivate(_other);
	currentActivations++;
	EvaluateLogic(_other);
}

void ALogicNode::OnDeActivate(AActor* _other)
{
	currentActivations = currentActivations - 1;
	if (currentActivations < 0) currentActivations = 0; 
	Super::OnDeActivate(_other);
	EvaluateLogic(_other);
}

void ALogicNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ALogicNode::ShowDebug()
{
	Super::ShowDebug();
	if (!showDebug) return;
	for (AActivableActor* _target : targets)
	{
		if (_target)
		{
			DrawDebugSphere(GetWorld(), _target->GetActorLocation(), 60.f, 8, FColor::Yellow, false, -1.f, 0, 2.f);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), _target->GetActorLocation(), 
			   150.f, FColor::Cyan, false, -1.f, 0, 5.0f);
		}
	}
}

void ALogicNode::EvaluateLogic(AActor* _other)
{
	bool _newStatus = false;

	switch (logicMode)
	{
	case ELogicMode::And: _newStatus = (currentActivations >= requiredActivations); break;
	case ELogicMode::Or:  _newStatus = (currentActivations > 0); break;
	case ELogicMode::Not: _newStatus = (currentActivations == 0); break;
	case ELogicMode::Xor: _newStatus = (currentActivations % 2 != 0); break;
	}
    
	if (_newStatus != lastOutputStatus)
	{
		lastOutputStatus = _newStatus;
		
		Super::SendSignal(_newStatus, _other);
		
		for (AActivableActor* _target : targets)
		{
			if (_target)
			{
				if (_newStatus) _target->OnActivate(_other);
				else _target->OnDeActivate(_other);
			}
		}
	}
}

void ALogicNode::ForceActivate()
{
	currentActivations = requiredActivations;
	EvaluateLogic(this);
}

void ALogicNode::ForceDeActivate()
{
	currentActivations = 0;
	EvaluateLogic(this);
}
