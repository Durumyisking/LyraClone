// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LCGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ALCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALCGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience);
};
