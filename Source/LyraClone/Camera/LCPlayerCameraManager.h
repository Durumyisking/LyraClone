// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LCPlayerCameraManager.generated.h"

/**
 * Controller에 바인딩된 CameraManager
 */

#define LC_CAMERA_DEFAULT_FOV (90.f)
#define LC_CAMERA_DEFAULT_PITCH_MIN (-89.f)
#define LC_CAMERA_DEFAULT_PITCH_MAX (89.f)

UCLASS()
class LYRACLONE_API ALCPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	ALCPlayerCameraManager(const FObjectInitializer& ObjectInitializer);
};
