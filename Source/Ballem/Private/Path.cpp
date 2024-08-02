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

	for (int i = 0; i < trunc(PathSpline->GetSplineLength()) / XLength; i++)
	{
		//attempt to create new spline mesh component
		USplineMeshComponent* NewSplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		if (NewSplineMeshComponent)
		{
			//Get direction at spline and rotate to prevent mesh twists
			//FVector SplineDirection = PathSpline->GetWorldDirectionAtDistanceAlongSpline(i * XLength);
			FVector SplineDirection = PathSpline->GetWorldDirectionAtDistanceAlongSpline(i * XLength);
			FVector NewSplineDirection = SplineDirection.RotateAngleAxis(-90, FVector(0, 1, 0));
			NewSplineMeshComponent->SetSplineUpDir(NewSplineDirection);


			//attach and add component
			NewSplineMeshComponent->RegisterComponent();
			NewSplineMeshComponent->SetMobility(EComponentMobility::Movable);
			NewSplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
			AddInstanceComponent(NewSplineMeshComponent);

			//Move static mesh into correct place
			NewSplineMeshComponent->SetupAttachment(PathSpline);
			NewSplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			NewSplineMeshComponent->SetSimulatePhysics(true);
			NewSplineMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
			NewSplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X);
			NewSplineMeshComponent->SetStaticMesh(SplineMesh);


			FVector StartPos = PathSpline->GetWorldLocationAtDistanceAlongSpline(i * XLength);
			//FVector StartTan = PathSpline->GetWorldDirectionAtDistanceAlongSpline(i * XLength).GetClampedToSize(0.f, XLength);
			FVector StartTan = PathSpline->GetWorldTangentAtDistanceAlongSpline(i * XLength).GetClampedToSize(0.f, XLength);
			FVector EndPos = PathSpline->GetWorldLocationAtDistanceAlongSpline((i + 1) * XLength);
			//FVector EndTan = PathSpline->GetWorldDirectionAtDistanceAlongSpline((i + 1) * XLength).GetClampedToSize(0.f, XLength);
			FVector EndTan = PathSpline->GetWorldTangentAtDistanceAlongSpline((i + 1) * XLength).GetClampedToSize(0.f, XLength);

			NewSplineMeshComponent->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);
		}
	}
}

void APath::ToggleHighlight()
{
	UE_LOG(LogTemp, Display, TEXT("Toggle Highlight called"));

	TArray<USceneComponent*> SplineComponents;

	PathSpline->GetChildrenComponents(false, SplineComponents);

	for (USceneComponent* ChildComponent : SplineComponents)
	{
		//Attempt to cast comopnent to spline mesh. set custom render pass if able
		if (USplineMeshComponent* ChildSplineMesh = Cast<USplineMeshComponent>(ChildComponent))
		{
			if (IsHighlighted)
			{
				ChildSplineMesh->SetRenderCustomDepth(false);
			}
			else
			{
				ChildSplineMesh->SetRenderCustomDepth(true);
			}
		}
	}

	IsHighlighted = !IsHighlighted;
}

void APath::DestroyPath()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for(AActor* Actor : AttachedActors)
	{
		if(Actor)
			Actor->Destroy();
	}
	Destroy();
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

