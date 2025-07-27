// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LCCameraMode.h"
#include "LCCameraMode_ThirdPerson.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LYRACLONE_API ULCCameraMode_ThirdPerson : public ULCCameraMode
{
	GENERATED_BODY()
public:
	ULCCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer);

	/*
	 * ULCCameraMode Interface
	 */
	virtual void UpdateView(float DeltaTime) override;

	/*
	 * Member Variables
	 */

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const class UCurveVector> TargetOffsetCurve;
	
};
