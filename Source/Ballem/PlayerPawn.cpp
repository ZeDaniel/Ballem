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

	BallemGameMode = Cast<ABallemGameMode>(UGameplayStatics::GetGameMode(this));

	if (BallemGameMode)
	{
		BallemPathManager = BallemGameMode->GetBallemPathManager();
	}

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
			//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1000.f);
			AddBuildPoint(HitResult.ImpactPoint);

			BuildInProgress = true;
			BallemGameMode->AddBuildControls();

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
			//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1000.f);
			AddBuildPoint(HitResult.ImpactPoint);
			NewSpline->AddSplinePoint(HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);

			UE_LOG(LogTemp, Display, TEXT("Build func continued"));
		}	
	}
}

void APlayerPawn::EndBuild(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Build func ended"));

	//FlushPersistentDebugLines(GetWorld());
	ClearBuildPoints();

	BallemPathManager->AddNewPath(NewSpline);

	BuildInProgress = false;
	BallemGameMode->RemoveBuildControls();
}

void APlayerPawn::CancelBuild()
{
	if (BuildInProgress)
	{
		//FlushPersistentDebugLines(GetWorld());
		ClearBuildPoints();

		NewSpline->DestroyComponent();

		BuildInProgress = false;
		BallemGameMode->RemoveBuildControls();
	}
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

void APlayerPawn::UpdatePathHoverContext(bool AddContext)
{
	if (BallemPlayerController && PathHoverContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(BallemPlayerController->GetLocalPlayer()))
		{
			//Add path hover context with higher priority to override original context
			if (AddContext)
			{
				Subsystem->AddMappingContext(PathHoverContext, 1);
				UE_LOG(LogTemp, Display, TEXT("Hover context added"));
			}
			//remove context
			else
			{
				Subsystem->RemoveMappingContext(PathHoverContext);
				UE_LOG(LogTemp, Display, TEXT("Hover context removed"));
			}
		}		
	}
}

void APlayerPawn::DeletePath()
{
	if (HoveredPath && BallemPathManager)
	{
		BallemPathManager->RemovePath(HoveredPath);
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(BuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::Build);
		EnhancedInputComponent->BindAction(EndBuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::EndBuild);
		EnhancedInputComponent->BindAction(CancelBuildAction, ETriggerEvent::Triggered, this, &APlayerPawn::CancelBuild);
		EnhancedInputComponent->BindAction(StartSimulateAction, ETriggerEvent::Triggered, this, &APlayerPawn::StartSimulation);
		EnhancedInputComponent->BindAction(UndoAction, ETriggerEvent::Triggered, this, &APlayerPawn::UndoPath);
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &APlayerPawn::Reset);
		EnhancedInputComponent->BindAction(MoveCursorAction, ETriggerEvent::Triggered, this, &APlayerPawn::MoveCursor);
		EnhancedInputComponent->BindAction(DeletePathAction, ETriggerEvent::Triggered, this, &APlayerPawn::DeletePath);
	}
}

void APlayerPawn::AddBuildPoint(FVector PointLocation)
{
	if (BuildPointClass)
	{
		auto NewBuildPoint = GetWorld()->SpawnActor<AActor>(BuildPointClass, PointLocation, FRotator::ZeroRotator);

		if (NewBuildPoint)
		{
			BuildPointArray.Add(NewBuildPoint);
		}
	}
}

void APlayerPawn::ClearBuildPoints()
{
	for (AActor* BuildPoint : BuildPointArray)
	{
		BuildPoint->Destroy();
	}
	BuildPointArray.Empty();
}

void APlayerPawn::CheckForHoveredPath()
{
	//Don't check for hover if we're currently building
	if (!BuildInProgress)
	{
		if (!HoveringPath && BallemPlayerController)
		{
			FHitResult HitResult;
			BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

			//If cursor is over a spline path
			if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(TEXT("SplinePath")))
			{
				UpdatePathHoverContext(true);

				HoveredPath = Cast<APath>(HitResult.GetActor());

				//Toggle highlight on path
				HoveredPath->ToggleHighlight();

				HoveringPath = true;
				BallemGameMode->AddHoverControls();
			}
		}
		else if (HoveringPath)
		{
			FHitResult HitResult;
			BallemPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

			if (HitResult.GetActor() != HoveredPath)
			{
				UpdatePathHoverContext(false);

				///Toggle highlight on path
				HoveredPath->ToggleHighlight();
				//Clear hovered path
				HoveredPath = nullptr;

				HoveringPath = false;
				BallemGameMode->RemoveHoverControls();
			}
		}
	}

}


void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForHoveredPath();
}


