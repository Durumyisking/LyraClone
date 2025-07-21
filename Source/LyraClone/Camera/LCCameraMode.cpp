// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LCCameraMode.h"

#include "LCPlayerCameraManager.h"

FLCCameraModeView::FLCCameraModeView()
	:Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(LC_CAMERA_DEFAULT_FOV)
{
}

ULCCameraMode::ULCCameraMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

ULCCameraModeStack::ULCCameraModeStack(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

ULCCameraMode* ULCCameraModeStack::GetCameraModeInstance(TSubclassOf<ULCCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeClass로 등록된 CameraModeInstance가 있는지 확인한다.
	// 싱글톤 같은거라고 보면됨
	for (ULCCameraMode* CameraMode : CameraModeInstance)
	{
		// CameraMode는 UClass를 비교한다.
		// 즉 CameraMode는 클래스 타입당 하나만 생기게 된다.
		if ( (nullptr != CameraMode) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass에 맞는 Instance가 없다면 생성한다.
	ULCCameraMode* NewCameraMode = NewObject<ULCCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 여기서 CameraModeInstance는 CameraModeClass에 맞는 인스턴스를 관리하는 컨테이너라는 것을 알 수 있다.
	CameraModeInstance.Add(NewCameraMode);

	return NewCameraMode;
}

void ULCCameraModeStack::PushCameraMode(TSubclassOf<ULCCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}
	
	ULCCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0 )&& (CameraModeStack[0] == CameraMode))
	{
		// 가장 최신 데이터와 똑같은 CameraMode가 Push된것이므로 작업 해줄 필요 없다
		return;
	}
		
	// ExistingStackIndex는 CameraModeStack에서 CameraMode에 맞는 Index를 찾음
	// ExistingStackContribution은 위에서 아래로 최종 BlendWeight 값을 찾기 위해 초기값으로 1.0으로 설정
	int32 ExistingStackIndex = INDEX_NONE /*-1*/;
	float ExistingStackContribution = 1.0f;

	/**
	 * 맨 위가 제일 최신의 데이터
	 * BlendWeight    |    ExistingStackContribution    |    ExistingStackContribution (accumulated)
	 * 0.1f           |    (1.0f) * 0.1f = 0.1f         |    (1.0f - 0.1f) = 0.9f
	 * 0.3f           |    (0.9f) * 0.3f = 0.27f        |    (1.0f - 0.3f) * 0.9f = 0.63f
	 * 0.6f           |    (0.63f) * 0.6f = 0.378f      |    (1.0f - 0.6f) * 0.63f = 0.252f
	 * 1.0f           |    (0.252f) * 1.0f = 0.252f     | // 반드시 1.0이다.
	 *                |    0.1f + 0.27f + 0.378f + 0.252f = 1.0f!
	 */
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode) // push한 카메라 모드가 이미 존재하면!
		{
			ExistingStackIndex = StackIndex;
			// - 여기서 ExistingStackContribution 우리가 찾는 CameraMode를  찾았으니깐, 누적된 BlendWeight와 함께 BlendWeight를 곱하여, 루프를 탈출한다.
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			// 원하는 CamearMode가 아니면, InvBlendWeight = (1.0 - BlendWeight)를 곱해줘야, 값이 누적되겠징?
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// 해당 루프의 동작 원리는 앞서 적어놓은 표를 확인해보며 이해해보자.

	// 인자로 들어온 CameraMode를 Top으로 만들어야 하기 때문에 제거하여 다시 Push 해준다.
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// 새로 들어온 CameraMode이기 때문에 0부터 시작해서 점점 보간되어야한다.
		ExistingStackContribution = 0.0f;
	}

	// BlendTime이 0보다 크다는 것은 Blend을 얼마 시간동안 진행함을 의미 따라서, ExistingStackContribution을 적용
	// - 따라서 Blend하지 않는다면, BlendWeight를 1.0을 넣어 새로 넣는 CameraMode만 적용할 것이다
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	// 인자로 들어온 CameraMode를 0번인덱스에 넣는다.
	CameraModeStack.Insert(CameraMode, 0);

	// 마지막은 항상 1.0이 되어야 함!
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void ULCCameraModeStack::EvaluateStack(float DeltaTime, FLCCameraModeView& OutCameraModeView)
{
	// Top -> Bottom [0 -> Num]까지 순차적으로 Stack에 있는 Camera Update
	UpdateStack(DeltaTime);

	// Bottom -> Top까지 CameraModeStack에 대해 Blending 진행
	BlendStack(OutCameraModeView);
}

void ULCCameraModeStack::UpdateStack(float DeltaTime)
{
	// 카메라 모드 스택이 비어있으면 할 필요 없음
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack을 순회하며, CameraMode를 업데이트한다 (가장 최신 데이터부터 가장 나중 데이터까지 순회)
	// 현재 내가 쓰고있는 데이터부터 썼던데이터들을 쭉 돌아보는것
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		ULCCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// UpdateCameraMode를 확인해보자:
		CameraMode->UpdateCameraMode(DeltaTime);

		// 만약 하나라도 CameraMode가 BlendWeight가 1.0에 도달했다면, 그 이후 CameraMode를 제거한다
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// 생각해보면 이거 때문에 Pop하고 Push를 안한거일지도?
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}

}

void ULCCameraModeStack::BlendStack(FLCCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack은 Bottom -> Top 순서로 Blend를 진행한다
	const ULCCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// 참고로 가장 아래 (Bottom)은 BlendWeight가 1.0이다!
	OutCameraModeView = CameraMode->View;

	// 이미 Index = [StackSize - 1] 이미 OutCameraModeView로 지정했으므로, StackSize - 2부터 순회하면 된다
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// HakCameraModeView Blend 함수를 구현하자:
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}

}
