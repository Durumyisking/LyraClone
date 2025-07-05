// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LCPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ALCPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	/*
	 * Actor's Interface
	*/
	virtual void PostInitializeComponents() final;

	/*
	 * member method
	*/

	void OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience);

	/*
	 * 나중에 Gas쓸때 AbilitySystemComponent를 얘가 들고있을건데
	 * 그때 PawnData를 통해 능력을 부여할거기 때문에 미리 캐싱
	*/
	UPROPERTY()
	TObjectPtr<const class ULCPawnData> PawnData;
};
