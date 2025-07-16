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
	//Super::GetCameraView(DeltaTime, DesiredView);

	check(CameraModeStack);
	
	UpdateCameraModes();
}

void ULCCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate is Bound To HeroComponent
	if (DetermineCameraModeDelegate.IsBound())
	{
		if (const TSubclassOf<ULCCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			// CameraModeStack->PushCameraMode(CameraMode);	
		}
	}
}