// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystems.h>
#include "EnhancedInputComponent.h"
#include "Components/SplineComponent.h"
#include "Path.h"
#include "Kismet\GameplayStatics.h"
#include "BallemGameMode.h"
#include "PathManager.h"
#include "LemBall.h"

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

	BallemPathManager = Cast<ABallemGameMode>(UGameplayStatics::GetGameMode(this))->GetBallemPathManager();

	FTimerHandle StartTimer;
	FTimerDelegate StartTimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerPawn::FindBalls);
	GetWorldTimerManager().SetTimer(StartTimer, StartTimerDelegate, 0.5f, false);
}

void APlayerPawn::FindBalls()
{
	TArray<AActor*> BallsToFind;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALemBall::StaticClass(), BallsToFind);

	for (AActor* Actor : BallsToFind)
	{
		ALemBall* Ball = Cast<ALemBall>(Actor);
		LemBallArray.Add(Ball);
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

		if (!(HitResult.GetActor()->ActorHasTag(TEXT("SplinePath"))))
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1000.f);

			BuildInProgress = true;

			NewSpline = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
			NewSpline->ClearSplinePoints();
			NewSpline->AddSplinePoint(HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);

			UE_LOG(LogTemp, Display, TEXT("Build func started"));
		}
	}	
}

void APlayerPawn::ContinueBuild()
{
	if (BallemPlayerController)
	{
		FHitResult HitResult;
		BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		

		//Check validity of impact point. Is it far enough? Is it hitting a path?
		FSplinePoint LastPoint =  NewSpline->GetSplinePointAt(NewSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		if ((HitResult.ImpactPoint - LastPoint.Position).Length() > MinSplinePointDistance && !(HitResult.GetActor()->ActorHasTag(TEXT("SplinePath"))))
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1000.f);
			NewSpline->AddSplinePoint(HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);

			UE_LOG(LogTemp, Display, TEXT("Build func continued"));
		}	
	}
}

void APlayerPawn::EndBuild(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Build func ended"));

	FlushPersistentDebugLines(GetWorld());

	BallemPathManager->AddNewPath(NewSpline);

	BuildInProgress = false;
}

void APlayerPawn::StartSimulation(const FInputActionValue& Value)
{
	for (ALemBall* Ball : LemBallArray)
	{
		Ball->EnableBallPhysics();
	}
}

void APlayerPawn::UndoPath(const FInputActionValue& Value)
{
	BallemPathManager->RemoveLastPath();
}

void APlayerPawn::Reset(const FInputActionValue& Value)
{
	for (ALemBall* Ball : LemBallArray)
	{
		Ball->DisableBallPhysics();
		Ball->ResetPosition();
	}
}

void APlayerPawn::MoveCursor(const FInputActionValue& Value)
{
	FVector2D CursorAxisVector = Value.Get<FVector2D>();


	if (BallemPlayerController)
	{
		double NewMouseX;
		double NewMouseY;
		BallemPlayerController->GetMousePosition(NewMouseX, NewMouseY);

		UE_LOG(LogTemp, Display, TEXT("Controller input: X:%f, Y:%f"), CursorAxisVector.X, CursorAxisVector.Y);

		NewMouseX += CursorAxisVector.X * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		NewMouseY += CursorAxisVector.Y * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

		BallemPlayerController->SetMouseLocation(round(NewMouseX), round(NewMouseY));
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(BuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::Build);
		EnhancedInputComponent->BindAction(EndBuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::EndBuild);
		EnhancedInputComponent->BindAction(StartSimulateAction, ETriggerEvent::Triggered, this, &APlayerPawn::StartSimulation);
		EnhancedInputComponent->BindAction(UndoAction, ETriggerEvent::Triggered, this, &APlayerPawn::UndoPath);
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &APlayerPawn::Reset);
		EnhancedInputComponent->BindAction(MoveCursorAction, ETriggerEvent::Triggered, this, &APlayerPawn::MoveCursor);
	}
}


void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


