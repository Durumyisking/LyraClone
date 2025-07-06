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

	// BPNativeEvent 매크로 처리 되어있음
	// BP에서는 바로 재정의 가능하고
	// cpp에서는 함수이름_Implementation 이렇게 만든 함수에다가 재정의해야함
	// cpp재정의 해놓으면 bp보다 cpp우선 호출
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;
	
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience);
};
