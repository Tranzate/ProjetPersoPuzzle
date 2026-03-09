// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivableActor.h"

AActivableActor::AActivableActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AActivableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AActivableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

