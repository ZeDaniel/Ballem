// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Components\SplineMeshComponent.h>

// Sets default values
APath::APath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Path Spline"));
	RootComponent = PathSpline;

	SplineMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("Spline Mesh"));

}

void APath::SetMesh(UStaticMesh* NewSplineMesh)
{
	UE_LOG(LogTemp, Display, TEXT("Spline mesh set"));

	if (NewSplineMesh)
	{
		SplineMesh = NewSplineMesh;
	}
}

void APath::PopulatePathWithMesh()
{
	FBox MeshBox = SplineMesh->GetBoundingBox();
	float XLength = (MeshBox.Min.GetAbs() + MeshBox.Max.GetAbs()).X;

	for (int i = 0; i < PathSpline->GetSplineLength() / XLength; i++)
	{
		//attempt to create new spline mesh component
		USplineMeshComponent* NewSplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		if (NewSplineMeshComponent)
		{
			//attach and add component
			NewSplineMeshComponent->RegisterComponent();
			NewSplineMeshComponent->SetMobility(EComponentMobility::Movable);
			NewSplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
			AddInstanceComponent(NewSplineMeshComponent);

			//Move static mesh into correct place
			NewSplineMeshComponent->SetupAttachment(PathSpline);
			NewSplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			NewSplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X);
			NewSplineMeshComponent->SetStaticMesh(SplineMesh);

			FVector StartPos = PathSpline->GetLocationAtDistanceAlongSpline(i * XLength, ESplineCoordinateSpace::Local);
			FVector StartTan = PathSpline->GetTangentAtDistanceAlongSpline(i * XLength, ESplineCoordinateSpace::Local).GetClampedToSize(0.f, XLength);
			FVector EndPos = PathSpline->GetLocationAtDistanceAlongSpline((i + 1) * XLength, ESplineCoordinateSpace::Local);
			FVector EndTan = PathSpline->GetTangentAtDistanceAlongSpline((i + 1) * XLength, ESplineCoordinateSpace::Local).GetClampedToSize(0.f, XLength);

			NewSplineMeshComponent->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);
		}
		
	}
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

}

