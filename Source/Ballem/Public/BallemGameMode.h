// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BallemGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BALLEM_API ABallemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	class APathManager* GetBallemPathManager() { return BallemPathManager; }

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Paths")
	TSubclassOf<APathManager> BallemPathManagerClass;

	class APathManager* BallemPathManager;
	
};
