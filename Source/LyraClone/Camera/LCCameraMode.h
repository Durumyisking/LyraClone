// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LCCameraMode.generated.h"

/**
 * 
 */
// CameraBlending 대상 유닛
UCLASS(Abstract, NotBlueprintable)
class LYRACLONE_API ULCCameraMode : public UObject
{
	GENERATED_BODY()
public:
	ULCCameraMode(const FObjectInitializer& ObjectInitializer);
	
};

// Camera Blending을 담당하는 객체
// 말 그대로 Camera를 배열에 저장하니까 CameraStack
UCLASS()
class ULCCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	ULCCameraModeStack(const FObjectInitializer& ObjectInitializer);

	/*
	 * Member Variables
	 */

	// 생성된 CameraMode를 관리
	UPROPERTY()
	TArray<TObjectPtr<ULCCameraMode>> CameraModeInstance;

	// Camera Matrix Blend 업데이트 진행 큐
	UPROPERTY()
	TArray<TObjectPtr<ULCCameraMode>> CameraModeStack;
};