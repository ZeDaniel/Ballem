// Fill out your copyright notice in the Description page of Project Settings.


#include "LemBall.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALemBall::ALemBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LemBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LemBall Mesh"));
	SetRootComponent(LemBallMesh);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComp->SetupAttachment(LemBallMesh);
}

void ALemBall::EnableBallPhysics()
{
	LemBallMesh->SetSimulatePhysics(true);
}

void ALemBall::DisableBallPhysics()
{
	LemBallMesh->SetSimulatePhysics(false);
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

	LemBallMesh->OnComponentHit.AddDynamic(this, &ALemBall::OnComponentHit);
}

void ALemBall::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (NormalImpulse.GetAbs().GetMax() > 10000)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
}

// Called every frame
void ALemBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

