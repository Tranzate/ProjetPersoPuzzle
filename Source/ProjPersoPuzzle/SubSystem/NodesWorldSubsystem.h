// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"
#include "NodesWorldSubsystem.generated.h"

class ANode;

UCLASS

()
class PROJPERSOPUZZLE_API UNodesWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere) TArray<ANode*> nodes = TArray<ANode*>();

protected:
protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
public:
	UFUNCTION(BlueprintCallable)
	void Register(ANode* _node);
	UFUNCTION(BlueprintCallable)
	void DeRegister(ANode* _node);

	UFUNCTION(BlueprintCallable)
	TArray<ANode*> GetAllNodes(){return nodes;}
	int GetNumNodes(){return nodes.Num();}
	
	UFUNCTION(BlueprintCallable)
	ANode* GetNodeByID(int _nodeID);

	UFUNCTION(BlueprintCallable)
	void ShowAllDebugText(const bool _value);
};


