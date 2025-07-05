// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LCGameState.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ALCGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ALCGameState();

public:
	UPROPERTY()
	TObjectPtr<class ULCExperienceManagerComponent> ExperienceManagerComponent;
	
};
