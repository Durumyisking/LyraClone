// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LCCameraComponent.h"

#include "LCCameraMode.h"

ULCCameraComponent::ULCCameraComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void ULCCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		// 초기화(Beginplay같은)가 필요없는 객체이기 때문에 NewObject로 할당
		// Constructor가 아니라 OnRegister에서 하기 때문에 LCCharacter에 CameraComponent가 부착되고 난 다음에 CameraModeStack이 생성된다.
		CameraModeStack = NewObject<ULCCameraModeStack>(this);		
	}
}

// 매 프레임 호출!
void ULCCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);
	
	UpdateCameraModes();

	// EvaluateStack은 CameraStack에 있는 CameraMode를 업데이트, 블렌딩 하고 CameraModeStack의 Bottom->Top까지 업데이트된 CameraMode에 대해 보간을 진행한다.
	// 이에 대한 결과는 CameraModeView에 캐싱된다.
	FLCCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);
}

void ULCCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate is Bound To HeroComponent
	if (DetermineCameraModeDelegate.IsBound())
	{
		if (const TSubclassOf<ULCCameraMode> CameraMode = DetermineCameraModeDelegate.Execute()) // 현재 pawndata의 DefaultCameramode를 return한다.
		{
			CameraModeStack->PushCameraMode(CameraMode);	
		}
	}
}