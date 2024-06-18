// Fill out your copyright notice in the Description page of Project Settings.

#include "BallemGameMode.h"
#include "Engine/World.h"
#include "PathManager.h"

void ABallemGameMode::BeginPlay()
{
	Super::BeginPlay();

	BallemPathManager = GetWorld()->SpawnActor<APathManager>(BallemPathManagerClass);
}
