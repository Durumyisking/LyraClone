// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LCCameraMode.generated.h"


// 최종적으로 카메라가 렌더링 하는 영역
struct FLCCameraModeView
{
	FLCCameraModeView();

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};


// CameraBlending 대상 유닛
UCLASS(Abstract, NotBlueprintable)
class LYRACLONE_API ULCCameraMode : public UObject
{
	GENERATED_BODY()
public:
	ULCCameraMode(const FObjectInitializer& ObjectInitializer);

	/*
 * Member Methods
 */
	void UpdateCameraMode(float DeltaTime);

	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	ULCCameraComponent* GetLCCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;
	
/*
 * Member variables
 */
	FLCCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;
	
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/* 선형적인 Blend 값 [0~1]*/
	float BlendAlpha;

	// CameraMode의 최종 Blending값
	float BlendWeight;
	
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

	ULCCameraMode* GetCameraModeInstance(TSubclassOf<ULCCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<ULCCameraMode> CameraModeClass);
	void EvaluateStack(float DeltaTime, FLCCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FLCCameraModeView& OutCameraModeView) const;	
	
	// 생성된 CameraMode를 관리
	UPROPERTY()
	TArray<TObjectPtr<ULCCameraMode>> CameraModeInstance;

	// Camera Matrix Blend 업데이트 진행 큐
	UPROPERTY()
	TArray<TObjectPtr<ULCCameraMode>> CameraModeStack;
};