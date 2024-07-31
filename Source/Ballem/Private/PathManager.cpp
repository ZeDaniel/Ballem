// Fill out your copyright notice in the Description page of Project Settings.


#include "PathManager.h"
#include "Path.h"

// Sets default values
APathManager::APathManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APathManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APathManager::AddNewPath(USplineComponent* NewSpline)
{
	FTransform PathTransform;

	auto NewPath = GetWorld()->SpawnActorDeferred<APath>(APath::StaticClass(), PathTransform);

	NewPath->SetSpline(NewSpline);
	NewPath->SetMesh(PathMesh);

	NewPath->SetOwner(this);

	NewPath->PopulatePathWithMesh();

	NewPath->Tags.Add(TEXT("SplinePath"));

	PathArray.Add(NewPath);

	NewPath->FinishSpawning(PathTransform);
}

void APathManager::RemoveLastPath()
{
	if (!PathArray.IsEmpty() && PathArray.Last())
	{
		PathArray.Last()->DestroyPath();
		PathArray.Pop(EAllowShrinking::Yes);
	}
}

