// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "InputActionValue.h"
#include "PlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class BALLEM_API APlayerPawn : public ABasePawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* BallemMappingContext; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* BuildAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* EndBuildAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* StartSimulateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* UndoAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ResetAction;

	void BeginPlay() override;

	void Build(const FInputActionValue& Value);

	void StartBuild();

	void ContinueBuild();

	void EndBuild(const FInputActionValue& Value);

	void StartSimulation(const FInputActionValue& Value);

	void UndoPath(const FInputActionValue& Value);

	void Reset(const FInputActionValue& Value);

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components");
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components");
	class UCameraComponent* Camera;

	class USplineComponent* NewSpline;

	bool BuildInProgress = false;

	APlayerController* BallemPlayerController;
	
	class APathManager* BallemPathManager;

	TArray<class ALemBall*> LemBallArray;
};
