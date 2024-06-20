// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalBasket.h"
#include "Components/BoxComponent.h"
#include "BallemGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGoalBasket::AGoalBasket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	RootComponent = BoxComp;

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AGoalBasket::OnOverlapBegin);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AGoalBasket::OnOverlapEnd);

	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Goal Mesh"));
	GoalMesh->SetupAttachment(BoxComp);

}

// Called when the game starts or when spawned
void AGoalBasket::BeginPlay()
{
	Super::BeginPlay();
	
	BallemGameMode = Cast<ABallemGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AGoalBasket::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Overlap begin"));
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag(BallTag))
	{
		if (BallCount == 0)
		{
			BallemGameMode->UpdateGoals(1);
		}
		BallCount++;
	}
}

//TODO: Figure out if one ball leaves but another doesn't? Unlikely but technically this is wrong right now
void AGoalBasket::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("Overlap end"));
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag(BallTag))
	{
		if(BallCount == 1)
		BallemGameMode->UpdateGoals(-1);
	}
	BallCount--;
}

// Called every frame
void AGoalBasket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

