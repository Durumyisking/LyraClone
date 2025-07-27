// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LCCameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"

ULCCameraMode_ThirdPerson::ULCCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void ULCCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = PivotRotation;
	View.FieldOfView = FieldOfView;

	// TargetOffsetCurve가 오버라이드 되어있으면 Curve에 값을 가져와서 적용 진행
	// -Camera관점에서 Character의 어느 부분에 Target으로 할 지 결정하는 것으로 이해하면 됨
	if (TargetOffsetCurve)
	{
		// Pitch값에 따른 Offset값 가져옴
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		// 현재 액터 위치로부터 회전방향으로 더해줌
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset); 
	}
	
}
