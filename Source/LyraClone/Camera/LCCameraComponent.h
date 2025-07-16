// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LCCameraComponent.generated.h"


// Template Forward Declare
template <class TClass> class TSubclassOf;

						// return type						// Delegatename
DECLARE_DELEGATE_RetVal(TSubclassOf<class ULCCameraMode>, FLCCameraModeDelegate)


UCLASS()
class LYRACLONE_API ULCCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	ULCCameraComponent(const FObjectInitializer& ObjectInitializer);
	

	// CaemraComponent Interface
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	// Member Method
	static ULCCameraComponent* FindCameraComponent(class AActor* Actor) { return Actor? Actor->FindComponentByClass<ULCCameraComponent>() : nullptr; }
	void UpdateCameraModes();
	
	// Member Variables
	// Camera Blending 기능을 지원하는 Stack
	UPROPERTY()
	TObjectPtr<class ULCCameraModeStack> CameraModeStack;

	// 현재 CameraMdoe를 가져오는 Delegate
	FLCCameraModeDelegate DetermineCameraModeDelegate;
};
