// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LCExperienceActionSet.generated.h"

class UGameFeatureAction;
/**
 * 
 */
UCLASS()
class LYRACLONE_API ULCExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	ULCExperienceActionSet();

	/*
	 * Member Variable
	 */
	UPROPERTY(EditAnywhere, Category="Actions To Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
	
};
