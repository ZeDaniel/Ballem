// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystems.h>
#include "EnhancedInputComponent.h"
#include "Components/SplineComponent.h"
#include "Path.h"

APlayerPawn::APlayerPawn()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	BallemPlayerController = Cast<APlayerController>(GetController());

	if (BallemPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(BallemPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BallemMappingContext, 0);
		}
	}
}

void APlayerPawn::Build(const FInputActionValue& Value)
{
	if (!BuildInProgress)
	{
		StartBuild();
	}
	else
	{
		ContinueBuild();
	}
}

void APlayerPawn::StartBuild()
{
	if (BallemPlayerController)
	{
		FHitResult HitResult;
		BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 10.f);

		BuildInProgress = true;

		NewSpline = NewObject<USplineComponent>();
		NewSpline->ClearSplinePoints();
		NewSpline->AddSplinePoint(HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);

		UE_LOG(LogTemp, Display, TEXT("Build func started"));
	}	
}

void APlayerPawn::ContinueBuild()
{
	if (BallemPlayerController)
	{
		FHitResult HitResult;
		BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 10.f);
		NewSpline->AddSplinePoint(HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);

		UE_LOG(LogTemp, Display, TEXT("Build func continued"));
	}
}

void APlayerPawn::EndBuild(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Build func ended"));

	auto Path = GetWorld()->SpawnActor<APath>();
	Path->SetSpline(NewSpline);
	
	Path->SetOwner(this);

	BuildInProgress = false;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(BuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::Build);
		EnhancedInputComponent->BindAction(EndBuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::EndBuild);
	}
}


void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BallemPlayerController)
	{
		/*FHitResult HitResult;
		BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 3.f);*/
	}
}


