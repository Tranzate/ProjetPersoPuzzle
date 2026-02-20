// Fill out your copyright notice in the Description page of Project Settings.

#include "HookPointSubsystem.h"
#include "GPE/HookPoint.h"
#include "Utility.h"


void UHookPointSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

}

void UHookPointSubsystem::Deinitialize()
{
	allHookPoint.Empty();
	Super::Deinitialize();
}

void UHookPointSubsystem::RegisterPoint(TObjectPtr<AHookPoint> _point)
{
	if (!IsValid(_point))
		return;

	if (!allHookPoint.Contains(_point))
	{
		allHookPoint.Add(_point);
	}
}

void UHookPointSubsystem::UnRegisterPoint(TObjectPtr<AHookPoint> _point)
{

	if (!IsValid(_point))
		return;

	if (allHookPoint.Contains(_point))
	{
		allHookPoint.Remove(_point);

	}
}
