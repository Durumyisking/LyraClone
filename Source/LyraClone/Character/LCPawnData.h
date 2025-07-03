// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LCPawnData.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ULCPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	ULCPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/*
		어떤 폰을 생성할지
		입력은 어떻게 받을지
		스킬은 어떻게 쓸지등의 정보를 가진다.	
	*/


};
