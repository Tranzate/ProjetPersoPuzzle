// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugMenuSaveGame.h"

TArray<FString> UDebugMenuSaveGame::GetAllName()
{
	TArray<FString> _result;

	const int _count = savedWaypoints.Num();
	for (int i = 0; i < _count; ++i)
	{
		_result.Add(savedWaypoints[i].name);
	}
	return _result;
}
