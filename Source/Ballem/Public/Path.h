// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Path.generated.h"

UCLASS()
class BALLEM_API APath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APath();

	void SetSpline(class USplineComponent* NewSpline) { UE_LOG(LogTemp, Display, TEXT("Spline set") ); PathSpline = NewSpline; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	class USplineComponent* PathSpline;

	//DEBUG. REMOVE LATER
	bool SplineDrawn = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
