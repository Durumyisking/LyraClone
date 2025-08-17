// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LCExperienceDefinition.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ULCExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	ULCExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<class ULCPawnData> DefaultPawnData;

	/*
		아래 변수는 단순히 마킹 및 기억용으로 남겨둔다.
		- 게임모드에 따른 GameFeature plugin을 로딩하는데 이에 대한 연결고리라고 생각하면 된다.
		- 후일 ShooterCore 관련 Plugin할때 사용할거임
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;

	// ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay용도에 맞게 분류의 목적으로 사용한다.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<class ULCExperienceActionSet>> ActionSets;

	// 일반적인 GameFeatureAction으로서 추가
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TArray<TObjectPtr<class UGameFeatureAction>> Actions; 

};
