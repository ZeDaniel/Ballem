// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathManager.generated.h"

UCLASS()
class BALLEM_API APathManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathManager();

	void AddNewPath(class USplineComponent* NewSpline);

	void RemoveLastPath();

	void RemovePath(class APath* PathToRemove);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<class APath*> PathArray;

private:

	UPROPERTY(EditAnywhere, Category = "Paths")
	UStaticMesh* PathMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
