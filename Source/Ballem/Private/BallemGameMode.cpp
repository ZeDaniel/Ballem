// Fill out your copyright notice in the Description page of Project Settings.

#include "BallemGameMode.h"
#include "Engine/World.h"
#include "PathManager.h"
#include "Kismet/GameplayStatics.h"
#include "GoalBasket.h"

void ABallemGameMode::BeginPlay()
{
	Super::BeginPlay();

	BallemPathManager = GetWorld()->SpawnActor<APathManager>(BallemPathManagerClass);

	FTimerHandle StartTimer;
	FTimerDelegate StartTimerDelegate = FTimerDelegate::CreateUObject(this, &ABallemGameMode::CalculateGoalsToWin);
	GetWorldTimerManager().SetTimer(StartTimer, StartTimerDelegate, 0.5f,false);
}

void ABallemGameMode::UpdateGoals(int AddValue)
{
	GoalsCompleted += AddValue;
	CheckForWin();
}

void ABallemGameMode::CalculateGoalsToWin()
{
	TArray<AActor*> ActorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalBasket::StaticClass(), ActorsFound);
	GoalsToWin = ActorsFound.Num();
}

void ABallemGameMode::CheckForWin()
{
	if (GoalsCompleted == GoalsToWin)
		WinGame();
}
