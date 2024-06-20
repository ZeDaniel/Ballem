// Fill out your copyright notice in the Description page of Project Settings.


#include "LemBall.h"
#include "Components/SphereComponent.h"

// Sets default values
ALemBall::ALemBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	RootComponent = SphereComp;

	LemBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LemBall Mesh"));
	LemBallMesh->SetupAttachment(SphereComp);

}

void ALemBall::EnableBallPhysics()
{
	SphereComp->SetSimulatePhysics(true);
}

void ALemBall::DisableBallPhysics()
{
	SphereComp->SetSimulatePhysics(false);
}

void ALemBall::ResetPosition()
{
	SetActorLocationAndRotation(StartPosition, StartRotation);
}

// Called when the game starts or when spawned
void ALemBall::BeginPlay()
{
	Super::BeginPlay();
	
	DisableComponentsSimulatePhysics();

	StartPosition = GetActorLocation();
	StartRotation = GetActorRotation();
}

// Called every frame
void ALemBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

