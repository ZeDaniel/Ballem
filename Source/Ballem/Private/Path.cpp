// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"
#include "Components/SplineComponent.h"

// Sets default values
APath::APath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PathSpline && !SplineDrawn)
	{
		UE_LOG(LogTemp, Display, TEXT("Spline present!"));
		for (int i = 0; i < PathSpline->GetSplineLength(); i += 50)
		{
			DrawDebugSphere(GetWorld(), PathSpline->GetWorldLocationAtDistanceAlongSpline(i), 20, 6, FColor::Red, true);
		}
		SplineDrawn = true;
	}
}

