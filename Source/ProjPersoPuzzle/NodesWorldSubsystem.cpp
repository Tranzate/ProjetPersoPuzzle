// Fill out your copyright notice in the Description page of Project Settings.


#include "NodesWorldSubsystem.h"

#include "Node.h"

void UNodesWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UNodesWorldSubsystem::Deinitialize()
{
		nodes.Empty();
	Super::Deinitialize();
}

void UNodesWorldSubsystem::Register(ANode* _node)
{
	if (!IsValid(_node))
		return;

	if (!nodes.Contains(_node))
	{
		nodes.Add(_node);
	}
}

void UNodesWorldSubsystem::DeRegister(ANode* _node)
{
	if (!IsValid(_node))
		return;

	if (nodes.Contains(_node))
	{
		nodes.Remove(_node);
	}
}

ANode* UNodesWorldSubsystem::GetNodeByID(int _nodeID)
{
	const int _count = nodes.Num();
	for (int _i = 0; _i <_count; _i++)
	{
		if (nodes[_i]->GetId() == _nodeID)
		{
			return nodes[_i];
		}
	}
	return nullptr;
	
}

void UNodesWorldSubsystem::ShowAllDebugText(const bool _value)
{
	const int _count = nodes.Num();
	for (int _i = 0; _i <_count; _i++)
	{
		nodes[_i]->SetVisibiltyDebugText(_value);
	}
}
